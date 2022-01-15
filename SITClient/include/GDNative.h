#pragma once
#ifndef GDNATIVE_H
#define GDNATIVE_H
#endif


#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include "SITClient.h"

using namespace godot;


class NetNode : public Node {
	GDCLASS(NetNode, Node);

	protected:

		static void _bind_methods();

	public:
	
		NetNode();
		~NetNode();

		void _init();
		void _process(float delta);
		void Connect();

		void SendPacket();
		void WriteCmd(GDNativeInt cmdID);
		bool PreparePacket();
		void PrepareNextPkt();

		void WriteShort(GDNativeInt data);
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