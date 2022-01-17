#pragma once
#ifndef GDNATIVE_H
#define GDNATIVE_H
#endif

#include <core/Godot.hpp>
#include <gen/Node.hpp>
#include <core/Ref.hpp>
#include "Client.h"
#include <gen/Object.hpp>


namespace godot {
	class NetNode : public Node {
		GODOT_CLASS(NetNode, Node);
	public:
		static void _register_methods();

		NetNode();
		~NetNode();

		void _init();
		void _process(float delta);
		void Connect(String host, uint16_t port);
		void Disconnect();

		void SendPacket();
		void WriteCmd(uint16_t cmdID);
		bool PreparePacket();
		void PrepareNextPkt();

		void WriteShort(uint16_t data);
		void WriteLong(uint32_t data);
		void WriteLongLong(uint64_t data);
		void WriteString(String data);
		void WriteDouble(double_t data);
		
		void PingServer() {
			NetLib::Packet <NetLib::CMD> pkt;
			pkt.header.cmdID = NetLib::CMD::Ping;
			std::chrono::system_clock::time_point timeNow = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
			std::chrono::duration millisNow = timeNow.time_since_epoch();
			pkt << millisNow.count();
			m_Client.SendToServer(pkt);
		}


		uint16_t	ReadShort();
		uint32_t	ReadLong();
		uint64_t	ReadLongLong();
		String		ReadString();
		double_t    ReadDouble();
	private:
		NetLib::Client					m_Client;
		NetLib::Packet<NetLib::CMD>			m_pktIn;
		NetLib::Packet<NetLib::CMD>			m_pktOut;
		
	};
}