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


NEL_OSC::NEL_OSC( const char * host  , int port  ) 
   
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
  slimeThread.detach();
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


#pragma mark sending

void NEL_OSC::sendOSC( const std::string & addressStem, const std::vector<float> & args )
{
  if (!senderOSC) initOSCSender( getBeSlimeIP().c_str(), 8000 );
  if (oscSendActive)
  {
    
    //todo: send float args
//    iplug::OscMessageWrite msg;
//    msg.PushWord( addressStem.c_str() );
//    for (int i=0; i<args.size() ; i++) {
//      msg.PushFloatArg( args.at(i));
//    }
//    senderOSC->SendOSCMessage(msg);
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
 

#pragma mark Receiving ( needs migrating to OSCPack )
//void NEL_OSC::OnOSCMessage(iplug::OscMessageRead& msg)  {
//
//
//  printf( "I: %s", msg.GetMessage());
//  if (strcmp( msg.GetMessage() , "/osc/response_from") == 0) {
//    beSlimeResponse = true;
//  }
//
//  int nbrArgs = msg.GetNumArgs();
//  char type;
//  int index = 0;
//  char oscMessage[128];
//  char address[64];
//  strcpy(address, "");
//  const char * m = msg.PopWord();
//  while (m)
//  {
//        strcat(address, "/");
//        strcat(address, m);
//        m = msg.PopWord();
//  };
//
//
//  while (nbrArgs) {
//
//    msg.GetIndexedArg(index, &type);
//    switch (type)
//    {
//        case 'i':
//        {
//          const int* pValue = msg.PopIntArg(false);
//          if (pValue) sprintf(oscMessage, "%s %i", address, *pValue);
//          break;
//        }
//        case 'f': {
//          const float* pValue = msg.PopFloatArg(false);
//          if (pValue) sprintf(oscMessage, "%s %f", address, *pValue);
//          break;
//        }
//        case 's': {
//          const char* pValue = msg.PopStringArg(false);
//          if (pValue) sprintf(oscMessage, "%s %s", address, pValue);
//          break;
//        }
//        default : break;
//    }
//    nbrArgs--;
//  }
//  messageLog->at(1) = messageLog->at(0);
//  messageLog->at(0) = oscMessage;
//}
//
//bool NEL_OSC::newMessage() { return (messageLog->at(1) != messageLog->at(0));   }
//
//std::string NEL_OSC::getLatestMessage() {
//  std::string r =  messageLog->at(0);
//  messageLog->at(0) = messageLog->at(1);
//  return r; }
//
//void NEL_OSC::changeDestination( const std::string & IP, int port) {
//  if (senderOSC) senderOSC->SetDestination(IP.c_str(), port);
//}


#pragma mark getters
std::string NEL_OSC::getBeSlimeIP() {
  mtx.lock();
  std::string IP = zeroConf.addressToString();
  if (IP == "0.0.0.0") IP = "127.0.0.1" ;
  mtx.unlock();
  return IP;
}

std::string NEL_OSC::getBeSlimeName() {
  std::string n {""};
  mtx.lock();
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


