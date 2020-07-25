//
//  packetListener.hpp
//  NEL_VirtualControlSurface-macOS
//
//  Created by Cristian Andres Vogel on 22/07/2020.
//

#pragma once

#include <iostream>
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"
#include <vector>

class NEL_PacketListener : public osc::OscPacketListener {
  
public:
  
  NEL_PacketListener(  int , PacketListener& );
  ~NEL_PacketListener();
  
  const char * m_destinationHost;
  int m_destinationPort;
  std::unique_ptr<UdpListeningReceiveSocket> m_receiveSocket;
  
  std::atomic_bool messageReceived{false};
  
  const std::string getMostRecentMessage( );
  const std::string getMostRecentAddress( );
  const std::vector<float> getMostRecentFloatArgs( );
  
  void setMostRecentMessage( const std::string& );
  void setMostRecentAddress( const std::string& );
  void setMostRecentFloatArgs( const std::vector<float>& );
  
  std::mutex msgMutex;

protected:
  void ProcessMessage( const osc::ReceivedMessage& m,
                                const IpEndpointName& remoteEndpoint ) ;
  std::vector<float> m_floatArgs;
  std::string mostRecentMessage{""};
  std::string mostRecentAddr{""};
};




