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
  if (nelosc.udpListener.m_receiveSocket != nullptr) {
    nelosc.launchNetworkingThread();
  } else {
    m_noNetwork = true;
  }
    
    InitParamRange(kDualDialInner, kDualDialInner + NBR_DUALDIALS - 1, 1, "Dual Dial %i", 0, 0., 1., 0, "%", 0, "Inner Value");
    InitParamRange(kDualDialOuter, kDualDialOuter + NBR_DUALDIALS - 1, 1, "Dual Dial %i", 0, 0., 1., 0, "%", 0, "Outer Value");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
    mMakeGraphicsFunc = [&]()
    {
        return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_HEIGHT));
    };
  
  
#pragma mark Layout lambda init
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
        
#pragma mark some lambdas used in main layout lambda
      /*
       flip the visibility of the OSC addresses
      */
      auto showHideControlInfo =
        [this] (IControl * pCaller) {
          hideReadouts = !hideReadouts;
          GetUI()->ForControlInGroup(
                                     "addressStems",
                                      [ ] (IControl& field) { field.Hide(!field.IsHidden()); });
                                    };
      
      auto showHideNetInfo =
        [this] (IControl * pCaller) {
     
        GetUI()->ForControlInGroup(
                                  "netInfo",
                                  [ ] (IControl& field) { field.Hide(!field.IsHidden()); });
        };
     /*
       if valid port number, make new connection
      */
      auto changeTargetPort =
        [this] (IControl * pCaller) {
          if (!beSlimeConnected) {
            auto targetPortDisplay = GetUI()->GetControlWithTag(kTargetPort)->As<ITextControl>();
            auto inputtedPortNumber = gsh->cstring_to_ul(targetPortDisplay->GetStr() );
              
            if ( ((inputtedPortNumber > 0) && (inputtedPortNumber < ULONG_MAX)) && (nelosc.m_targetPort != inputtedPortNumber) ) {
                      targetPortDisplay->SetStr( std::to_string(inputtedPortNumber).c_str() );
                      nelosc.udpSender->changeTargetPort(static_cast<int> (inputtedPortNumber) );
                      errno = 0;  //best practice, reset errno variable after check
                   } else if ( (inputtedPortNumber == 0) | (inputtedPortNumber == ULONG_MAX) ) {
                      targetPortDisplay->SetStr( std::to_string(nelosc.m_targetPort).c_str() );
                      errno = 0; }
          } else {
            nelosc.udpSender->setTargetPortForKyma();
          }
        };
      
      /*
        if valid port number, make new connection
       */
  
      auto changeListenerPort =
      [this] (IControl * pCaller) {
        if (!beSlimeConnected) {
          
          auto targetPortDisplay = GetUI()->GetControlWithTag(kListenPort)->As<ITextControl>();
          auto inputtedPortNumber = gsh->cstring_to_ul(targetPortDisplay->GetStr() );
            
          if ( ((inputtedPortNumber > 0) && (inputtedPortNumber < ULONG_MAX)) && (nelosc.m_listenerPort != inputtedPortNumber) ) {
                    targetPortDisplay->SetStr( std::to_string(inputtedPortNumber).c_str() );
            //TODO
                    //nelosc.udpListener.changeListenPort(static_cast<int> (inputtedPortNumber) );
                    errno = 0;  //best practice, reset errno variable after check
                 } else if ( (inputtedPortNumber == 0) | (inputtedPortNumber == ULONG_MAX) ) {
                    targetPortDisplay->SetStr( std::to_string(nelosc.m_listenerPort).c_str() );
                    errno = 0; }
        }
      };
      
      auto showHideMsgConsole =
      [this] (IControl * pCaller) {
        auto messageConsol = GetUI()->GetControlWithTag(kCtrlNetStatus);
        messageConsol->Hide(!messageConsol->IsHidden());
        messageConsol->SetDirty();
      };
      
      
      // this function is incomplete, work in progress
      // idea would be to check if a user given host is on the network
      // via UDP browse
      
//      auto changeHost =
//      [this] (IControl * pCaller) {
//        auto targetHostDisplay = GetUI()->GetControlWithTag(kTargetPort)->As<ITextControl>();
//        auto inputtedHostname = targetHostDisplay->GetStr();
//        std::cout << nelosc.remoteAddressToString(inputtedHostname) << std::endl;
//      };

      
#pragma mark mainCanvas
      
      // main app GUI IRECT
      const IRECT b = pGraphics->GetBounds().GetScaledAboutCentre(0.95f).FracRectVertical(0.9f, true);
      const IRECT consoleBounds = pGraphics->GetBounds().GetFromBottom( 24.f ).GetGridCell(0, 1, 1);
      const IRECT plotBounds = pGraphics->GetBounds().GetFromTop( 24.f );
      
      
#pragma mark console text
        //▼ network logging console outputs OSC messages and stuff
      
        pGraphics->AttachControl(new ITextControl
                                (consoleBounds,
                                consoleText.c_str(),
                                DEFAULT_CONSOLE_TEXT,
                                NEL_TUNGSTEN,
                                true
                                )
                                 , kCtrlNetStatus)->Hide(true);
      
      
#pragma mark decorative plot with reveal
      
      pGraphics->AttachControl(new IVPlotControl(plotBounds, {{getSwatch(Memariani, 0).WithOpacity(0.5f),  [](double x){ return std::sin(x * 12);} },
        {getSwatch(Memariani, 1).WithOpacity(0.35f), [](double x){ return std::cos(x * 12.5);} },
        {getSwatch(Memariani, 2).WithOpacity(0.25f), [](double x){ return std::sin( x * 12 ) * std::cos( x* 12.5) ;} }

      }, 256, "", DEFAULT_STYLE.WithColor(kBG, COLOR_TRANSPARENT)), kCtrlPlot, "plot")->SetAnimationEndActionFunction(showHideNetInfo);
      
     

#pragma mark preference widgets
         
      IRECT settingWidgetBounds = plotBounds.GetGridCell(1, 11, 3, 12).GetScaledAboutCentre(1.5f).GetHPadded(-15.f);
      std::vector<IActionFunction> actions { showHideControlInfo, showHideNetInfo, showHideMsgConsole};
      
      for (int i=0; i<NBR_WIDGETS; i++)
      {
          pGraphics->AttachControl(
                 new NEL_GlyphButton(
                                     settingWidgetBounds.GetGridCell(0, i, 1, 3),
                                     actions.at(i),
                                     m_noNetwork ? WIDGET_OFF_NO_NET : WIDGET_OFF,
                                     m_noNetwork ? WIDGET_ON_NO_NET : WIDGET_ON,
                                     GLYPH,
                                     toolTips[ kNoWidgets + ( i + 1 ) ] // toolTip text
                                    )
                                   , kNoWidgets + ( i + 1 ) , "prefs"
                                   );
      }
      

      
#pragma mark net configs
      
       IRECT widgetCell = plotBounds.GetGridCell(1, 4, 3, 7);
      
      pGraphics->AttachControl(new NEL_TextField(
                                                 widgetCell,
                                                 changeTargetPort,
                                                 (std::to_string(nelosc.m_targetPort)).c_str(),
                                                 DEFAULT_CONSOLE_TEXT,
                                                 "Send Port"
                               ), kTargetPort, "netInfo" )->Hide(true);
      
      widgetCell = plotBounds.GetGridCell(1, 3, 3, 7);
    
      pGraphics->AttachControl(new NEL_TextField(
                                                 widgetCell,
                                                 changeListenerPort,
                                                 (std::to_string(nelosc.m_listenerPort)).c_str(),
                                                 DEFAULT_CONSOLE_TEXT,
                                                 "Listen Port"
                               ), kListenPort, "netInfo")->Hide(true);
    
    widgetCell = plotBounds.GetGridCell(1, 2, 3, 7);
    
    auto* hostnameReadOnly = new NEL_TextField(
                                               widgetCell,
                                               nullptr,
                                               nelosc.m_targetHost ,
                                               DEFAULT_CONSOLE_TEXT,
                                               "Hostname",
                                               false
                                               );
      pGraphics->AttachControl(hostnameReadOnly, kTargetHost , "netInfo")->Hide(true);
      
      
         
#pragma mark dual dials replicated attach
      
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
                              nelosc.udpSender->sendOSC( dialAddress, floatArgs );
                              pDialLoop->SetDirty(false);
            };
          
            pGraphics->AttachControl
            (new NEL_DualDial(
                 dualDialBounds
                 , {kDualDialInner + d, kDualDialOuter + d, kDualDialPulse + d}
                 , getSwatch( Lunada, d % 3)
                 , getSwatch( Lunada, (d + 1) % 3)
                 , getSwatch( Lunada, (d + 2) % 3)
             ), kCtrlFluxDial + d, "dualDials")
          ->As<NEL_DualDial>()
          ->setupButtonStateSVG(pGraphics->LoadSVG(NEL_BUTTON_ON), pGraphics->LoadSVG(NEL_BUTTON_OFF))
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
                                       DEFAULT_CONSOLE_TEXT.WithFGColor(getSwatch( Memariani, 1))),
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
                               .WithValueText ( DEFAULT_CONSOLE_TEXT.WithSize(12.f).WithFGColor(  COLOR_WHITE  ) ) ),
           kCtrlReadOuterRing + d, "readouts");
          
          const IRECT& numericDisplayInner = dualDialBounds.GetCentredInside(dualDialBounds).GetGridCell(3, 1, 6, 3).GetVShifted(-6.0f);
          pGraphics->AttachControl
          (new IVLabelControl (
                            numericDisplayInner.GetMidVPadded(10.f),
                              " ",
                              rescanButtonStyle()
                              .WithDrawShadows(false)
                              .WithColor(kBG, COLOR_TRANSPARENT)
                              .WithValueText ( DEFAULT_CONSOLE_TEXT.WithSize(12.f).WithFGColor(  COLOR_WHITE  ) ) ),
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
          
          // simulate press of the Address reveal widget
          auto showInfoButton = GetUI()->GetControlWithTag(kAddressWidget)->As<NEL_GlyphButton>();
              showInfoButton->SetValue( showInfoButton->GetValue() >0.5f ? 0.f : 1.0f  );
              showInfoButton->SetDirty(true);
          
          //try to re-open listener port if it was blocked at initialisation
          if ( m_noNetwork )
          {
            m_noNetwork = nelosc.tryToOpenListener();
            if (m_noNetwork) { GetUI()->
                ForControlInGroup("prefs", [pCaller] (IControl& prefButton) {
                  prefButton.As<NEL_GlyphButton>()->SetOnText(WIDGET_ON_NO_NET)->SetOffText(WIDGET_OFF_NO_NET);
                  prefButton.SetDirty(true);
                });
            }
          }
               
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
          
          if (nelosc.udpListener.hardwareConnected) { try {nelosc.initKyma();} catch (...) { std::cout << " tried to reinitialise Kyma, fail \n"; }; }
          })
      -> SetTooltip("Network Refresh");
      
      ;
      };
  
  
}; //end main layout

//TODO
//beta test bidrectional protocol

void NEL_VirtualControlSurface::updatesFromIncomingOSC() {
  
  int dualDialIndex = 0;
  std::string oscMsg = nelosc.getLatestMessage();

  
      auto setValueFromIncomingOSC = [ this, &oscMsg, &dualDialIndex ] ( IControl & pCaller ) {
          std::string::size_type oscMsgEntry = oscMsg.find("/dualDial/" + std::to_string( dualDialIndex ) );
          if (oscMsgEntry != std::string::npos) { // get a match
            const std::vector<float>& floatArgs = nelosc.getLatestFloatArgs();
            if (!floatArgs.empty()) {
              int paramIndex = 0; float sum = 0;
              for(const auto& value: floatArgs) {
              pCaller.As<NEL_DualDial>()->SetValue( value, paramIndex );
              ++paramIndex; sum+=value;
              }
              pCaller.As<NEL_DualDial>()->setFlashRate( sum / floatArgs.size() ); //average
              pCaller.SetDirty(false);
            }
          }
      dualDialIndex++;
    };
  
  GetUI()->ForControlInGroup("dualDials", setValueFromIncomingOSC );
}

 
void NEL_VirtualControlSurface::updateNumericDisplays( ) {
  
  
  //hide and don't update values
    GetUI()->ForControlInGroup("readouts", [ & ]( IControl& rd ){
    rd.Hide(hideReadouts);
    });
  
    //show  and update values on numeric displays
    for (int i = 0; i < NBR_DUALDIALS; i++)
      {
        IControl *pDialLoop = GetUI()->GetControlWithTag(kCtrlFluxDial + i)->As<NEL_DualDial>();
        auto* readout = GetUI()->GetControlWithTag(kCtrlReadOuterRing + i)-> As<IVLabelControl>();
        readout -> SetStrFmt(4, std::to_string( pDialLoop->GetValue(0) ).c_str() );
        readout = GetUI()-> GetControlWithTag(kCtrlReadInnerRing + i)-> As<IVLabelControl>();
        readout -> SetStrFmt(4, std::to_string( pDialLoop->GetValue(1) ).c_str() ) ;
      }
  
}


#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
NEL_VirtualControlSurface::~NEL_VirtualControlSurface()
{
    // may need some kind of attempt to kill all detached threads from launchNetworkingThreads()
}


#pragma mark On Idle

// most of the action going on here

void NEL_VirtualControlSurface::OnIdle() {

  
  if ( (!nelosc.getBeSlimeIP().empty()) && !beSlimeConnected ) {
                  beSlimeIP = nelosc.getBeSlimeIP();
                  beSlimeName = nelosc.getBeSlimeName();

                  if ( gsh->stringContains( beSlimeName, "beslime" ) ) {
              
                    nelosc.initKyma();
                    consoleText = beSlimeName;
                    beSlimeConnected = true;
                    nelosc.m_targetHost = beSlimeName.c_str();
                    }
                  }
                  
        // check to see if GUI closed because OnIdle() continues
        // even without GUI
        IGraphics* pGraphics = GetUI(); if(!pGraphics) return;

        ITextControl* cnsl = pGraphics->GetControlWithTag(kCtrlNetStatus)->As<ITextControl>();
        IVButtonControl* cnslButton = pGraphics->GetControlWithTag(kCtrlReScan)->As<IVButtonControl>();
        
        //update console when osc received
        
  if ( !cnsl->IsHidden() ){
        if ( cnslButton->GetMouseIsOver() ) {
          cnsl->SetAnimation( unGhostText , 500.0f);
          pGraphics->SetMouseCursor(ECursor::HELP);
        } else { pGraphics->SetMouseCursor(ECursor::ARROW);}
          
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
  }
  
  updatesFromIncomingOSC(); //do something with incoming OSC
  updateNumericDisplays();
        
   //keep host info display syncd to actual host connection
  auto targetHostDisplay = pGraphics->GetControlWithTag(kTargetHost)->As<ITextControl>();
  if (!targetHostDisplay->IsHidden()) {
    targetHostDisplay->SetStr( nelosc.m_targetHost );
    GetUI()->GetControlWithTag(kTargetPort)->SetValue( nelosc.m_targetPort );
    GetUI()->GetControlWithTag(kListenPort)->SetValue( nelosc.m_listenerPort );
  }
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





