#pragma once
#ifndef GDNATIVE_H
#define GDNATIVE_H
#endif

#include <core/Godot.hpp>
#include <gen/Node.hpp>
#include <core/Ref.hpp>
#include "SITClient.h"
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
		void Connect();

		void SendPacket();
		void WriteCmd(uint16_t cmdID);
		bool PreparePacket();
		void PrepareNextPkt();

		void WriteShort(uint16_t data);
		void WriteLong(uint32_t data);
		void WriteLongLong(uint64_t data);
		void WriteString(String data);
		void WriteDouble(double_t data);

		uint16_t	ReadShort();
		uint32_t	ReadLong();
		uint64_t	ReadLongLong();
		String		ReadString();
		double_t    ReadDouble();
	private:
		SITNet::SITClient					m_sitclient;
		SITNet::Packet<SITNet::CMD>			m_pktIn;
		SITNet::Packet<SITNet::CMD>			m_pktOut;
		
	};
}