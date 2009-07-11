#include "osc.h"
#include <string>
#include <sstream>
#include <iostream> //rkh
#include "oscpack/osc/OscOutboundPacketStream.h"
#include "oscpack/ip/UdpSocket.h"
#include "oscpack/ip/IpEndpointName.h"
#include "oscpack/osc/OscReceivedElements.h"
#include "oscpack/osc/OscPacketListener.h"
#define ADDRESS "192.168.176.56"
#define PORT 7000

namespace oscplugin {
//osc_input_vars gOSCvars;


// if dynamic IP is enabled, ip's will be selected from an array populated by the user
// currently, ips are set in the client and assed in 
void getIPAddress (int index){
	std::cout << "testing OSC library" << std::endl;
}

#define OUTPUT_BUFFER_SIZE 2048

void sendOSCmessage(ball_coordinates currentClient)
{
std::string str(currentClient.port);
std::istringstream strin(str);
int port;
strin >> port;

   char buffer[OUTPUT_BUFFER_SIZE];
   osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
   UdpTransmitSocket socket( IpEndpointName( currentClient.hostname, port ));
   p.Clear();

p << osc::BeginMessage( "/ball" )
    << (float)currentClient.ball_x
    << (float)currentClient.ball_y
    << (float)currentClient.ball_z
  << osc::EndMessage;

   if(p.IsReady()){ socket.Send( p.Data(), p.Size() );}
}

void sendOSCbundle(ball_coordinates currentClient) {

	std::string str(currentClient.port);
	std::istringstream strin(str);
	int port;
	strin >> port;

	char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
	UdpTransmitSocket socket( IpEndpointName( currentClient.hostname, port ));
	p.Clear();

	p << osc::BeginBundle()
	<< osc::BeginMessage( "/x" ) << currentClient.origin[0] << osc::EndMessage
	<< osc::BeginMessage( "/y" ) << currentClient.origin[1] << osc::EndMessage
	<< osc::BeginMessage( "/z" ) << currentClient.origin[2] << osc::EndMessage
	<< osc::EndBundle;

   if(p.IsReady()){ socket.Send( p.Data(), p.Size() );}
}
}

