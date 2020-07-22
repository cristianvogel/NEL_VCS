#pragma once


#include "IPlug_include_in_plug_hdr.h"
#include "NEL_VCS_Constants.h"
#include "NEL_OSC_Methods.hpp"
#include "GlobSeqHelpers.h"
#include "NELDoubleDial.h"
#include <atomic>

const int kNumPresets = 1;

#pragma mark NEL_Enumerators
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
  kCtrlNetStatus = 0,
  kCtrlReScan,
  kCtrlFluxDial,
  kNumCtrlTags = kCtrlFluxDial + NBR_DUALDIALS
};

enum EControlDialTags
{
  kCtrlFluxDialInner = 0,
  kCtrlFluxDialOuter = kCtrlFluxDialInner + NBR_DUALDIALS,
  kNumCtrlFluxDials = kCtrlFluxDialOuter + NBR_DUALDIALS
};

enum EStatusMessages
{
  kMsgScanning = 0,
  kMsgConnected,
  kNumStatusMessages
};

class NEL_VirtualControlSurface final : public iplug::Plugin
{
public:
  NEL_VirtualControlSurface(const iplug::InstanceInfo& info);
  ~NEL_VirtualControlSurface();

  NEL_OSC nelosc;
  std::string beSlimeName = "";
  std::vector<std::string> cnsl =
  {
    "⚇ localhost",
    "⚉ "
  };
 
  std::string consoleText = "";
  
  std::string beSlimeIP = "";
  std::atomic_bool beSlimeConnected {false};
  
  IText consoleFont;
  std::unique_ptr<GlobSeqHelpers> gsh = std::make_unique<GlobSeqHelpers>();


  
#if IPLUG_DSP // http://bit.ly/2S64BDd
 // void ProcessBlock(iplug::sample** inputs, iplug::sample** outputs, int nFrames) override;
#endif
};
