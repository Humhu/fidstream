#ifndef _STREAM_PROTOCOL_H_
#define _STREAM_PROTOCOL_H_

#include "fidstr/Fiducial.h"

namespace fidstr {

	
	/*! \brief A streaming protocol has associated with it an encoding
	 * and decoding mechanism. */
	class StreamProtocol {
	public:

		StreamProtocol();

		/*! \brief Encodes the data as a seqeuence of fiducials. This may include non-data
		 * fiducials needed by the protocol. */
		virtual std::vector<Fiducial::Ptr> Encode( char* data, unsigned int numBytes ) = 0;

		/*! \brief Decodes the sequence of fiducials to a data array. This takes into
		 * account non-data fiducials needed by the protocol. */
		virtual std::vector<char> Decode( const std::vector<unsigned int> detectedIDs ) = 0;
		
	};
	
}

#endif