#ifndef _RAW_STREAM_PROTOCOL_H_
#define _RAW_STREAM_PROTOCOL_H_

#include "fidstr/Fiducial.h"

namespace fidstr {

	class RawStreamProtocol {
	public:

		FiducialEncoder( const FiducialFamily& _family );

		std::vector<Fiducial::Ptr> Encode( char* _data, 
		
	private:
		
	};
	
}

#endif