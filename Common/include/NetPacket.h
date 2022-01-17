#pragma once
#include "NetCommon.h"


namespace NetLib {

	enum class CMD : uint16_t {
		Invalid,
		AcceptTCPConnection,
		SelfEnterWorld,
		PlyEnterWorld,
		ServerMessage,
		MovePlayer,
		Ping,
		MessageAll
	};
	template <typename T>
	struct PacketHeader
	{
		T cmdID;  
		uint32_t size = 0;

	};


	template <typename T>
	class Packet {
	public:
		Packet() {};
		~Packet() {};

		uint32_t get_packet_len() const { return static_cast<uint32_t>(body.size()); }

		template <typename DataType>
		friend Packet<T>& operator << (Packet<T>& pkt, const DataType& data) {

			// Check that the type of the data being pushed is trivially copyable
			static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

			// Cache current size of vector, as this will be the point we insert the data
			size_t i = pkt.body.size();
			// Resize the vector by the size of the data being pushed
			pkt.body.resize(pkt.body.size() + sizeof(DataType));

			// Physically copy the data into the newly allocated vector space
			memcpy(pkt.body.data() + i, &data, sizeof(DataType));

			// Recalculate the message size
			pkt.header.size = pkt.get_packet_len();

			// Return the target message so it can be "chained"
			return pkt;
		}
		
		template<typename DataType>
		friend Packet<T>& operator >> (Packet<T>& pkt, DataType& data) {
			// Check that the type of the data being pushed is trivially copyable
			static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pulled from vector");


			// Cache the location towards the end of the vector where the pulled data starts
			size_t i = pkt.body.size() - sizeof(DataType);

			// Physically copy the data from the vector into the user variable
			memcpy(&data, pkt.body.data() + i, sizeof(DataType));

			// Shrink the vector to remove read bytes, and reset end position
			pkt.body.resize(i);

			// Recalculate the message size
			pkt.header.size = pkt.get_packet_len();

			// Return the target message so it can be "chained"
			return pkt;
		}

		PacketHeader<T> header{};
		std::vector<uint8_t> body;
	
	};


	template <typename T>
	class TCPConnection;

	template <typename T>
	struct OwnedPacket {
		std::shared_ptr<TCPConnection<T>> connectionPtr = nullptr;
		Packet<T> pkt;

	};

}