#include "fidstr/AprilTagFiducial.h"

#include <apriltags/TagFamily.h>
#include <apriltags/Tag16h5.h>
#include <apriltags/Tag25h7.h>
#include <apriltags/Tag25h9.h>
#include <apriltags/Tag36h9.h>
#include <apriltags/Tag36h11.h>

#include <iostream>
#include <stdexcept>
#include <bitset>

namespace fidstr {

	AprilTagFiducialFamily::AprilTagFiducialFamily( AprilTagType _type ) :
		FiducialFamily( GetFamilyName( _type ), GetFamilySize( _type ) ),
		type( _type ),
		hammingDistance( GetFamilyHammingDistance( _type ) ),
		tagIDs( GetFamilyIDs( _type ) ),
		numTagBits( GetFamilyTagBits( _type ) ) {

		GenerateTags();
	}

	AprilTagFiducialFamily::AprilTagFiducialFamily( const std::vector<unsigned long long>& ids,
													unsigned int hDist, unsigned int nBits ) :
		FiducialFamily( "custom", ids.size() ),
		type( APRILTAG_CUSTOM ),
		hammingDistance( hDist ),
		IDs( ids ),
		tagIDs( IDs.data() ),
		numTagBits( nBits ) {

		GenerateTags();
	}

	void AprilTagFiducialFamily::GenerateTags() {

		// Tags are all square
		unsigned int tagWidth = std::sqrt( numTagBits );

		// Tags have a white and black border for visibility
		cv::Mat bitmapBase = 255*cv::Mat::ones( tagWidth + 4, tagWidth + 4, CV_8U );		
 		bitmapBase( cv::Rect_<int>( 1, 1, tagWidth + 2, tagWidth + 2 ) ) = 0.0;
		
		for( unsigned int n = 0; n < numIDs; n++ ) {

			cv::Mat bitmap = bitmapBase.clone();
			
			unsigned long long id = tagIDs[n];
			
			for( int i = tagWidth - 1; i >= 0 ; i-- ) {
				for( int j = tagWidth - 1; j >= 0; j-- ) {
// 					std::bitset<64> idbits( id );
// 					std::cout << "ID: " << idbits << " i: " << i << " j: " << j << std::endl;
					bitmap.at<unsigned char>(i+2, j+2) = 255*( id & 1 );
					id = id >> 1;
				}
			}

			std::stringstream ss;
			ss << name << "_ID" << n;
			Fiducial::Ptr fid = std::make_shared<Fiducial>( ss.str(), n, bitmap, *this );
			fiducials[n] = fid;
		}
		
	}

	Fiducial::Ptr AprilTagFiducialFamily::GetFiducial( unsigned int id ) const {
		try {
			Fiducial::Ptr fid = fiducials.at(id);
			return fid;
		}
		catch ( std::out_of_range e ) {
			std::stringstream ss;
			ss << "Fiducial id " << id << " exceeds family size " << numIDs;
			throw std::runtime_error( ss.str() );
		}
	}
	
		
	std::string AprilTagFiducialFamily::GetFamilyName( AprilTagType _type ) {
		switch( _type ) {
			case APRILTAG_16H5:
				return "AprilTag_16H5";
			case APRILTAG_25H7:
				return "AprilTag_27H5";
			case APRILTAG_25H9:
				return "AprilTag_25H9";
			case APRILTAG_36H9:
				return "AprilTag_36H9";
			case APRILTAG_36H11:
				return "AprilTag_36H11";
			default:
				throw std::runtime_error("Unrecognized AprilTag type.");
		}
	}

	unsigned int AprilTagFiducialFamily::GetFamilySize( AprilTagType _type ) {
		switch( _type ) {
			case APRILTAG_16H5:
				return sizeof( AprilTags::t16h5 )/sizeof( unsigned long long );
			case APRILTAG_25H7:
				return sizeof( AprilTags::t25h7 )/sizeof( unsigned long long );
			case APRILTAG_25H9:
				return sizeof( AprilTags::t25h9 )/sizeof( unsigned long long );
			case APRILTAG_36H9:
				return sizeof( AprilTags::t36h9 )/sizeof( unsigned long long );
			case APRILTAG_36H11:
				return sizeof( AprilTags::t36h11 )/sizeof( unsigned long long );
			default:
				throw std::runtime_error("Unrecognized AprilTag type.");
		}
	}

	unsigned int AprilTagFiducialFamily::GetFamilyHammingDistance( AprilTagType _type ) {
		switch( _type ) {
			case APRILTAG_16H5:
				return 5;
			case APRILTAG_25H7:
				return 7;
			case APRILTAG_25H9:
				return 9;
			case APRILTAG_36H9:
				return 9;
			case APRILTAG_36H11:
				return 11;
			default:
				throw std::runtime_error("Unrecognized AprilTag type.");
		}
	}

	const unsigned long long* AprilTagFiducialFamily::GetFamilyIDs( AprilTagType _type ) {
		switch( _type ) {
			case APRILTAG_16H5:
				return AprilTags::t16h5;
			case APRILTAG_25H7:
				return AprilTags::t25h7;
			case APRILTAG_25H9:
				return AprilTags::t25h9;
			case APRILTAG_36H9:
				return AprilTags::t36h9;
			case APRILTAG_36H11:
				return AprilTags::t36h11;
			default:
				throw std::runtime_error("Unrecognized AprilTag type.");
		}
	}

	unsigned int AprilTagFiducialFamily::GetFamilyTagBits( AprilTagType _type ) {
		switch( _type ) {
			case APRILTAG_16H5:
				return 16;
			case APRILTAG_25H7:
				return 25;
			case APRILTAG_25H9:
				return 25;
			case APRILTAG_36H9:
				return 36;
			case APRILTAG_36H11:
				return 36;
			default:
				throw std::runtime_error("Unrecognized AprilTag type.");
		}
	}
	
}

