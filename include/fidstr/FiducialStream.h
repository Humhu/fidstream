#ifndef _FIDUCIAL_STREAM_H_
#define _FIDUCIAL_STREAM_H_

#include "fidstr/FiducialFamily.h"

namespace fidstr {

	class FiducialStream {
	public:

		/*! \brief Converts the data to a stream using the specified family. */
		FiducialStream( char* _data, unsigned int _numBytes, const FiducialFamily& _family );

	private:

		char* data;
		unsigned int numBytes;
		unsigned int index;
		const FiducialFamily& family;

		std::vector<Fiducial::Ptr> fiducialStream;
		
		void ConvertData();
		
	};
	
}

#endif