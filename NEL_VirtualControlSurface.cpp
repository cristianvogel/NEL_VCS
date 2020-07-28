// NEL_Virtual Control Surface
// 𝌺 Created by Cristian Vogel / NeverEngineLabs 2020

#include "NEL_VirtualControlSurface.h"
#include "NEL_OSC_Methods.hpp"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "tiny-process/process.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <sstream>
#include <mutex>

using namespace iplug;



NEL_VirtualControlSurface::NEL_VirtualControlSurface(const InstanceInfo &info)
    : Plugin(info, MakeConfig(kNumParams, kNumPresets))
    , nelosc( "localhost", 8080)

{
    consoleText = cnsl[kMsgScanning];
    nelosc.launchNetworkingThread();
    
    InitParamRange(kDualDialInner, kDualDialInner + NBR_DUALDIALS - 1, 1, "Dual Dial %i", 0, 0., 1., 0, "%", 0, "Inner Value");
    InitParamRange(kDualDialOuter, kDualDialOuter + NBR_DUALDIALS - 1, 1, "Dual Dial %i", 0, 0., 1., 0, "%", 0, "Outer Value");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
    mMakeGraphicsFunc = [&]()
    {
        return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_HEIGHT));
    };
  
  
#pragma mark Layout lambda function initialiser
    mLayoutFunc = [&](IGraphics * pGraphics)
    {
        // start layout lambda function
        // load all dependencies once here
        pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
        pGraphics->AttachPanelBackground(NEL_TUNGSTEN);
        pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
        pGraphics->LoadFont("Menlo", MENLO_FN);
        
  
      const iplug::igraphics::IVStyle rescanButtonStyle = rescanButtonStyleDef();


#pragma mark mainCanvas
        // main app GUI IRECT
      
      const IRECT b = pGraphics->GetBounds().GetScaledAboutCentre(0.95f).FracRectVertical(0.9f, true);
        const IRECT consoleBounds = pGraphics->GetBounds().GetFromBottom( 24.f ).GetGridCell(0, 1, 1);

#pragma mark console text
        //▼ small network logging console outputs OSC messages and host
      
        consoleTextDef = IText ( 12.f, "Menlo").WithFGColor(NEL_TUNGSTEN_FGBlend);
        pGraphics->AttachControl(new ITextControl
                                 (consoleBounds,
                                  consoleText.c_str(),
                                  consoleTextDef,
                                  NEL_TUNGSTEN,
                                  true
                                  )
                                 , kCtrlNetStatus);
      
#pragma mark dual dials
        //▼ rows of dual concentric dials with two paramIdx
      
        for (int d = 0; d < NBR_DUALDIALS; d++)
        {
          const IRECT dualDialBounds = b.GetGridCell( d , 2 , NBR_DUALDIALS/2 ).GetScaledAboutCentre(0.75f);
          
            pGraphics->AttachControl
            (new NEL_DualDial(
                 dualDialBounds
                 , {kDualDialInner + d, kDualDialOuter + d}
                 , getSwatch( Lunada, d % 3)
                 , getSwatch( Lunada, (d + 1) % 3)
                 , getSwatch( Lunada, (d + 2) % 3)
             ), kCtrlFluxDial + d)
              ->As<NEL_DualDial>()->setButtonStates(pGraphics->LoadSVG(NEL_BUTTON_ON), pGraphics->LoadSVG(NEL_BUTTON_OFF));
    
      
#pragma mark text input fields for changing send messages
      
            const IRECT& editableTextBounds = dualDialBounds;
            pGraphics->AttachControl
            (new IEditableTextControl (
                                       editableTextBounds.SubRectVertical(4, 4).GetMidVPadded(10.f),
                                       ("/dualDial/" + std::to_string(d)).c_str(),
                                       consoleTextDef.WithFGColor(getSwatch( Memariani, 1))),
             kCtrlTextInput + d, "textInputs");
          
      
#pragma mark numeric displays

          numericDisplayTextDef = IText ( 12.f, "Menlo").WithFGColor(NEL_TUNGSTEN_FGBlend);
          const IRECT& numericDisplayOuter = dualDialBounds.GetCentredInside(dualDialBounds).GetGridCell(2, 1, 6, 3).GetVShifted(6.0f);
          pGraphics->AttachControl
          (new IVLabelControl (
                             numericDisplayOuter.GetMidVPadded(10.f),
                               " ",
                               rescanButtonStyle
                               .WithDrawShadows(false)
                               .WithColor(kBG, COLOR_TRANSPARENT)
                               .WithValueText ( consoleTextDef.WithSize(12.f).WithFGColor(  COLOR_WHITE  ) ) ),
           kCtrlReadOuterRing + d, "readouts");
          
          const IRECT& numericDisplayInner = dualDialBounds.GetCentredInside(dualDialBounds).GetGridCell(3, 1, 6, 3).GetVShifted(-6.0f);
          pGraphics->AttachControl
          (new IVLabelControl (
                            numericDisplayInner.GetMidVPadded(10.f),
                              " ",
                              rescanButtonStyle
                              .WithDrawShadows(false)
                              .WithColor(kBG, COLOR_TRANSPARENT)
                              .WithValueText ( consoleTextDef.WithSize(12.f).WithFGColor(  COLOR_WHITE  ) ) ),
          kCtrlReadInnerRing + d, "readouts");

}
      
#pragma mark network button / OSC value output
        //▼ Network button/status attaches OSC ActionFunction lambda to concentric dials
      
        pGraphics->AttachControl( new IVButtonControl(
                                      consoleBounds,
                                      nullptr,
                                      "",
                                      rescanButtonStyle.WithEmboss(false).WithDrawShadows(false),
                                      true,
                                      true,
                                      EVShape::Rectangle
                                  )
                                  , kCtrlReScan)
        -> SetActionFunction(
            [this] (IControl * pCaller)
        {
            pCaller->SetAnimation(
                [this] (IControl * pCaller)
            {
                auto progress = pCaller->GetAnimationProgress();
                if (progress > 1.)
                {
                    pCaller->OnEndAnimation();
                    consoleText = cnsl[kMsgScanning];
                    return;
                }
                dynamic_cast<IVectorBase *>(pCaller)->
                SetColor(kPR, IColor::LinearInterpolateBetween(NEL_LUNADA_stop2, kPR, static_cast<float>(progress)));
                pCaller->SetDirty(false);
            }
            , 1000  ); //click flash duration
            for (int i = 0; i < NBR_DUALDIALS; i++)
            {
                IControl *pDialLoop = pCaller->GetUI()->GetControlWithTag(kCtrlFluxDial + i);
                pDialLoop->SetActionFunction ( [this, i] (IControl* pDialLambda)
                {
                    std::vector<float> floatArgs;
                    floatArgs.push_back(pDialLambda->GetValue(0));
                    floatArgs.push_back(pDialLambda->GetValue(1));
                    nelosc.sender->sendOSC( "/dualDial/" + std::to_string(i), floatArgs );
                });
                pDialLoop->SetDirty();
            }
        });
      
      
#pragma mark lambda control
// hidden lambda updates text objects, reacts to OSC and network changes
            
              pGraphics->AttachControl(new ILambdaControl( consoleBounds,
                          [this](ILambdaControl* pCaller, IGraphics& g, IRECT& rect) {
                                if ( (!nelosc.getBeSlimeIP().empty()) && !beSlimeConnected   ) {
                                  beSlimeIP = nelosc.getBeSlimeIP();
                                  beSlimeName = nelosc.getBeSlimeName();

                                  if ( !beSlimeName.empty() ) {
                                    nelosc.sender->changeTargetHost(beSlimeIP.c_str());
                                    beSlimeConnected = true;
                                  };
                                  consoleText = cnsl[kMsgConnected] + beSlimeName;
                                  } else { consoleText =  "localhost"; }
                
                              
                ITextControl* cnsl = dynamic_cast<ITextControl*>(g.GetControlWithTag(kCtrlNetStatus));
                //update console when osc received
                auto msg = nelosc.getLatestMessage();
                if (!msg.empty()) {
                  consoleText = msg;
                  cnsl->SetText(consoleTextDef.WithFGColor(NEL_LUNADA_stop3));
                } else {
                  cnsl->SetText(consoleTextDef.WithFGColor(NEL_TUNGSTEN_FGBlend));
                }
                  cnsl->SetStr(consoleText.c_str());
                  cnsl->SetDirty(false); // not sure if this is needed
                
                  updateAllDialPulseFromOSC( g );
              

                
            } , DEFAULT_ANIMATION_DURATION, true /*loop*/, true /*start imediately*/));
      
      
    }; //end layout lambda function
}


void NEL_VirtualControlSurface::defaultConsoleText() { consoleText = cnsl[kMsgConnected]; }

void NEL_VirtualControlSurface::updateAllDialPulseFromOSC(  IGraphics & g  ) {
  std::string::size_type oscValue = nelosc.getLatestMessage().find("/dial/pulse");
  const std::vector<float>& floatArgs = nelosc.getLatestFloatArgs();
  
  //only update the pulse if OSC messages are in
  if (oscValue != std::string::npos)
  {
    for (int i = 0; i < NBR_DUALDIALS; i++)
      {
          IControl *pDialLoop = GetUI()->GetControlWithTag(kCtrlFluxDial + i);
          pDialLoop->As<NEL_DualDial>()->setFlashRate( floatArgs.at( i % floatArgs.size() ) );
          pDialLoop->SetDirty(true);
      }
  }
  //always update the readouts
    for (int i = 0; i < NBR_DUALDIALS; i++)
      {
        IControl *pDialLoop = GetUI()->GetControlWithTag(kCtrlFluxDial + i);
        GetUI()->
        GetControlWithTag(kCtrlReadOuterRing + i)->
          As<IVLabelControl>()->
            SetStrFmt(4, std::to_string( pDialLoop->GetValue(0) ).c_str() );
        GetUI()->
          GetControlWithTag(kCtrlReadInnerRing + i)->
            As<IVLabelControl>()->
              SetStrFmt(4, std::to_string( pDialLoop->GetValue(1) ).c_str() );
      }
 
}

#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
NEL_VirtualControlSurface::~NEL_VirtualControlSurface()
{
    // may need some kind of attempt to kill all detached threads from launchNetworkingThreads()
}


#if IPLUG_DSP
// not using
//void NEL_VirtualControlSurface::ProcessBlock(sample **inputs, sample **outputs, int nFrames)
//{
//   
//}
#endif





