#ifndef _SERIAL_PACKETIZER_H_
#define _SERIAL_PACKETIZER_H_

#include <vector>

namespace fidstr {

	class SerialPacketizer {
	public:

		/*! \brief Construct a packetizer with given byte array and number of bits. */
		SerialPacketizer( const char* _source, unsigned int _sourceLength );

		/*! \brief Returns the next packetSize bits from the source.
		 * If there are fewer than requested bits remaining, pads with zeros. */
		unsigned int GetPacket( unsigned int packetSize );

		bool HasBits() const;

	private:

		const char* source;
		char currentByte;
		unsigned int sourceLength; // Lenght in bits
		unsigned int sourceIndex;
		unsigned int bitIndex;
	};

	// TODO Worry about endianness!!
	class SerialDepacketizer {
	public:

		SerialDepacketizer();

		void AddPacket( unsigned int packet, unsigned int packetSize );
		std::vector<char> Finalize();
		
		void Clear();

	private:

		std::vector<char> buffer;
		unsigned int bufferIndex;
		unsigned int bitIndex;
	};
}

#endif