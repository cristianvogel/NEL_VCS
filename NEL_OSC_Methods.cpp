//
//  NEL_OSC.cpp
//  NEL_OSC-macOS
//
//  Created by Cristian Andres Vogel on 12/07/2020.
//


#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "NEL_VCS_Constants.h"
#include "NEL_OSC_Methods.hpp"


NEL_OSC::NEL_OSC( const char * host  , int port  ) :
   listener (8080, listener)
  
 {
   _host = host ;
   _port = port;
   messageLog = std::make_unique<std::vector<std::string>>(2);
   initOSCSender("localhost", 9090);
   
   for (int i = 0; i<= NBR_DUALDIALS; i++) {
     dialSendAddress.push_back( DEFAULT_DIAL_ADDRESS + std::to_string( i ) );
   }
 };
 NEL_OSC::~NEL_OSC() {  }; // to-do: close ports

#pragma mark setup

/**
* mDNS discovery thread
*/
void NEL_OSC::launchNetworkingThread(){
  std::thread slimeThread( [this] () { zeroConf.init(); } );
  std::thread listener ( [this] () { runOSCListener(  8080  ); } );
  slimeThread.detach();
  listener.detach();
}

void NEL_OSC::runOSCListener ( int port ) {

  listener.m_receiveSocket->Run();

}

bool NEL_OSC::tryToOpenListener() {
  listener.openListenerSocket( listener );
  if (listener.m_receiveSocket != nullptr) {launchNetworkingThread(); }
  return (listener.m_receiveSocket != nullptr);
}

void NEL_OSC::initOSCSender( const char* IP, int port ) {

if (sender == nullptr) {
  sender = std::make_unique<osc::NEL_PacketSender>(IP, port);
  if ( getBeSlimeName() != "localhost") initKyma();
  }

}

void NEL_OSC::initKyma() {
  if (!sender) initOSCSender( getBeSlimeIP().c_str(), 8000 );
  else {
    sender->changeTargetHost(getBeSlimeName().c_str());
  }
  sender->sendOSC("/osc/respond_to", _port); //hardware handshake should receive /osc/response_from
}

 
#pragma mark getters

std::string NEL_OSC::getLatestMessage() {
  
  if ( listener.messageReceived && !(listener.getMostRecentMessage().empty()) )  return listener.getMostRecentMessage();
  else return "";
}

const std::vector<float> NEL_OSC::getLatestFloatArgs() {
  
  return listener.getMostRecentFloatArgs();
}

std::string NEL_OSC::getBeSlimeIP() {
  mtx.lock();
  std::string IP = zeroConf.addressToString();
  if (IP == "0.0.0.0") IP = "127.0.0.1" ;
  mtx.unlock();
  return IP;
}

std::string NEL_OSC::getBeSlimeName() {
  mtx.lock();
  std::string n {""};
  n = zeroConf.hostNameToString();
  mtx.unlock();
  return n;
}

void NEL_OSC::hardwareDisconnected() {
  listener.hardwareConnected = false;
}

bool NEL_OSC::hardwareConnected() {
  return listener.hardwareConnected;
}


