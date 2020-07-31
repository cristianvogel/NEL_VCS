#pragma once


#include "IPlug_include_in_plug_hdr.h"
#include "NEL_VCS_Constants.h"
#include "NEL_OSC_Methods.hpp"
#include "GlobSeqHelpers.h"
#include "NEL_VCS_Enums.hpp"
#include "NEL_DualDial.h"


#include <atomic>

const int kNumPresets = 1;

#pragma mark NEL_enumerators


class NEL_VirtualControlSurface final : public iplug::Plugin
{
public:
  NEL_VirtualControlSurface(const iplug::InstanceInfo& info);
  ~NEL_VirtualControlSurface();
  
  void OnIdle() override;

  NEL_OSC nelosc;
  std::string prevMsg = "";
  std::string beSlimeName = "";
  std::vector<std::string> cnsl =
  {
    "⚇ localhost",
    "⚉ "
  };
 
  std::string dialSendAddress = "/dualDial/";
  
  std::string consoleText = "";
  
  std::string beSlimeIP = "";
  std::atomic_bool beSlimeConnected {false};
  
  IText consoleTextDef;
  IText numericDisplayTextDef;
  IText ledOn;
  IText ledOff;
  
  void defaultConsoleText();
  void updateAllDialInfoFromOSC();
  std::unique_ptr<GlobSeqHelpers> gsh = std::make_unique<GlobSeqHelpers>();
  
#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(iplug::sample** inputs, iplug::sample** outputs, int nFrames) override;
#endif
};
