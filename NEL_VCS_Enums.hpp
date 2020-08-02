//
//  NEL_VCS_Enums.hpp
//  NEL_VirtualControlSurface-macOS
//
//  Created by Cristian Andres Vogel on 28/07/2020.
//

#pragma once
#ifndef NEL_VCS_Enums_hpp
#define NEL_VCS_Enums_hpp


enum EParams
{
  kNetstatus = 0,
  kReScan,
  kDualDialInner,
  kDualDialOuter = kDualDialInner + NBR_DUALDIALS,
  kNumParams = kDualDialOuter + NBR_DUALDIALS
};

enum EControlTags
{
  kCtrlNetStatus = 100,
  kCtrlReScan,
  kCtrlFluxDial,
  kNumCtrlTags = kCtrlFluxDial + NBR_DUALDIALS
};

enum EControlReadOutTags
{
  kCtrlReadInnerRing = 150,
  kCtrlReadOuterRing = kCtrlReadInnerRing + NBR_DUALDIALS
};

enum EControlDialTags
{
  kCtrlFluxDialInner = 200,
  kCtrlFluxDialOuter = kCtrlFluxDialInner + NBR_DUALDIALS,
  kNumCtrlFluxDials = kCtrlFluxDialOuter + NBR_DUALDIALS
};

enum EControlTextInputTags
{
  kCtrlTextInput = 300,
  kNumCtrlTextInputs = kCtrlTextInput + NBR_DUALDIALS
};

enum EStatusMessages
{
  kMsgScanning = 400,
  kMsgConnected,
  kNumStatusMessages
};

enum EDisplays
{
  kCtrlTagDisplay = 500,
  kCtrlPlot,
  kCtrlShowInfo,
  kNumDisplays
};


#endif /* NEL_VCS_Enums_hpp */
