#ifndef _UDP_STREAM_PROTOCOL_H_
#define _UDP_STREAM_PROTOCOL_H_

#include "fidstr/StreamProtocol.h"

namespace fidstr {

	/*! \brief A simple UDP-based streaming protocol. */
	class UDPStreamProtocol {
	public:

		UDPStreamProtocol( const FiducialFamily& _family );

		virtual std::vector<Fiducial::Ptr> Encode( char* data, unsigned int numBytes );

		virtual std::vector<char> Decode( const std::vector<unsigned int> detectedIDs );

	private:

		FiducialFamily family;
		
	};

}

#endif