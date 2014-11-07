#include "fidstr/SerialPacketizer.h"

#include <stdexcept>

#include <iostream>
#include <cmath>

namespace fidstr {

	SerialPacketizer::SerialPacketizer( const char* _source, unsigned int _sourceLength ) :
		source( _source ),
		sourceLength( _sourceLength ),
		sourceIndex( 0 ),
		bitIndex( 0 ) {
			
		if( _source == nullptr ) {
			throw std::runtime_error( "Null source!" );
		}
		if( _sourceLength == 0 ) {
			throw std::runtime_error( "Zero-length source!" );
		}
		currentByte = source[0];
	}

	unsigned int SerialPacketizer::GetPacket( unsigned int packetSize ) {

		unsigned int packet = 0;
		unsigned int numBitsRemaining = packetSize;

		while( numBitsRemaining > 0 ) {

			if( !HasBits() ) {
				break;
			}

			unsigned int currentByte = source[ sourceIndex ];
			
			// Bit mask for current bytes
			unsigned int mask = (1 << (8 - bitIndex)) - 1;
			currentByte = currentByte & mask;
			
			unsigned int numBitsAdded = 0;
			int shift = numBitsRemaining - 8 + bitIndex;
			if( shift > 0 ) {
				currentByte = currentByte << shift;
				numBitsAdded = 8 - bitIndex;
			}
			else {
				currentByte = currentByte >> -shift;
				numBitsAdded = 8 + std::min(shift, -((int)bitIndex) );
			}

			packet = packet | currentByte;

			numBitsRemaining =  numBitsRemaining - numBitsAdded;
			bitIndex += numBitsAdded;

			if( bitIndex >= 8 ) {
				bitIndex = 0;
				sourceIndex++;
			}
			
		}
		
		return packet;
	}

	bool SerialPacketizer::HasBits() const {
		return sourceIndex < sourceLength;
	}

	SerialDepacketizer::SerialDepacketizer() :
		bufferIndex( 0 ),
		bitIndex( 0 ) {

		buffer.push_back( 0 );
	}

	void SerialDepacketizer::AddPacket( unsigned int packet, unsigned int packetSize ) {

		unsigned int remaining = packetSize;

		std::cout << "Adding packet " << packet << " of size " << packetSize << std::endl;
		
		while( remaining > 0 ) {

			// Number of bits we are adding into the buffer this round
			unsigned int numToContribute = 8 - bitIndex;
			if( numToContribute > remaining ) {
				numToContribute = remaining;
			}
	
			std::cout << "Contributing " << numToContribute << " bits." << std::endl;
				
			// The mask to get the added bits from the packet
			unsigned int mask = (1 << numToContribute) - 1;

			// Getting the bits we're going to add and sliding them to the right
			unsigned int contrib = (packet >> (remaining - numToContribute)) & mask;

			std::cout << "Contribution: " << contrib << std::endl;

			buffer[ bufferIndex ] = buffer[ bufferIndex ] | contrib;
			bitIndex += numToContribute;
			remaining = remaining - numToContribute;
			
			if( bitIndex >= 8 ) {
				bufferIndex++;
				buffer.push_back( 0 );
				bitIndex = 0;
			}

		}
	}

	std::vector<char> SerialDepacketizer::Finalize() {
		if( bitIndex == 0 ) {
			buffer.pop_back();
		}
		return buffer;
	}

	void SerialDepacketizer::Clear() {
		buffer.clear();
	}
	
}