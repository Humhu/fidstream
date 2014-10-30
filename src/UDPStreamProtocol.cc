#include "fidstr/UDPStreamProtocol.h"

#include <boost/crc.hpp>

namespace fidstr {

	UDPStreamProtocol::UDPStreamProtocol( const FiducialFamily& _family ) :
		family( _family ) {}

	std::vector<Fiducial::Ptr> UDPStreamProtocol::Encode( char* data, unsigned int numBytes ) {

		boost::crc_32_type crc;
		crc.process_bytes( data, numBytes );
		unsigned int checksum = crc.checksum();
		
		std::vector<
		
	}

	std::vector<char> UDPStreamProtocol::Decode( const std::vector<unsigned int> detectedIDs ) {

	}
	
}