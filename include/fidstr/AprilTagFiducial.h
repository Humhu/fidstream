#ifndef _APRILTAG_FIDUCIAL_H_
#define _APRILTAG_FIDUCIAL_H_

#include "fidstr/Fiducial.h"

namespace fidstr {

	enum AprilTagType {
		APRILTAG_16H5,
		APRILTAG_25H7,
		APRILTAG_25H9,
		APRILTAG_36H9,
		APRILTAG_36H11,
		APRILTAG_CUSTOM
	};
	
	class AprilTagFiducialFamily : public FiducialFamily {
	public:

		const AprilTagType type;
		const unsigned int numTagBits;
		const unsigned int hammingDistance;
		
		AprilTagFiducialFamily( AprilTagType _type );
		AprilTagFiducialFamily( const std::vector<unsigned long long>& ids,
								unsigned int hDist, unsigned int nBits );

		virtual Fiducial::Ptr GetFiducial( unsigned int id ) const;
		
	protected:

		std::map <unsigned int, Fiducial::Ptr> fiducials;
		
		/*! \brief Generates the AprilTag bitmaps from their IDs. */
		virtual void GenerateTags();

	private:

		std::vector<unsigned long long> IDs;
		const unsigned long long* tagIDs; // The AprilTag header array
		
		static std::string GetFamilyName( AprilTagType _type );
		static unsigned int GetFamilySize( AprilTagType _type );
		static unsigned int GetFamilyHammingDistance( AprilTagType _type );
		static const unsigned long long* GetFamilyIDs( AprilTagType _type );
		static unsigned int GetFamilyTagBits( AprilTagType _type );
		
	};
	
}

#endif