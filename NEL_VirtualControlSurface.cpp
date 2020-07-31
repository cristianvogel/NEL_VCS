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
    consoleText = cnsl[kMsgScanning]; // some kind of undefined behaviour happening here sometimes
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
        pGraphics->Resize(PLUG_WIDTH, PLUG_HEIGHT, 1.618f);
        pGraphics->AttachPanelBackground(NEL_TUNGSTEN);
        pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
        pGraphics->LoadFont("Menlo", MENLO_FN);
        
  
            const IVStyle rescanButtonStyle
        {
            true, // Show label
            false, // Show value
            {
                DEFAULT_SHCOLOR, // Background
                COLOR_TRANSLUCENT, // Foreground
                COLOR_LIGHT_GRAY, // Pressed
                COLOR_TRANSPARENT, // Frame
                DEFAULT_HLCOLOR, // Highlight
                DEFAULT_SHCOLOR, // Shadow
                COLOR_BLACK, // Extra 1
                DEFAULT_X2COLOR, // Extra 2
                DEFAULT_X3COLOR  // Extra 3
            }, // Colors
            IText(
                12.f,
                COLOR_LIGHT_GRAY,
                "Menlo",
                EAlign::Center,
                EVAlign::Middle,
                0.f,
                DEFAULT_TEXTENTRY_BGCOLOR,
                DEFAULT_TEXTENTRY_FGCOLOR
            ) // Label text
        };



#pragma mark mainCanvas
        // main app GUI IRECT
      
      const IRECT b = pGraphics->GetBounds().GetScaledAboutCentre(0.95f).FracRectVertical(0.9f, true);
      const IRECT consoleBounds = pGraphics->GetBounds().GetFromBottom( 24.f ).GetGridCell(0, 1, 1);
      const IRECT plotBounds = pGraphics->GetBounds().GetFromTop( 24.f );
      
      

#pragma mark console text
        //▼ small network logging console outputs OSC messages and host
      
       consoleTextDef = IText ( 12.f, "Menlo").WithFGColor(NEL_TUNGSTEN_FGBlend);
       ledOn = consoleTextDef.WithFGColor(NEL_LUNADA_stop3);
       ledOff = consoleTextDef.WithFGColor(NEL_TUNGSTEN_FGBlend);
      
        pGraphics->AttachControl(new ITextControl
                                 (consoleBounds,
                                  consoleText.c_str(),
                                  consoleTextDef,
                                  NEL_TUNGSTEN,
                                  true
                                  )
                                 , kCtrlNetStatus);
      
      pGraphics->AttachControl(new IVPlotControl(plotBounds, {{getSwatch(Memariani, 0),  [](double x){ return std::sin(x * 6.2);} },
                                                              {getSwatch(Memariani, 1), [](double x){ return std::cos(x * 6.2);} },
                                                              {getSwatch(Memariani, 2), [](double x){ return std::sin(x * 12.2);} }

      }, 32, "", DEFAULT_STYLE.WithColor(kBG, COLOR_TRANSPARENT)), kCtrlPlot, "plot") ;
      
#pragma mark dual dials
        //▼ rows of dual concentric dials with two paramIdx
      
      IActionFunction swapSVG = [&, pGraphics] ( IControl* pCtrl) {
        NEL_DualDial* pDial = pCtrl->As<NEL_DualDial>();
        pGraphics->
        DrawSVG(  pDial->buttonStates [ pDial->pulse ? 1 : 0 ],
                  pDial->getDialBounds()->GetCentredInside(fmin(pDial->getDialBounds()->W(), 50.0f + (NBR_DUALDIALS * 1.618f))));
        };
        
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
          ->As<NEL_DualDial>()
          ->setupButtonStateSVG(pGraphics->LoadSVG(NEL_BUTTON_ON), pGraphics->LoadSVG(NEL_BUTTON_OFF))
          ->SetActionFunction( swapSVG );
          
#pragma mark text input fields for changing send messages
      
           auto setAddressStem = [&, pGraphics, d] (IControl* pCaller) {
                    
                      const char* newAddressStem = GetUI()->GetControlWithTag(kCtrlTextInput + d)->As<IEditableTextControl>()->GetStr();
                      dialSendAddress = newAddressStem;
                    };
          
            const IRECT& editableTextBounds = dualDialBounds;
            pGraphics->AttachControl (new IEditableTextControl (
                                       editableTextBounds.SubRectVertical(4, 4).GetMidVPadded(10.f),
                                       (dialSendAddress + std::to_string(d)).c_str(),
                                       consoleTextDef.WithFGColor(getSwatch( Memariani, 1))),
             kCtrlTextInput + d, "textInputs")->SetActionFunction( setAddressStem );
          
      
#pragma mark numeric displays

          numericDisplayTextDef = IText ( 12.f, "Menlo").WithFGColor(NEL_TUNGSTEN_FGBlend);
          const IRECT& numericDisplayOuter = dualDialBounds.GetCentredInside(dualDialBounds).GetGridCell(2, 1, 6, 3).GetVShifted(6.0f);
          pGraphics->AttachControl
          (new IVLabelControl (
                             numericDisplayOuter.GetMidVPadded(10.f),
                               "…",
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
     
      #pragma mark iLambda control moved to OnIdle()

      
      #pragma mark network button / OSC value output
        //▼ Network button/status attaches OSC ActionFunction lambda to concentric dials
      // needs to come after ILambda control
      
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
                   // pCaller->OnEndAnimation();
                    consoleText = cnsl[kMsgScanning]; //seems to crash here every now and then??
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
                    nelosc.sender->sendOSC( dialSendAddress + std::to_string(i), floatArgs );
                });
                pDialLoop->SetDirty();
            }
        });
      
    }; //end layout lambda function
}


void NEL_VirtualControlSurface::defaultConsoleText() { consoleText = cnsl[kMsgConnected]; }



void NEL_VirtualControlSurface::updateAllDialInfoFromOSC() {
  std::string::size_type oscMsgEntry = nelosc.getLatestMessage().find("/dial/pulse");
  const std::vector<float>& floatArgs = nelosc.getLatestFloatArgs();
  
  //only update the pulse if OSC messages are in
  if (oscMsgEntry != std::string::npos)
  {
    for (int i = 0; i < NBR_DUALDIALS; i++)
      {
          IControl *pDialLoop = GetUI()->GetControlWithTag(kCtrlFluxDial + i);
          pDialLoop->As<NEL_DualDial>()->setFlashRate( floatArgs.at( i % floatArgs.size() ) );
          //pDialLoop->SetDirty(true);
      }
  }
  //always updates the readouts
  //todo: change-driven draw optimisation
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

void NEL_VirtualControlSurface::OnIdle() {

                if ( (!nelosc.getBeSlimeIP().empty()) &&
                !beSlimeConnected   ) { beSlimeIP =
                nelosc.getBeSlimeIP(); beSlimeName =
                nelosc.getBeSlimeName();

                if ( !beSlimeName.empty() ) {
                nelosc.sender->changeTargetHost(beSlimeIP.c_str());
                beSlimeConnected = true; }; consoleText =
                cnsl[kMsgConnected] + beSlimeName; } else {
                consoleText =  "localhost"; }

                // check to see if GUI closed because OnIdle() continues
                // even without GUI
                IGraphics* pGraphics = GetUI(); if(!pGraphics) return;

                ITextControl* cnsl = pGraphics->GetControlWithTag(kCtrlNetStatus)->As<ITextControl>();
                //update console when osc received
                auto msg = nelosc.getLatestMessage();
                if (!msg.empty() && msg != prevMsg) {
                  consoleText = msg;
                  cnsl->SetText(ledOn); //declare text once as constants not in onIdle()
                  cnsl->SetStr(consoleText.c_str());
                  cnsl->SetDirty(false);
                } else {
                  cnsl->SetText(ledOff);
                  cnsl->SetDirty(false);
                }
                prevMsg = msg;
                updateAllDialInfoFromOSC( );
          };

#if IPLUG_DSP
// muted
void NEL_VirtualControlSurface::ProcessBlock(sample **inputs, sample **outputs, int nFrames)
{
     for(int i=0;i<nFrames;i++) {
       outputs[0][i] = 0.;
       outputs[1][i] = 0.;
     }
}
#endif





