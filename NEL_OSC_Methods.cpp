//
//  NEL_OSC.cpp
//  NEL_OSC-macOS
//
//  Created by Cristian Andres Vogel on 12/07/2020.
//

#include "NEL_OSC_Methods.hpp"
#include <string>
#include <vector>

NEL_OSC::NEL_OSC( const char * IP , int port ) 
 {
   IP = IP;
   port = port;
   messageLog = std::make_unique<std::vector<std::string>>(2);
   SetReceivePort(9090);
 };
 NEL_OSC::~NEL_OSC() {  }; // to-do: close ports

void NEL_OSC::initOSCSender( const char* IP, int port ) {

if (oscSender == nullptr) {
  oscSender = std::make_unique<iplug::OSCSender>(IP, port );
  }
}

void NEL_OSC::initKyma( const char* kymaIP, int kymaPort ) {
  initOSCSender(kymaIP , kymaPort);
  sendOSC("/osc/respondTo", 1 ); //hardware handshake
}

void NEL_OSC::sendOSC( const std::string & addressStem, const std::vector<float> & args )
{
  if (!oscSender) initOSCSender();
  if (oscSendActive)
  {
    iplug::OscMessageWrite msg;
    msg.PushWord( addressStem.c_str() );
    for (int i=0; i<args.size() ; i++) {
      msg.PushFloatArg( args.at(i));
    }
    oscSender->SendOSCMessage(msg);
  }
}

void NEL_OSC::sendOSC( const std::string & addressStem, const std::vector<int> & args )
{
  if (!oscSender) initOSCSender();
  if (oscSendActive)
  {
    iplug::OscMessageWrite msg;
    msg.PushWord( addressStem.c_str() );
    for (int i=0; i<args.size() ; i++) {
      msg.PushIntArg( args.at(i));
    }
    oscSender->SendOSCMessage(msg);
  }
}

void NEL_OSC::sendOSC( const std::string & addressStem, const int & arg )
{
  if (!oscSender) initOSCSender();
  if (oscSendActive)
  {
    iplug::OscMessageWrite msg;
    msg.PushWord( addressStem.c_str() );
    msg.PushIntArg( arg);
    oscSender->SendOSCMessage(msg);
  }
}

void NEL_OSC::sendOSC( const std::string & addressStem, const float & arg )
{
  if (!oscSender) initOSCSender();
  if (oscSendActive)
  {
    iplug::OscMessageWrite msg;
    msg.PushWord( addressStem.c_str() );
    msg.PushFloatArg( arg);
    oscSender->SendOSCMessage(msg);
  }
}
 
void NEL_OSC::OnOSCMessage(iplug::OscMessageRead& msg)  {

  int nbrArgs = msg.GetNumArgs();
  char type;
  int index = 0;
  char oscMessage[128];
  char address[64];
  strcpy(address, "");
  const char * m = msg.PopWord();
  while (m)
  {
        strcat(address, "/");
        strcat(address, m);
        m = msg.PopWord();
    };


  while (nbrArgs) {

    msg.GetIndexedArg(index, &type);
    switch (type)
    {
        case 'i':
        {
          const int* pValue = msg.PopIntArg(false);
          if (pValue) sprintf(oscMessage, "%s %i", address, *pValue);
          break;
        }
        case 'f': {
          const float* pValue = msg.PopFloatArg(false);
          if (pValue) sprintf(oscMessage, "%s %f", address, *pValue);
          break;
        }
        case 's': {
          const char* pValue = msg.PopStringArg(false);
          if (pValue) sprintf(oscMessage, "%s %s", address, pValue);
          break;
        }
        default : break;
    }
    nbrArgs--;
  }
  messageLog->at(1) = messageLog->at(0);
  messageLog->at(0) = oscMessage;
}

bool NEL_OSC::newMessage() { return (messageLog->at(1) != messageLog->at(0));   }

std::string NEL_OSC::getLatestMessage() {
  std::string r =  messageLog->at(0);
  messageLog->at(0) = messageLog->at(1);
  return r; }
