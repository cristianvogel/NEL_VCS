//
//  NEL_OSC_Methods.hpp
//  NEL_VirtualControlSurface-macOS
//
//  Created by Cristian Andres Vogel on 12/07/2020.
//
#pragma once

#include <string>
#include <vector>
#include "IPlugOSC.h"

class NEL_OSC {
  
public:
  NEL_OSC( const char * IP  = "127.0.0.1", int port = 8080 );
  ~NEL_OSC(); 
    
  const char * IP;
  int port;
  std::unique_ptr<iplug::OSCSender> oscSender;
  void initOSCSender( const char* IP = "127.0.0.1",  int port = 8080 );
  void initKyma( const char * IP = "beslime-877.local", int port = 8000);
  bool oscSendActive {true}; //todo: make UI button to disable OSC activity
  void sendOSC( const std::string & , const std::vector<float> &  );
  void sendOSC( const std::string & , const std::vector<int> &  );
  void sendOSC( const std::string & , const int & );
  void sendOSC( const std::string & , const float & );
};


