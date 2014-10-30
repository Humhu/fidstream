#ifndef _FIDUCIAL_H_
#define _FIDUCIAL_H_

#include <opencv2/core/core.hpp>

#include <map>
#include <memory>

namespace fidstr {

	class FiducialFamily;
	
	class Fiducial {
	public:

		typedef std::shared_ptr<Fiducial> Ptr;
		
		const std::string name;
		const unsigned int id;
		const cv::Mat bitmap;
		const FiducialFamily& family;

		Fiducial( const std::string& _name, unsigned int _id,
				  const cv::Mat& _bitmap, const FiducialFamily& _family );
		
	};
	
	class FiducialFamily {
	public:

		const std::string name;
		const unsigned int numIDs;
		const unsigned int numDataBits; // Calculated as log2(NumIDs - 2)
		
		FiducialFamily( const std::string& _name, unsigned int numIDs );

		virtual Fiducial::Ptr GetFiducial( unsigned int id ) const = 0;
		
	};

}

#endif