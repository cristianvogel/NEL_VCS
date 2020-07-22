//
//  packetListener.cpp
//  NEL_VirtualControlSurface-macOS
//
//  Created by Cristian Andres Vogel on 22/07/2020.
//

#include "NEL_PacketListener.hpp"

#define LOCALHOST "127.0.0.1"
#define PORT 8080

NEL_PacketListener::NEL_PacketListener(  int port , PacketListener listener) : _destinationPort(port) {
 
  
  
  try {
       UdpListeningReceiveSocket s(
                IpEndpointName( IpEndpointName::ANY_ADDRESS, PORT ),
                &listener );
    } catch (std::runtime_error e) {

      // catch UDP error
      // todo: do something about it
      std::cout << "\n  ▶︎ error creating socket!  " << e.what() << "  \n";
    }
  
};

NEL_PacketListener::~NEL_PacketListener() {}

void NEL_PacketListener::ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint)  {
  
  std::cout << m.AddressPattern() << std::endl;
    try{
        // example of parsing single messages. osc::OsckPacketListener
        // handles the bundle traversal.
        
        if( std::strcmp( m.AddressPattern(), "/osc/respond_to" ) == 0 ){
            // example #1 -- argument stream interface
            osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
            osc::int32 a1;
            args >> a1 >> osc::EndMessage;
            
            std::cout << "received '/osc/respond_to' message with arguments: "
                << a1 << "\n";
            
        } else if( std::strcmp( m.AddressPattern(), "/test2" ) == 0 ){
            // example #2 -- argument iterator interface, supports
            // reflection for overloaded messages (eg you can call
            // (*arg)->IsBool() to check if a bool was passed etc).
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            bool a1 = (arg++)->AsBool();
            int a2 = (arg++)->AsInt32();
            float a3 = (arg++)->AsFloat();
            const char *a4 = (arg++)->AsString();
            if( arg != m.ArgumentsEnd() )
                throw osc::ExcessArgumentException();

            std::cout << "received '/test2' message with arguments: "
                << a1 << " " << a2 << " " << a3 << " " << a4 << "\n";
        }
      
    }catch( osc::Exception& e ){
        // any parsing errors such as unexpected argument types, or
        // missing arguments get thrown as exceptions.
        std::cout << "error while parsing message: "
            << m.AddressPattern() << ": " << e.what() << "\n";
    }
}



