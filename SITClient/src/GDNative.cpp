#include "GDNative.h"


#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "SITClient.h"

const uint16_t MAX_LEN = 1024*5;
using namespace godot;
using namespace SITNet;

void NetNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_process"), &NetNode::_process);
	ClassDB::bind_method(D_METHOD("ConnectToServer"), &NetNode::Connect);
	ClassDB::bind_method(D_METHOD("SendPacket"), &NetNode::SendPacket);
	ClassDB::bind_method(D_METHOD("PreparePacket"), &NetNode::PreparePacket);
	ClassDB::bind_method(D_METHOD("PrepareNextPkt"), &NetNode::PrepareNextPkt);
	ClassDB::bind_method(D_METHOD("WriteCmd"), &NetNode::WriteCmd);
	ClassDB::bind_method(D_METHOD("WriteShort"), &NetNode::WriteShort);
	ClassDB::bind_method(D_METHOD("WriteLong"), &NetNode::WriteLong);
	ClassDB::bind_method(D_METHOD("WriteLongLong"), &NetNode::WriteLongLong);
	ClassDB::bind_method(D_METHOD("WriteString"), &NetNode::WriteString);
	ClassDB::bind_method(D_METHOD("WriteDouble"), &NetNode::WriteDouble);
	
	ClassDB::bind_method(D_METHOD("ReadShort"), &NetNode::ReadShort);
	ClassDB::bind_method(D_METHOD("ReadLong"), &NetNode::ReadLong);
	ClassDB::bind_method(D_METHOD("ReadLongLong"), &NetNode::ReadLongLong);
	ClassDB::bind_method(D_METHOD("ReadString"), &NetNode::ReadString);
	ClassDB::bind_method(D_METHOD("ReadDouble"), &NetNode::ReadDouble);

	ADD_SIGNAL(MethodInfo("packet_received", PropertyInfo(Variant::INT, "packet_cmd_id")));
}


NetNode::NetNode() {
	// Initialize your NativeScript classes in their `_init()` method, not their constructor.The constructor can't access the base class's methods.
}

NetNode::~NetNode() {
	if(m_sitclient.IsConnected())
	m_sitclient.Disconnect();
}

void NetNode::_init() {
	m_pktIn.header.cmdID = SITNet::CMD::Invalid;



}

void NetNode::_process(float delta) {

	if (m_sitclient.IsConnected()) {
		if (!m_sitclient.GetIncomingPktQue().is_empty()) {

			m_pktIn = m_sitclient.GetIncomingPktQue().get_front().pkt;
			emit_signal("packet_received", static_cast<int> (m_pktIn.header.cmdID));
		
		}
	}
	else {
		//godot::Godot::print("Disconnected to server");
	}

}

void NetNode::Connect() {
	m_sitclient.Connect("26.192.37.108", 9313);
}


bool NetNode::PreparePacket() {
	if (!m_sitclient.IsConnected()) {
		return false;
	}
	m_pktOut.body.clear();
	return true;
}

void NetNode::WriteCmd(GDNativeInt cmdid) {
	m_pktOut.header.cmdID = static_cast<CMD>(cmdid);
}

void NetNode::SendPacket() {
	m_sitclient.SendToServer(m_pktOut);
	return;
}

void NetNode::WriteShort(GDNativeInt data) {
	 m_pktOut << static_cast<uint16_t>(data); 
}

void NetNode::WriteLong(uint32_t data) {
	m_pktOut << data;
}

void NetNode::WriteLongLong(uint64_t data) {
	m_pktOut << data;
}

void NetNode::WriteString(String data) {
	size_t oldBody = m_pktOut.body.size();
	m_pktOut.body.resize(oldBody + data.length() + 1);
	memset(m_pktOut.body.data() + oldBody, '\0', 1);
	memcpy(m_pktOut.body.data() + oldBody + 1, data.utf8().get_data() , data.length());
	m_pktOut.header.size = m_pktOut.get_packet_len();
}

void NetNode::WriteDouble(double_t data) {
	m_pktOut << data;
}

uint16_t NetNode::ReadShort() {
	uint16_t data = 0;
	m_pktIn >> data;
	return data;
}

uint32_t NetNode::ReadLong() {
	uint32_t data = 0;
	m_pktIn >> data;
	return data;
}

uint64_t NetNode::ReadLongLong() {
	uint64_t data = 0;
	m_pktIn >> data;
	return data;
}

String NetNode::ReadString() {
	char data[1024 * 5]{0};
	size_t oldBody = m_pktIn.body.size();
	size_t strlen = 0;
	for (auto i = m_pktIn.body.rbegin() ; *i; i++) {
		strlen++;
	}
	memcpy(data, m_pktIn.body.data() + oldBody - strlen, strlen);
	m_pktIn.body.resize(oldBody - strlen - 1);
	return String(data);
}

double_t NetNode::ReadDouble() {
	double_t data = 0.0f;
	m_pktIn >> data;
	return data;
}

void NetNode::PrepareNextPkt() {
	m_sitclient.GetIncomingPktQue().pop_front();
	m_pktIn.body.clear();
	m_pktIn.header.cmdID = SITNet::CMD::Invalid;

}
