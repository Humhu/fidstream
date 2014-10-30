#include "fidstr/SerialPacketizer.h"

#include <stdexcept>

#include <iostream>

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

			unsigned int numBitsAdded = 0;
			int shift = numBitsRemaining - 8;
			if( shift > 0 ) {
				currentByte = currentByte << shift;
				numBitsAdded = 8;
			}
			else {
				currentByte = currentByte >> -shift;
				numBitsAdded = 8 + shift;
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
		
		while( remaining > 0 ) {

			char shiftedPacket = packet << bitIndex;
			char current = buffer[ bufferIndex ] | shiftedPacket;

			unsigned int increment;
			if( remaining < sizeof( char ) ) {
				increment = remaining;
			}
			else {
				increment = sizeof( char ) - bitIndex;
			}
			
			packet = packet >> increment;
			bitIndex += increment;
			remaining = remaining - increment;

			// NOTE bitIndex should never go over 8
			if( bitIndex >= 8 ) {
				bitIndex = 0;
				buffer.push_back( 0 );
				bufferIndex++;
			}
		}
	}

	std::vector<char> SerialDepacketizer::GetData() const {
		return buffer;
	}

	void SerialDepacketizer::Clear() {
		buffer.clear();
	}
	
}