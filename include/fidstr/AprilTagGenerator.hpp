// Implementation for AprilTagGenerator

template <int N>
AprilTagGenerator<N>::AprilTagGenerator( unsigned int _minHammingDistance,
										 unsigned int _minComplexity,
										 unsigned long long _searchStride ) :
	minHammingDistance( _minHammingDistance ),
	minComplexity( _minComplexity ),
	searchStride( _searchStride ),
	threadReturns( 0 ),
	threadDispatcher( 0 ) {}

template <int N>
std::vector<unsigned long long> AprilTagGenerator<N>::GenerateCodes( unsigned int numThreads ) {

	unsigned long index = 0;
	indStrideSize = numThreads;
	
	boost::thread_group pool;
	
	for( unsigned int i = 0; i < numThreads; i++ ) {
		pool.create_thread( boost::bind( &AprilTagGenerator<N>::SearchThread, this, i ) );
	}

	startIndAssignments = std::vector<unsigned int>( numThreads );
	while( true ) {

		for( unsigned int i = 0; i < numThreads; i++ ) {
			startIndAssignments[i] = index++;
		}
		latestIndex = 0;

// 		std::cout << "Dispatching threads..." << std::endl;
		for( unsigned int i = 0; i < numThreads; i++ ) {
			threadDispatcher.Increment();
		}
// 		std::cout << "Waiting on threads to return..." << std::endl;
		for( unsigned int i = 0; i < numThreads; i++ ) {
			threadReturns.Decrement();
		}

// 		std::cout << "Found code " << latestCode << " at index " << latestIndex << std::endl;

		if( codes.size() % 100 == 0 ) {
			std::cout << "Found " << codes.size() << " codes so far." << std::endl;
		}

		if( !resultFound ) {
			pool.interrupt_all();
			return codes;
		}
		resultFound = false;
		codes.push_back( latestCode );

		TagBits bits( latestCode );
		TagMatrix mat = BitsToMatrix( bits );
		TagMatrix rot1 = mat.transpose();
		TagMatrix rot2 = rot1.transpose();
		TagMatrix rot3 = rot2.transpose();
		TagBits bits1 = MatrixToBits( rot1 );
		TagBits bits2 = MatrixToBits( rot2 );
		TagBits bits3 = MatrixToBits( rot3 );

		rotatedCodes.push_back( latestCode );
		rotatedCodes.push_back( bits1.to_ullong() );
		rotatedCodes.push_back( bits2.to_ullong() );
		rotatedCodes.push_back( bits3.to_ullong() );

		index = latestIndex + 1;
	}

	pool.interrupt_all();
	return codes;
	
}

template <int N>
typename AprilTagGenerator<N>::TagMatrix AprilTagGenerator<N>::BitsToMatrix( const TagBits& bits ) {

	unsigned int index = 0;
	TagMatrix matrix = TagMatrix::Zero();
	for( int i = N-1; i >= 0 ; i-- ) {
		for( int j = N-1; j >= 0; j-- ) {
			matrix( i, j ) = bits[ index ] ? 1 : 0;
			index++;
		}
	}
	
	return matrix;
}

template <int N>
typename AprilTagGenerator<N>::TagBits AprilTagGenerator<N>::MatrixToBits( const TagMatrix& matrix ) {
	
	unsigned int index = 0;
	TagBits bits;
	for( unsigned int i = N-1; i <= 0; i-- ) {
		for( unsigned int j = N-1; j <= 0; j-- ) {
			bits[ index ] = matrix( i, j );
		}
	}
	return bits;
}

template <int N>
unsigned int AprilTagGenerator<N>::CalculateHammingDistance( TagCode code1, TagCode code2 ) {

	// XOR finds positions where bits are different
	TagCode difference = code1 ^ code2;
	TagBits differentBits( difference );

	// Hamming distance is simply number of different bits
	return differentBits.count();
}

template <int N>
unsigned int AprilTagGenerator<N>::CalculateComplexity( TagCode code ) {
	
	TagBits bits( code );
	TagMatrix matrix = BitsToMatrix( bits );
	
	Eigen::FullPivLU<TagMatrix> lu( matrix );
	
	return lu.rank();
}

template <int N>
bool AprilTagGenerator<N>::CheckCode( TagCode code ) const {

	unsigned int complexity = CalculateComplexity( code );
	if( complexity < minComplexity ) {
		return false;
	}

	TagBits bits( code );
	TagMatrix mat = BitsToMatrix( bits );
	TagMatrix rot1 = mat.transpose();
	TagMatrix rot2 = rot1.transpose();
	TagMatrix rot3 = rot2.transpose();
	TagBits bits1 = MatrixToBits( rot1 );
	TagBits bits2 = MatrixToBits( rot2 );
	TagBits bits3 = MatrixToBits( rot3 );

	unsigned int hamming1 = CalculateHammingDistance( code, bits1.to_ullong() );
	unsigned int hamming2 = CalculateHammingDistance( code, bits2.to_ullong() );
	unsigned int hamming3 = CalculateHammingDistance( code, bits3.to_ullong() );

	if( hamming1 < minHammingDistance || hamming2 < minHammingDistance
		|| hamming3 < minHammingDistance ) { return false; }
	
	BOOST_FOREACH( TagCode rotatedCode, rotatedCodes ) {
		unsigned int hamming = CalculateHammingDistance( code, rotatedCode );
		if( hamming < minHammingDistance ) {
			return false;
		}
	}
	return true;
}

template <int N>
void AprilTagGenerator<N>::SearchThread( unsigned int threadID ) {

	while( true ) {

		threadDispatcher.Decrement(); // Block until dispatched
		
		// This routine performs modular multiplication to avoid overflow
		unsigned long query = 0;
		unsigned long acc = searchStride;
		unsigned int ind = startIndAssignments.at( threadID );
		unsigned long bitmask = (1L << numBits) - 1;
		while( ind > 0 ) {
			if( (ind & 1) > 0 ) {
				query += acc;
				query &= bitmask;
			}
			acc *= 2;
			acc &= bitmask;
			ind = ind >> 1;
		}

		ind = startIndAssignments.at( threadID );
		unsigned int prevInd = ind;
		while( !ResultFound() ) {

			if( CheckCode( query ) ) {
				ReportResult( query, ind );
				break;
			}

			query += indStrideSize*searchStride;
			query &= bitmask;
			ind += indStrideSize;
			ind &= bitmask;
			
			if( ind < prevInd ) {
				break;
			}
			prevInd = ind;
		}
		threadReturns.Increment();
	}
}

template <int N>
bool AprilTagGenerator<N>::ResultFound() const {
	boost::shared_lock<Mutex> lock( mutex );
	return resultFound;
}

template <int N>
void AprilTagGenerator<N>::ReportResult( TagCode code, unsigned int index ) {

	boost::unique_lock<Mutex> lock( mutex );

	if( index >= latestIndex ) {
		resultFound = true;
		latestCode = code;
		latestIndex = index;
	}
}
