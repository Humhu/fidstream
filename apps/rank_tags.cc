#include "fidstr/AprilTagFiducial.h"
#include "fidstr/AprilTagGenerator.h"

#include "fidstr/PhysicalLayer.h"

#include <iostream>

#include <apriltags/TagFamily.h>
#include <apriltags/Tag16h5.h>
#include <apriltags/Tag25h7.h>
#include <apriltags/Tag25h9.h>
#include <apriltags/Tag36h9.h>
#include <apriltags/Tag36h11.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

using namespace fidstr;

int main( int argc, char* argv[] ) {

	AprilTagFiducialFamily family( APRILTAG_36H9 );
	std::cout << "Expecting family of size " << family.numIDs << std::endl;
	
	unsigned int numTags = sizeof( AprilTags::t36h9 )/sizeof( unsigned long long );
	const unsigned long long* ids = AprilTags::t36h9;

	unsigned int minComplexity = std::numeric_limits<unsigned int>::max();
	unsigned int maxComplexity = 0;
	
	for( unsigned int i = 0; i < numTags; i++ ) {

		unsigned long long code = ids[i];
		unsigned int complexity = AprilTagGenerator<4>::CalculateComplexity( code );
		if( complexity < minComplexity ) {
			minComplexity = complexity;
		}
		if( complexity > maxComplexity ) {
			maxComplexity = complexity;
		}
	}

	std::cout << "Min complexity: " << minComplexity << " Max complexity: " << maxComplexity << std::endl;

	AprilTagGenerator<5> gen( 9, 5, 982451653 );
	std::vector<unsigned long long> codes = gen.GenerateCodes( 4 );

	std::cout << "Found " << codes.size() << " codes." << std::endl;

	AprilTagFiducialFamily cfamily( codes, 9, 25 );

	cv::startWindowThread(); // TODO
	FiducialTransmitter transmitter( "Output", 1 );
	transmitter.SetWindowCoordinates( 0, 0 );
	
	for( unsigned int i = 0; i < cfamily.numIDs; i++ ) {
		Fiducial::Ptr fid = cfamily.GetFiducial( i );
		transmitter.Transmit( fid );
	}
	
}