#include "fidstr/AprilTagFiducial.h"

#include "fidstr/PhysicalLayer.h"
#include "fidstr/DataLinkLayer.h"

#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace fidstr;

int main( int argc, char* argv[] ) {

	// TODO Check that frame length, etc. fit in the fiducials!
	AprilTagFiducialFamily family( APRILTAG_36H9 );

	cv::startWindowThread();

	FiducialTransmitter transmitter( "AprilTagTransmitter", 30 );
	transmitter.SetWindowCoordinates( 0, 0 );
	transmitter.SetFullscreen( true );

	FiducialPacketTransmitter ptransmitter( family, transmitter, 16 );

	// Create a dummy packet
	Packet packet;
	for( char i = 0; i < 64; i++ ) {
		packet.push_back( i );
	}

	ptransmitter.TransmitPacket( packet );

	std::cout << "Transmission complete." << std::endl;
	exit(0);
	
}