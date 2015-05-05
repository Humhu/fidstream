#include "fidstr/AprilTagPhysicalLayer.h"

#include <stdexcept>

#include <opencv2/imgproc/imgproc.hpp>

#include <boost/foreach.hpp>

#include <apriltags/Tag16h5.h>
#include <apriltags/Tag25h7.h>
#include <apriltags/Tag25h9.h>
#include <apriltags/Tag36h9.h>
#include <apriltags/Tag36h11.h>

namespace fidstr {

	AprilTagReceiver::AprilTagReceiver( double _framerate, AprilTagFiducialFamily& _family ) :
		FiducialReceiver( _framerate ),
		family( _family ) {

		switch( family.type ) {
			case APRILTAG_16H5:
				detector = std::make_shared<AprilTags::TagDetector>( AprilTags::tagCodes16h5 );
				break;
			case APRILTAG_25H7:
				detector = std::make_shared<AprilTags::TagDetector>( AprilTags::tagCodes25h7 );
				break;
			case APRILTAG_25H9:
				detector = std::make_shared<AprilTags::TagDetector>( AprilTags::tagCodes25h9 );
				break;
			case APRILTAG_36H9:
				detector = std::make_shared<AprilTags::TagDetector>( AprilTags::tagCodes36h9 );
				break;
			case APRILTAG_36H11:
				detector = std::make_shared<AprilTags::TagDetector>( AprilTags::tagCodes36h11 );
				break;
			default:
				throw std::runtime_error( "Unrecognized April tag type!" );
		}
	}

	void AprilTagReceiver::Receive( const cv::Mat& input, Time timestamp) {

		cv::Mat gray;
		
#ifdef OPENCV3
		cv::cvtColor( input, gray, cv::COLOR_BGR2GRAY );
#else
		cv::cvtColor(input, gray, CV_BGR2GRAY);		
#endif
		std::vector<AprilTags::TagDetection> detections = detector->extractTags( gray );

		BOOST_FOREACH( const AprilTags::TagDetection& detection, detections ) {

			FiducialDetection::Ptr fdet = std::make_shared<FiducialDetection>(
				timestamp, detection.id, family, detection.cxy.first, detection.cxy.second );
			ProcessDetection( fdet );
		}
	}
	
}
