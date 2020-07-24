//
//  packetListener.cpp
//  NEL_VirtualControlSurface-macOS
//
//  Created by Cristian Andres Vogel on 22/07/2020.
//

#include "NEL_PacketListener.hpp"
#include <vector>
#include <sstream>

#define LOCALHOST "127.0.0.1"


NEL_PacketListener::NEL_PacketListener(  int port , PacketListener& listener) : m_destinationPort(port) {
 
  
  try {
         m_receiveSocket = std::make_unique<UdpListeningReceiveSocket>(
                IpEndpointName( IpEndpointName::ANY_ADDRESS, m_destinationPort ),
                &listener );
    
       std::cout << "\n  ▶︎ listener socket created on port  " << m_destinationPort << "  \n";
    } catch (std::runtime_error e) {

      // catch UDP error
      // todo: do something about it
      std::cout << "\n  ▶︎ error creating socket!  " << e.what() << "  \n";
    }
  
};

NEL_PacketListener::~NEL_PacketListener() {}

void NEL_PacketListener::ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint)  {
 
  
    const char * addr = m.AddressPattern();
 
    try{
        // example of parsing single messages. osc::OsckPacketListener
        // handles the bundle traversal.
        
        if( std::strcmp( addr, "/osc/response_from" ) == 0 ){
            // example #1 -- argument stream interface
            osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
            osc::int32 a1;
            args >> a1 >> osc::EndMessage;
            
            std::cout << "received '/osc/response_from' message with arguments: "
                << a1 << "\n";
            
        } else {
            // example #2 -- argument iterator interface, supports
            // reflection for overloaded messages (eg you can call
            // (*arg)->IsBool() to check if a bool was passed etc).
          
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            if( arg == m.ArgumentsEnd() )
                         throw osc::MissingArgumentException();
                        
          std::vector<float> floatArgs;
          std::basic_stringstream<char> msgAndArgs;
          while ( arg != m.ArgumentsEnd() ) {
          if ( arg->IsFloat() ) { floatArgs.push_back( (arg++) -> AsFloat() ); }
//todo:
//          if ((arg++)->IsInt32()) { }
//          if ((arg++)->IsBlob()) { }
          }
          msgAndArgs << addr;
          for ( float f: floatArgs) {
            
           msgAndArgs << " " << f;
          }
          setMostRecentMessage(msgAndArgs.str());
        }
    } catch ( osc::Exception& e ) {
            // any parsing errors such as unexpected argument types, or
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: " << m.AddressPattern() << ": " << e.what() << "\n";
            messageReceived = false;
            setMostRecentMessage( "⚠︎ parsing error" );
    }
}

void NEL_PacketListener::setMostRecentMessage( const std::string& msg ) {
  
  msgMutex.lock();
    mostRecentMessage = msg;
    messageReceived = true;
  msgMutex.unlock();
}

std::string NEL_PacketListener::getMostRecentMessage() {
  msgMutex.lock();
    std::string msg{ "" };
    msg = mostRecentMessage;
  msgMutex.unlock();
  return msg;
}

