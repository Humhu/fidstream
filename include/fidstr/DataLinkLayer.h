#ifndef _DATALINK_LAYER_H_
#define _DATALINK_LAYER_H_

#include "fidstr/FiducialStreamTypes.h"
#include "fidstr/Fiducial.h"

#include "fidstr/PhysicalLayer.h"
#include "fidstr/SerialPacketizer.h"

namespace fidstr {

	/*! \brief The data link layer (MAC and LLC) that controls frame sequences. */

	// TODO The start and stop fiducials should be common somehow!

	typedef std::vector<char> Packet;
	typedef std::vector<Fiducial::Ptr> Frame;
	typedef std::vector<char> Payload;
	
	class FiducialPacketTransmitter {
	public:

		const unsigned int frameSize; // payload size in bytes
		
		FiducialPacketTransmitter( FiducialFamily& _family, FiducialTransmitter& _phy,
								   unsigned int _frameSize );

		/*! \brief Converts a packet into frames and transmits them. Blocks until
		 * the last transmission begins. */
		void TransmitPacket( const Packet& packet );
		
	private:
		
		FiducialFamily& family;
		FiducialTransmitter& phy;

		void TransmitFrame( const Frame& frame );

		std::vector<Frame> FramePacket( const Packet& packet );
		
		/*! \brief Divides the packet into data chunks, each of which is at most
		 * frameSize bytes. */
		std::vector<Payload> DividePacket( const Packet& packet );

		/*! \brief Adds header and meta-data for a data to turn it into a frame. */
		Frame FramePayload( const Payload& data );

		Fiducial::Ptr GetStartFiducial() const;
		Fiducial::Ptr GetStopFiducial() const;
		
	};

	class FiducialPacketReceiver {
	public:

		FiducialPacketReceiver( FiducialFamily& _family, FiducialReceiver& _phy );

		/*! \brief Process all pending physical layer detections. */
		void ProcessInputs();

		/*! \brief Returns the earliest received payload from the queue.
		 * If there are none, returns an empty vector. */
		Payload DequeuePayload();
		
	private:

		mutable Mutex outputMutex;
		std::deque<Payload> outputBuffer;

		Frame currentFrame;
		
		FiducialFamily& family;
		FiducialReceiver& phy;

		bool IsFrameStart( unsigned int id );
		bool IsFrameEnd( unsigned int id );

		bool FrameIsValid( const Frame& frame );
		Payload GetFramePayload( const Frame& frame );
		
	};
	
}

#endif