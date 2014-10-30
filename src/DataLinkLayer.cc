#include "fidstr/DataLinkLayer.h"

#include <boost/foreach.hpp>

namespace fidstr {

	FiducialPacketTransmitter::FiducialPacketTransmitter( FiducialFamily& _family,
														  FiducialTransmitter& _phy,
														  unsigned int _frameSize ) :
		family( _family ),
		phy( _phy ),
		frameSize( _frameSize ) {}

	void FiducialPacketTransmitter::TransmitPacket( const Packet& packet ) {

		std::vector<Frame> frames = FramePacket( packet );
		BOOST_FOREACH( const Frame& frame, frames ) {
			TransmitFrame( frame );
		}
		
	}
		
	void FiducialPacketTransmitter::TransmitFrame( const Frame& frame ) {

		BOOST_FOREACH( const Fiducial::Ptr& fid, frame ) {
			phy.Transmit( fid );
		}
	}

	std::vector<Frame> FiducialPacketTransmitter::FramePacket( const Packet& packet ) {

		std::vector<Payload> payloads = DividePacket( packet );

// 		std::vector<Frame> frames( payloads.size() );
		std::vector<Frame> frames;
		BOOST_FOREACH( const Payload& payload, payloads ) {
			Frame frame = FramePayload( payload );
			std::cout << "Got a frame of length " << frame.size () << std::endl;
			frames.push_back( frame );
		}
		return frames;
	}
	
	std::vector<Payload> FiducialPacketTransmitter::DividePacket( const Packet& packet ) {

		std::vector<Payload> chunks;
		Payload data;
		
		unsigned int startInd = 0;
		unsigned int endInd;
		while( startInd < packet.size() ) {
			endInd = startInd + frameSize;
			if( endInd >= packet.size() ) {
				endInd = packet.size();
			}
			
			data = Payload( packet.begin() + startInd, packet.begin() + endInd );
			chunks.push_back( data );

			startInd = endInd + 1;
		}

		return chunks;
	}
	
	Frame FiducialPacketTransmitter::FramePayload( const Payload& data ) {

		SerialPacketizer packetizer( data.data(), data.size() );
		Frame frame;

		// First add frame start fiducial
		frame.push_back( GetStartFiducial() );

		// Then add payload length fiducial
		unsigned int payloadLength = std::ceil( data.size()*8/( (float) family.numDataBits ) );
		frame.push_back( family.GetFiducial( payloadLength ) );

		// Now add data
		unsigned int parity = 0;
		while( packetizer.HasBits() ) {
			unsigned int fid = packetizer.GetPacket( family.numDataBits );
			parity = parity ^ fid;
			frame.push_back( family.GetFiducial( fid ) );
		}

		// Finally add parity fiducial made by XORing data together
		// NOTE Have to scrub off extraneous front bits
		unsigned int mask = (1 << family.numDataBits) - 1;
		parity = parity && mask;
		frame.push_back( family.GetFiducial( parity ) );

		return frame;
	}

	Fiducial::Ptr FiducialPacketTransmitter::GetStartFiducial() const {
		unsigned int id = (1 << family.numDataBits);
		return family.GetFiducial( id );
	}

	Fiducial::Ptr FiducialPacketTransmitter::GetStopFiducial() const {
		unsigned int id = (1 << family.numDataBits) + 1;
		return family.GetFiducial( id );
	}
	
	FiducialPacketReceiver::FiducialPacketReceiver( FiducialFamily& _family,
													FiducialReceiver& _phy ) :
		family( _family ),
		phy( _phy ) {}

	void FiducialPacketReceiver::ProcessInputs() {

		FiducialDetection::Ptr detection;

		while( detection = phy.DequeueDetection() ) {

			Fiducial::Ptr fid = family.GetFiducial( detection->id );
			
			if( IsFrameStart( detection->id ) ) {
				currentFrame.clear();
				currentFrame.push_back( fid );
			}
			else if( IsFrameEnd( detection->id ) ) {

				currentFrame.push_back( fid );
				
				if( FrameIsValid( currentFrame ) ) {
					Payload payload = GetFramePayload( currentFrame );

					WriteLock lock( outputMutex );
					outputBuffer.push_back( payload );
				}
			}
			else {
				currentFrame.push_back( fid );
			}
		}
	}

	Payload FiducialPacketReceiver::DequeuePayload() {
		WriteLock lock( outputMutex );

		Payload payload;
		if( !outputBuffer.empty() ) {
			payload = outputBuffer.front();
			outputBuffer.pop_front();
		}
		return payload;
	}

	bool FiducialPacketReceiver::IsFrameStart( unsigned int id ) {
		unsigned int sid = (1 << family.numDataBits);
		return id == sid;
	}

	bool FiducialPacketReceiver::IsFrameEnd( unsigned int id ) {
		unsigned int eid = (1 << family.numDataBits) + 1;
		return id == eid;
	}

	bool FiducialPacketReceiver::FrameIsValid( const Frame& frame ) {

		// First sanity check start and end fiducials
		if( !IsFrameStart( frame[0]->id ) ) {
			return false;
		}
		if( !IsFrameEnd( frame[frame.size()-1]->id ) ) {
			return false;
		}

		// Now check number of fiducials in frame
		unsigned int payloadLength = frame[1]->id;
		if( payloadLength + 4 != frame.size() ) {
			return false;
		}

		// Finally compute parity
		unsigned int parity = 0;
		for( unsigned int i = 2; i < payloadLength + 2; i++ ) {
			parity = parity ^ frame[i]->id;
		}
		unsigned int mask = (1 << family.numDataBits) - 1;
		parity = parity && mask;
		if( parity != frame[frame.size()-2]->id ) {
			return false;
		}
		return true;
		
	}

	Payload FiducialPacketReceiver::GetFramePayload( const Frame& frame ) {
		Payload payload( frame.begin() + 2, frame.end() - 2 );
		return payload;
	}
	
}