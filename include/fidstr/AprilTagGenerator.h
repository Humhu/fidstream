#ifndef _APRILTAG_GENERATOR_H_
#define _APRILTAG_GENERATOR_H_

#include <vector>
#include <set>
#include <bitset>
#include <iostream>

#include "fidstr/SynchronizationPrimitives.h"

#include <boost/thread.hpp>
#include <boost/foreach.hpp>

#include <Eigen/Dense>

namespace fidstr {

	/*! \brief Generates tags of size N by N */
	template <int N>
	class AprilTagGenerator {
	public:

		constexpr static unsigned int numBits = N*N;
		
		typedef unsigned long long TagCode;
		typedef Eigen::Matrix<bool, N, N> TagMatrix;
		typedef std::bitset<numBits> TagBits;

		AprilTagGenerator( unsigned int _minHammingDistance, unsigned int _minComplexity,
						   unsigned long long _searchStride );

		std::vector<TagCode> GenerateCodes( unsigned int numThreads );
		
		const unsigned int minHammingDistance; // Min different bits between tags
		const unsigned int minComplexity; // Minimum rank of tag matrix
		const unsigned long long searchStride; // A largeish prime number

		static TagMatrix BitsToMatrix( const TagBits& bits );
		static TagBits MatrixToBits( const TagMatrix& matrix );
		static unsigned int CalculateHammingDistance( TagCode code1, TagCode code2 );
		static unsigned int CalculateComplexity( TagCode code ); // Approximated by rank
		
	private:

		mutable Mutex mutex;
		bool resultFound;
		Semaphore threadReturns;
		Semaphore threadDispatcher;
		
		TagCode latestCode;
		unsigned int latestIndex;

		std::vector<unsigned int> startIndAssignments;
		unsigned int indStrideSize;
		
		std::vector<TagCode> codes; // All valid codes found so far
		std::vector<TagCode> rotatedCodes; // All valid codes so far and their rotations
		std::set<TagCode> codeSet;

		/*! \brief Check code against all rotated codes for hamming distance and complexity. */
		bool CheckCode( TagCode code ) const;
		
 		void SearchThread( unsigned int threadID );
		bool ResultFound() const;
		void ReportResult( TagCode code, unsigned int index );
		
	};

	#include "fidstr/AprilTagGenerator.hpp"
	
}

#endif