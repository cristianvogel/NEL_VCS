// NEL_Virtual Control Surface
// 𝌺 Created by Cristian Vogel / NeverEngineLabs 2020

#include "NEL_VirtualControlSurface.h"
#include "NEL_OSC_Methods.hpp"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "tiny-process/process.hpp"
#include "IconsForkAwesome.h"
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
        // load some dependencies here
        pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
        pGraphics->EnableMouseOver(true);
        pGraphics->EnableTooltips(true);
        pGraphics->Resize(PLUG_WIDTH, PLUG_HEIGHT, 1.333f);
        pGraphics->AttachPanelBackground(NEL_TUNGSTEN);
        pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
        pGraphics->LoadFont("Menlo", MENLO_FN);
        pGraphics->LoadFont("ForkAwesome", FORK_AWESOME_FN);
        const IText forkAwesomeText {16.f, NEL_TUNGSTEN_FGBlend, "ForkAwesome"};
        
      auto showHideAddresses =
      
        [this] (IControl * pCaller) { GetUI()->ForControlInGroup( "addressStems",
                                                                  [&] (IControl& field) { field.Hide(!field.IsHidden()); }
                                                                 ); };
     
#pragma mark mainCanvas
      
      // main app GUI IRECT
      const IRECT b = pGraphics->GetBounds().GetScaledAboutCentre(0.95f).FracRectVertical(0.9f, true);
      const IRECT consoleBounds = pGraphics->GetBounds().GetFromBottom( 24.f ).GetGridCell(0, 1, 1);
      const IRECT plotBounds = pGraphics->GetBounds().GetFromTop( 24.f );
      
      
#pragma mark console text
        //▼ small network logging console outputs OSC messages and host
      
       consoleTextDef = IText ( 11.f, "Menlo").WithFGColor(NEL_TUNGSTEN_FGBlend);
//       ledOn = consoleTextDef.WithFGColor(NEL_TUNGSTEN_FGBlend);
//       ledOff = consoleTextDef.WithFGColor(NEL_TUNGSTEN_FGBlend);
      
        pGraphics->AttachControl(new ITextControl
                                (consoleBounds,
                                consoleText.c_str(),
                                consoleTextDef,
                                NEL_TUNGSTEN,
                                true
                                )
                                 , kCtrlNetStatus);
      
      
      
      
#pragma mark decorative plot
      pGraphics->AttachControl(new IVPlotControl(plotBounds, {{getSwatch(Memariani, 0).WithOpacity(0.5f),  [](double x){ return std::sin(x * 12);} },
        {getSwatch(Memariani, 1).WithOpacity(0.35f), [](double x){ return std::cos(x * 12.5);} },
        {getSwatch(Memariani, 2).WithOpacity(0.25f), [](double x){ return std::sin( x * 12 ) * std::cos( x* 12.5) ;} }

      }, 256, "", DEFAULT_STYLE.WithColor(kBG, COLOR_TRANSPARENT)), kCtrlPlot, "plot");

#pragma mark settings
         
         pGraphics->AttachControl(new ITextToggleControl(
                                                           plotBounds.FracRectHorizontal(0.125f),
                                                           showHideAddresses,
                                                           ICON_FK_SQUARE_O,
                                                           ICON_FK_PLUS_SQUARE_O,
                                                           forkAwesomeText
                                                         )
                                  , kCtrlShowInfo, "prefs");
      
         
#pragma mark dual dials
        //▼ rows of dual concentric dials with two paramIdx
            
        
        for (int d = 0; d < NBR_DUALDIALS; d++)
        {
          const IRECT dualDialBounds = b.GetGridCell( d , 2 , NBR_DUALDIALS/2 ).GetScaledAboutCentre(0.75f);
          
          IActionFunction sendOSCFromDials =
            [&, pGraphics, d] ( IControl* pCaller) {
                              DialAddr dialAddress = !nelosc.dialSendAddress.empty() ? nelosc.dialSendAddress.at(d) : DEFAULT_DIAL_ADDRESS + std::to_string(d);
                              IControl *pDialLoop = pCaller->GetUI()->GetControlWithTag(kCtrlFluxDial + d);
                              std::vector<float> floatArgs;
                              floatArgs.push_back(pDialLoop->GetValue(0));
                              floatArgs.push_back(pDialLoop->GetValue(1));
                              nelosc.sender->sendOSC( dialAddress, floatArgs );
                              pDialLoop->SetDirty(false);
            };
          
            pGraphics->AttachControl
            (new NEL_DualDial(
                 dualDialBounds
                 , {kDualDialInner + d, kDualDialOuter + d}
                 , getSwatch( Lunada, d % 3)
                 , getSwatch( Lunada, (d + 1) % 3)
                 , getSwatch( Lunada, (d + 2) % 3)
             ), kCtrlFluxDial + d, "dualDials")
          ->As<NEL_DualDial>()
          ->setupButtonStateSVG(pGraphics->LoadSVG(NEL_BUTTON_ON), pGraphics->LoadSVG(NEL_BUTTON_OFF))
          ->setTickMarkSVG(pGraphics->LoadSVG(NEL_TICK_SVG)) 
          ->SetActionFunction( sendOSCFromDials );
         
          
#pragma mark text input address stems
      
           
           auto setAddressStem = [&, pGraphics, d] (IControl* pCaller) {
                    
              std::string newAddressStem = static_cast<std::string> ( pCaller->As<IEditableTextControl>()->GetStr() );
              newAddressStem = gsh->chomp(newAddressStem, d);
              nelosc.dialSendAddress.at(d) = newAddressStem;
              pCaller->SetDirty(false);
                    };
          
            const IRECT& editableTextBounds = dualDialBounds;
          
            pGraphics->AttachControl (new IEditableTextControl (
                                       editableTextBounds.SubRectVertical(4, 4).GetMidVPadded(10.f),
                                       nelosc.dialSendAddress.at(d).c_str(),
                                       consoleTextDef.WithFGColor(getSwatch( Memariani, 1))),
             kCtrlTextInput + d, "addressStems")->SetActionFunction( setAddressStem )->Hide(true);
          
      

#pragma mark numeric displays

          numericDisplayTextDef = IText ( 12.f, "Menlo").WithFGColor(NEL_TUNGSTEN_FGBlend);
          const IRECT& numericDisplayOuter = dualDialBounds.GetCentredInside(dualDialBounds).GetGridCell(2, 1, 6, 3).GetVShifted(6.0f);
          pGraphics->AttachControl
          (new IVLabelControl (
                             numericDisplayOuter.GetMidVPadded(10.f),
                               "…",
                               rescanButtonStyle()
                               .WithDrawShadows(false)
                               .WithColor(kBG, COLOR_TRANSPARENT)
                               .WithValueText ( consoleTextDef.WithSize(12.f).WithFGColor(  COLOR_WHITE  ) ) ),
           kCtrlReadOuterRing + d, "readouts");
          
          const IRECT& numericDisplayInner = dualDialBounds.GetCentredInside(dualDialBounds).GetGridCell(3, 1, 6, 3).GetVShifted(-6.0f);
          pGraphics->AttachControl
          (new IVLabelControl (
                            numericDisplayInner.GetMidVPadded(10.f),
                              " ",
                              rescanButtonStyle()
                              .WithDrawShadows(false)
                              .WithColor(kBG, COLOR_TRANSPARENT)
                              .WithValueText ( consoleTextDef.WithSize(12.f).WithFGColor(  COLOR_WHITE  ) ) ),
          kCtrlReadInnerRing + d, "readouts");

}
           
      #pragma mark network button
      
        pGraphics->AttachControl( new IVButtonControl(
                                      consoleBounds,
                                      nullptr,
                                      "",
                                      rescanButtonStyle().WithEmboss(false).WithDrawShadows(false),
                                      true,
                                      true,
                                      EVShape::Rectangle
                                  ), kCtrlReScan)
        -> SetActionFunction(
        [this] (IControl * pCaller) {
          
              auto showInfoButton = GetUI()->GetControlWithTag(kCtrlShowInfo)->As<ITextToggleControl>();
              showInfoButton->SetValue( showInfoButton->GetValue() >0.5f ? 0.f : 1.0f  );
              showInfoButton->SetDirty(true);
               
              pCaller->SetAnimation(
                  [this] (IControl * pCaller)
              {
                  auto progress = pCaller->GetAnimationProgress();
                  if (progress > 1.)
                  {
                      pCaller->OnEndAnimation();
                      return;
                  }
                  dynamic_cast<IVectorBase *>(pCaller)->
                  SetColor(kPR, IColor::LinearInterpolateBetween(NEL_LUNADA_stop2, kPR, static_cast<float>(progress)));
                  pCaller->SetDirty(false);
              }
              , 1000  ); //click flash duration
    
                //redraw dials and sendOSC from attached action
                GetUI()->ForControlInGroup("dualDials", [pCaller] (IControl& pDial) {
                  pDial.SetDirty(true);
                });
          });
      }; //end of mLayoutFunc lambda
  
  
}; //end main layout

void NEL_VirtualControlSurface::defaultConsoleText() {
  consoleText = cnsl[kMsgConnected];
}

void NEL_VirtualControlSurface::updateAllDialInfoFromOSC() {
  std::string::size_type oscMsgEntry = nelosc.getLatestMessage().find("/dial/pulse");
  const std::vector<float>& floatArgs = nelosc.getLatestFloatArgs();
  
  
  for (int i = 0; i < NBR_DUALDIALS; i++)
    {
      IControl *pDialLoop = GetUI()->GetControlWithTag(kCtrlFluxDial + i);
      if (oscMsgEntry != std::string::npos) {  //only update the pulse if OSC messages are in
        pDialLoop->As<NEL_DualDial>()->setFlashRate( floatArgs.at( i % floatArgs.size() ) );
      }
    
      //always updates the readouts
      //todo: change-driven draw optimisation
      GetUI()->
      GetControlWithTag(kCtrlReadOuterRing + i)-> As<IVLabelControl>()->
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

                if ( !beSlimeName.empty() && !beSlimeConnected ) {
                nelosc.sender->changeTargetHost(beSlimeIP.c_str());
                beSlimeConnected = true; }; consoleText =
                cnsl[kMsgConnected] + beSlimeName; } else {
                consoleText =  "localhost"; }

                // check to see if GUI closed because OnIdle() continues
                // even without GUI
                IGraphics* pGraphics = GetUI(); if(!pGraphics) return;

                auto* cnsl = pGraphics->GetControlWithTag(kCtrlNetStatus)->As<ITextControl>();
                IVButtonControl* cnslButton = pGraphics->GetControlWithTag(kCtrlReScan)->As<IVButtonControl>();

                //update console when osc received
                auto msg = nelosc.getLatestMessage();
                if (!msg.empty() && msg != prevMsg) {
                  cnslButton->SetDirty(false);
                  consoleText = msg;
                  cnsl->SetStr(consoleText.c_str());
                  cnsl->SetDirty(false);
                  cnsl->SetAnimation( unGhostText , 500.0f);
                } else {
                  if (cnsl->GetAnimationProgress() > 0.99f) {
                    cnsl->SetAnimation( ghostText , 500.0f);
                  }
                }
                prevMsg = msg;
                updateAllDialInfoFromOSC( );
          }


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





