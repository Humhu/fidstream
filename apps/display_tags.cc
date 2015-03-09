#include "fidstr/AprilTagFiducial.h"

#include "fidstr/AprilTagPhysicalLayer.h"
#include "fidstr/PhysicalLayer.h"
#include "fidstr/DataLinkLayer.h"

#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

using namespace fidstr;

void CameraLoop() {

	AprilTagFiducialFamily family( APRILTAG_36H9 );

	AprilTagReceiver receiver( 15, family );
	FiducialPacketReceiver preceiver( family, receiver );

	cv::VideoCapture cap( 1 );
	cv::namedWindow( "capture" );

	unsigned int iter = 0;
	
	while( true ) {
		cv::Mat image;
		cap >> image;

		Time now = boost::posix_time::microsec_clock::universal_time();
		
		imshow( "capture", image );

// 		std::stringstream ss;
// 		ss << "capture_" << iter << ".jpg";

// 		imwrite( ss.str(), image );
// 		iter++;
		
 		receiver.Receive( image, now );
// 		preceiver.ProcessInputs();
// 
// 		Payload pl = preceiver.DequeuePayload();
// 
// 		if( !pl.empty() ) {
// 			std::cout << "Received payload of length " << pl.size() << std::endl;
// 		}
	}
	
}

int main( int argc, char* argv[] ) {

	unsigned int messageLength = 128;
	if( argc >= 2 ) {
		messageLength = stoi( argv[1] );
	}
	
	// TODO Check that frame length, etc. fit in the fiducials!
	AprilTagFiducialFamily family( APRILTAG_36H9 );
	
	cv::startWindowThread();

// 	boost::thread cameraThread( boost::bind( &CameraLoop ) );
	
	FiducialTransmitter transmitter( "AprilTagTransmitter", 15 );
	transmitter.SetWindowCoordinates( 0, 0 );
	transmitter.SetFullscreen( true );

	usleep( 0.25*1E6 );
	
	FiducialPacketTransmitter ptransmitter( family, transmitter, messageLength );

	// Create a dummy packet
	Packet packet;
	for( char i = 0; i < messageLength; i++ ) {
		packet.push_back( i );
	}

	Time startTime = boost::posix_time::microsec_clock::universal_time();
	ptransmitter.TransmitPacket( packet );
	Time endTime = boost::posix_time::microsec_clock::universal_time();

	TimeDuration transmissionTime = endTime - startTime;
	
	
	std::cout << "Transmission took " << transmissionTime << " at " <<
		packet.size() / (transmissionTime.total_milliseconds() * 1E-3)
		<< " bytes/sec." << std::endl;

	usleep( 2*1E6 );
		
	exit(0);
	
}