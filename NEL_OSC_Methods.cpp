//
//  NEL_OSC.cpp
//  NEL_OSC-macOS
//
//  Created by Cristian Andres Vogel on 12/07/2020.
//

#include "NEL_OSC_Methods.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <thread>


NEL_OSC::NEL_OSC( const char * host  , int port  ) :
   listenerOSC (8080, listenerOSC)
 {
   _host = host ;
   _port = port;
   messageLog = std::make_unique<std::vector<std::string>>(2);
   
 };
 NEL_OSC::~NEL_OSC() {  }; // to-do: close ports

#pragma mark setup

/**
* mDNS discovery thread
*/
void NEL_OSC::launchNetworkingThread(){
  std::thread slimeThread( [this] () { zeroConf.init(); } );
  std::thread listener ( [this] () { initOSCListener(  8080  ); } );
  slimeThread.detach();
  listener.detach();
}

void NEL_OSC::initOSCListener ( int port ) {

  listenerOSC.m_receiveSocket->Run();

}


void NEL_OSC::initOSCSender( const char* IP, int port ) {

if (senderOSC == nullptr) {
  senderOSC = std::make_unique<osc::NEL_PacketSender>(IP, port);
  initKyma();
  }
}

void NEL_OSC::initKyma() {
  if (!senderOSC) initOSCSender( getBeSlimeIP().c_str(), 8000 );
  else {
    
     // todo: write SetDestination method
      // senderOSC->SetDestination(getBeSlimeIP().c_str(), 8000);
    
  }
  sendOSC("/osc/respond_to", static_cast<int>(_port) ); //hardware handshake should receive /osc/response_from
}


#pragma mark sending needs migrating to OSCpack

void NEL_OSC::sendOSC( const std::string & addressStem, const std::vector<float> & args )
{
  if (!senderOSC) initOSCSender( getBeSlimeIP().c_str(), 8000 );
  if (oscSendActive)
  {
    
   
  }
}

void NEL_OSC::sendOSC( const std::string & addressStem, const std::vector<int> & args )
{
  if (!senderOSC) initOSCSender( getBeSlimeIP().c_str(), 8000 );
  if (oscSendActive)
  {
    //todo: send int args
//    iplug::OscMessageWrite msg;
//    msg.PushWord( addressStem.c_str() );
//    for (int i=0; i<args.size() ; i++) {
//      msg.PushIntArg( args.at(i));
//    }
//    senderOSC->SendOSCMessage(msg);
  }
}

void NEL_OSC::sendOSC( const std::string & addressStem, const int & arg )
{
  if (!senderOSC) initOSCSender( getBeSlimeIP().c_str(), 8000 );
  if (oscSendActive) {
    //todo: send one int arg
//  {
//    iplug::OscMessageWrite msg;
//    msg.PushWord( addressStem.c_str() );
//    msg.PushIntArg( arg);
//    senderOSC->SendOSCMessage(msg);
//  }
    }
}

void NEL_OSC::sendOSC( const std::string & addressStem, const float & arg )
{
  if (!senderOSC) initOSCSender( getBeSlimeIP().c_str(), 8000 );
  if (oscSendActive) {
    //todo: send one float arg
//  {
//    iplug::OscMessageWrite msg;
//    msg.PushWord( addressStem.c_str() );
//    msg.PushFloatArg( arg);
//    senderOSC->SendOSCMessage(msg);
//  }
    }
}
 
#pragma mark getters

std::string NEL_OSC::getLatestMessage() {
  
  if ( listenerOSC.messageReceived && !(listenerOSC.getMostRecentMessage().empty()) )  return listenerOSC.getMostRecentMessage();
  else return "";
}

const std::vector<float> NEL_OSC::getLatestFloatArgs() {
  
  return listenerOSC.getMostRecentFloatArgs();
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

bool NEL_OSC::getBeSlimeResponse() {
  return beSlimeResponse;
}

void NEL_OSC::resetBeSlimeResponse() {
  beSlimeResponse = false;
}


