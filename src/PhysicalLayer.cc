#include "fidstr/PhysicalLayer.h"
#include "fidstr/SerialPacketizer.h"

#include <opencv2/highgui/highgui.hpp>

namespace fidstr {

	FiducialDetection::FiducialDetection( const Time& _timestamp, unsigned int _id,
										  const FiducialFamily& _family, float _imageX,
										  float _imageY ) :
		timestamp( _timestamp ),
		id( _id ),
		family( _family ),
		imageX( _imageX ),
		imageY( _imageY ) {}
		
	
	FiducialReceiver::FiducialReceiver( double _framerate ) :
		framerate( _framerate ) {

		lastDetectionTime = boost::posix_time::microsec_clock::universal_time();
		framePeriod = boost::posix_time::microseconds( std::round( 1E6/framerate ) );
		framePeriodSlop = boost::posix_time::microseconds( 0 );
	}

	void FiducialReceiver::SetFramePeriodSlop( TimeDuration _framePeriodSlop ) {
		framePeriodSlop = _framePeriodSlop;
	}
	
	
	void FiducialReceiver::ProcessDetection( const FiducialDetection::Ptr& det ) {
		WriteLock lock( outputMutex );

		TimeDuration dt = det->timestamp - lastDetectionTime;
		if( dt > framePeriod - framePeriodSlop ) {
// 			std::cout << "ID " << det->id << " with dt of " << dt << " accepted." << std::endl;
			lastDetectionTime = det->timestamp;
			outputBuffer.push_back( det );
		}
		else {
// 			std::cout << "ID " << det->id << " with dt of " << dt << " rejected!" << std::endl;
		}
	}
	
	FiducialDetection::Ptr FiducialReceiver::DequeueDetection() {
		WriteLock lock( outputMutex );

		FiducialDetection::Ptr detection;
		if( !outputBuffer.empty() ) {
			detection = outputBuffer.front();
			outputBuffer.pop_front();
		}

		return detection;
	}

	FiducialTransmitter::FiducialTransmitter( const std::string& _name, double _framerate ) :
		name( _name ), framerate( _framerate ) {

		cv::namedWindow( name, CV_WINDOW_NORMAL );
		transmissionStartTime = boost::posix_time::microsec_clock::universal_time();
		transmissionPeriod = boost::posix_time::microseconds( std::round( 1E6/framerate ) );
	}

	void FiducialTransmitter::SetWindowCoordinates( unsigned int x, unsigned int y ) {
		cv::moveWindow( name, x, y );
	}

	void FiducialTransmitter::SetFullscreen( bool fullscreen ) {

		if( fullscreen ) {
			cv::setWindowProperty( name, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN );
		}
		else {
			cv::setWindowProperty( name, CV_WND_PROP_FULLSCREEN, CV_WINDOW_NORMAL );
		}
	}

	void FiducialTransmitter::Transmit( const Fiducial::Ptr& fid ) {

		while( true ) {
			Time now = boost::posix_time::microsec_clock::universal_time();
			TimeDuration sinceStart = now - transmissionStartTime;

			if( sinceStart < transmissionPeriod ) {
				TimeDuration remaining = transmissionPeriod - sinceStart;
				boost::this_thread::sleep( remaining );
			}
			else {
				break;
			}
		}

		std::cout << "Transmitting fiducial ID: " << fid->id << std::endl;
		cv::imshow( name, fid->bitmap );
		transmissionStartTime = boost::posix_time::microsec_clock::universal_time();
	}
	
// 	std::vector<Fiducial::Ptr> EncodeArray( char* data, unsigned int numBytes,
// 											 const FiducialFamily& family ) {
// 		
// 		SerialPacketizer packetizer( data, numBytes );
// 		unsigned int numFiducials = std::ceil( ((float) numBytes) / family.numDataBits );
// 		
// 		std::vector<Fiducial::Ptr> fiducials( numFiducials );
// 		
// 		for( unsigned int i = 0; i < numFiducials; i++ ) {
// 			unsigned int packet = packetizer.GetPacket( family.numDataBits );
// 			fiducials[i] = family.GetFiducial( packet );
// 		}
// 		return fiducials;
// 	}
// 
// 	std::vector<char> DecodeArray( std::vector<unsigned int> detected,
// 									const FiducialFamily& family ) {
// 
// 		SerialDepacketizer depacketizer;
// 
// 		for( unsigned int i = 0; i < detected.size(); i++ ) {
// 			unsigned int id = detected[i];
// 			depacketizer.AddPacket( id, family.numDataBits );
// 		}
// 
// 		return depacketizer.Finalize();
// 	}
	
}