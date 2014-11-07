#ifndef _APRILTAG_PHYSICAL_LAYER_H_
#define _APRILTAG_PHYSICAL_LAYER_H_

#include "fidstr/PhysicalLayer.h"
#include "fidstr/AprilTagFiducial.h"

#include "apriltags/TagDetector.h"

namespace fidstr {

	class AprilTagReceiver : public FiducialReceiver {
	public:
		
		AprilTagReceiver( double _framerate, AprilTagFiducialFamily& _family );

		virtual void Receive( const cv::Mat& input, Time timestamp );

	private:

		AprilTagFiducialFamily& family;
		
		std::shared_ptr<AprilTags::TagDetector> detector;
		
	};

}

#endif