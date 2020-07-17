//
//  NEL_OSC_Methods.hpp
//  NEL_VirtualControlSurface-macOS
//
//  Created by Cristian Andres Vogel on 12/07/2020.
//
#pragma once

#include <string>
#include <iostream>
#include <vector>
#include "mDNS_IPExtract.h"
#include "IPlugOSC.h"

class NEL_OSC : public iplug::OSCReceiver
{
public:
  NEL_OSC( const char * IP  = "127.0.0.1", int port = 8080 );
  ~NEL_OSC();
    
  const char * IP;
  int port;
  std::unique_ptr<std::vector<std::string>> messageLog;
  
  std::unique_ptr<iplug::OSCSender> oscSender;
  void initOSCSender( const char* IP ,  int port );
  // void initKyma( const char * IP = "", int port = 8000); //todo: Kyma handshake
  bool oscSendActive {true}; //todo: make UI button to disable OSC activity
  void sendOSC( const std::string & , const std::vector<float> &  );
  void sendOSC( const std::string & , const std::vector<int> &  );
  void sendOSC( const std::string & , const int & );
  void sendOSC( const std::string & , const float & );
  void changeDestination( const std::string &, int );
  
  void OnOSCMessage(iplug::OscMessageRead& msg) override;
  WDL_String senderNetworkInfo;
  std::string getLatestMessage();
  bool newMessage();
  void launchNetworkingThread();
  std::string getBeSlimeIP();
  std::string getBeSlimeName();
  
private:
  mDNS zeroConf;
};


