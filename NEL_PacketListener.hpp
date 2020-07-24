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


class NEL_PacketListener : public osc::OscPacketListener {
  
public:
  
  NEL_PacketListener(  int , PacketListener& );
  ~NEL_PacketListener();
  
  const char * m_destinationHost;
  int m_destinationPort;
  std::unique_ptr<UdpListeningReceiveSocket> m_receiveSocket;
  
  std::atomic_bool messageReceived{false};
  std::string mostRecentMessage{""};
  
  std::string getMostRecentMessage(  );
  void setMostRecentMessage( const std::string& );
  std::mutex msgMutex;

protected:
  void ProcessMessage( const osc::ReceivedMessage& m,
                                const IpEndpointName& remoteEndpoint ) ;

};




