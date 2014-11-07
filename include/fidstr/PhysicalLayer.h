#ifndef _PHYSICAL_LAYER_H_
#define _PHYSICAL_LAYER_H_

#include "fidstr/FiducialStreamTypes.h"
#include "fidstr/Fiducial.h"

namespace fidstr {

	class FiducialDetection {
	public:
		
		typedef std::shared_ptr<FiducialDetection> Ptr;
		
		const Time timestamp;
		const unsigned int id;
		const FiducialFamily& family;

		// Image-frame detection coordinates
		const float imageX;
		const float imageY;

		// TODO Camera-relative pose
		FiducialDetection( const Time& _timestamp, unsigned int _id,
						   const FiducialFamily& _family, float _imageX,
						   float _imageY );
	};
	
	/*! \brief The physical layer (PHY) provides an abstraction to and from data
	 * packets and fiducial images. */

	/*! \brief A fiducial receiver finds and decodes fiducials in an image. */
	class FiducialReceiver {
	public:

		const double framerate;
		
		FiducialReceiver( double _framerate );

		void SetFramePeriodSlop( TimeDuration _framePeriodSlop );
		
		/*! \brief Receive and process a captured image. */
		virtual void Receive( const cv::Mat& input, Time timestamp ) = 0;

		/*! \brief Pops and returns the earliest detected fiducial off the
		 * output queue, or null if there are no detections. */
		FiducialDetection::Ptr DequeueDetection();

	protected:

		/*! \brief Adds the detection to the queue if it has occurred more than
		 * the frame period after the previous detection. */
		void ProcessDetection( const FiducialDetection::Ptr& det );
		
	private:

		Time lastDetectionTime;
		TimeDuration framePeriod;
		TimeDuration framePeriodSlop;
		
		mutable Mutex outputMutex;
		std::deque<FiducialDetection::Ptr> outputBuffer;

	};

	/*! \brief A fiducial transmitter displays fiducial images on an output device. */
	class FiducialTransmitter {
	public:

		const std::string name;
		const double framerate;
		
		// TODO Framerate and "busy" monitoring for framerate
		
		FiducialTransmitter( const std::string& _name, double _framerate );

		void SetWindowCoordinates( unsigned int x, unsigned int y );
		void SetFullscreen( bool fullscreen );
		
		/*! \brief Transmit (display) a fiducial. Returns as soon as transmission begins,
		 * but blocks if a previous transmission is in progress. */
		void Transmit( const Fiducial::Ptr& fid );

	private:

		Time transmissionStartTime;
		TimeDuration transmissionPeriod;
		
	};

// 	/*! \brief Encodes a data byte array as an array of fiducials using the specified family. */
// 	std::vector<Fiducial::Ptr> EncodeArray( char* data, unsigned int numBytes,
// 											const FiducialFamily& family );
// 
// 	/*! \brief Decodes an array of fiducials to a data byte array using the specified family. */
// 	std::vector<char> DecodeArray( const std::vector<unsigned int> detectedIDs,
// 								const FiducialFamily& family );

}


#endif