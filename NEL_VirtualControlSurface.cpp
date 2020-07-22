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
        const IRECT b = pGraphics->GetBounds().GetScaledAboutCentre(0.95f);
        const IRECT consoleBounds = b.GetFromBottom( 12.f ).GetGridCell(1, 1, 3);

#pragma mark console text
        //▼ small logging console output status update text
        consoleFont = IText ( 12.f, "Menlo").WithFGColor(NEL_TUNGSTEN_FGBlend);
        pGraphics->AttachControl(new ITextControl(consoleBounds, consoleText.c_str(), consoleFont, false), kCtrlNetStatus);
        
      
        pGraphics->AttachControl(new ILambdaControl( consoleBounds,
                    [this](ILambdaControl* pCaller, IGraphics& g, IRECT& rect) {
                        
                        
                        if (nelosc.getBeSlimeIP()!="") {
                          beSlimeIP = nelosc.getBeSlimeIP();
                          beSlimeName = nelosc.getBeSlimeName();
                         // osc.changeDestination(beSlimeIP, 8000);
                          // to do change host when Kyma hardware connected
                          consoleText = cnsl[kMsgConnected] + beSlimeName;
                        }
          
          if (nelosc.getBeSlimeResponse() ) {
            std::cout << "BeSlime responded." << std::endl;
            g.GetControlWithTag(kCtrlReScan)->Animate();
            nelosc.resetBeSlimeResponse(); }
                        
                        ITextControl* cnsl = dynamic_cast<ITextControl*>(g.GetControlWithTag(kCtrlNetStatus));
          
          //todo : flash new message
//                       if ( osc.newMessage()) {
//                         consoleText = osc.getLatestMessage();
//                         cnsl->SetText(consoleFont.WithFGColor(NEL_TUNGSTEN));
                     
                         cnsl->SetText(consoleFont.WithFGColor(NEL_TUNGSTEN_FGBlend));
                        cnsl->SetStr(consoleText.c_str());
                        cnsl->SetDirty(true); // not sure if this is needed
                  },
                DEFAULT_ANIMATION_DURATION, true /*loop*/, false /*start imediately*/));
#pragma mark dual dials
        //▼ rows of dual concentric dials with two paramIdx
        for (int d = 0; d < NBR_DUALDIALS; d++)
        {

            const IRECT dualDialBounds = b.GetGridCell(0, 0, 4, 1).SubRectHorizontal(NBR_DUALDIALS, d).FracRect(EDirection::Horizontal, 0.75f);
            pGraphics->AttachControl
            (new NELDoubleDial(
                 dualDialBounds
                 , {kDualDialInner + d, kDualDialOuter + d}
                 , getSwatch( Lunada, d % 3)
                 , getSwatch( Lunada, (d + 1) % 3)
                 , getSwatch( Lunada, (d + 2) % 3)
             ), kCtrlFluxDial + d
             )->As<NELDoubleDial>()->setButtonStates(pGraphics->LoadSVG(NEL_BUTTON_ON), pGraphics->LoadSVG(NEL_BUTTON_OFF));
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
                if(progress > 1.)
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
                  {
                    std::vector<float> floatArgs;
                    floatArgs.push_back(pDialLambda->GetValue(0));
                    floatArgs.push_back(pDialLambda->GetValue(1));
                    
                    nelosc.sendOSC( "/dualDial/" + std::to_string(i), floatArgs );
                    
                  }
                });
                pDialLoop->SetDirty();
            }
        });
    }; //end layout lambda function
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
NEL_VirtualControlSurface::~NEL_VirtualControlSurface()
{
    // some kind of naive attempt to kill all detached processes by launchNetworkingThreads()
}


#endif






#if IPLUG_DSP
// not using
//void NEL_VirtualControlSurface::ProcessBlock(sample **inputs, sample **outputs, int nFrames)
//{
//   
//}
#endif





