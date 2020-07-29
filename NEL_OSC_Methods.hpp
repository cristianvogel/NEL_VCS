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
#include "NEL_PacketListener.hpp"
#include "NEL_PacketSender.hpp"
#include <iostream>
#include <cstring>

#include <mutex>


static bool beSlimeResponse = false;

class NEL_OSC
{
public:
  NEL_OSC( const char * host , int port );
  ~NEL_OSC();
    
  const char * _host;
  int _port;
  std::unique_ptr<std::vector<std::string>> messageLog;
  
  NEL_PacketListener listener;
  std::unique_ptr<osc::NEL_PacketSender> sender;
  
  void launchNetworkingThread();
  void initOSCSender( const char* IP ,  int port );
  void initOSCListener ( int port);
  void initKyma(); 
  bool oscSendActive {true}; //todo: make UI button to disable OSC activity
  
 // void changeDestination( const std::string &, int );
  
  std::mutex mtx; // mutex for critical section in network thread
  
  std::string getLatestMessage();
  const std::vector<float> getLatestFloatArgs();
  bool hardwareConnected();
  bool getBeSlimeResponse();
  void hardwareDisconnected();
  std::string getBeSlimeIP();
  std::string getBeSlimeName();
  
  
private:
  mDNS zeroConf;

};





