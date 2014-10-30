#include "fidstr/Fiducial.h"

namespace fidstr {

	Fiducial::Fiducial( const std::string& _name, unsigned int _id,
						const cv::Mat& _bitmap, const FiducialFamily& _family ) :
		name( _name ),
		id( _id ),
		bitmap( _bitmap.clone() ),
		family( _family ) {}
	
	FiducialFamily::FiducialFamily( const std::string& _name, unsigned int numIDs ) :
		name( _name ),
		numIDs( numIDs ),
		numDataBits( std::floor( std::log2( numIDs - 1 ) ) ) {}

	
	
}