#include "GDNative.h"
#include "Client.h"


const uint16_t MAX_LEN = 1024*5;
using namespace godot;
using namespace NetLib;

void NetNode::_register_methods() {
	register_method("_process", &NetNode::_process);
	register_method("ConnectToServer", &NetNode::Connect);
	register_method("DisconnectFromServer", &NetNode::Disconnect);
	register_method("SendPacket", &NetNode::SendPacket);
	register_method("PreparePacket", &NetNode::PreparePacket);
	register_method("PrepareNextPkt", &NetNode::PrepareNextPkt);
	register_method("WriteCmd", &NetNode::WriteCmd);
	register_method("WriteShort", &NetNode::WriteShort);
	register_method("WriteLong", &NetNode::WriteLong);
	register_method("WriteLongLong", &NetNode::WriteLongLong);
	register_method("WriteString", &NetNode::WriteString);
	register_method("WriteDouble", &NetNode::WriteDouble);
	register_method("PingServer", &NetNode::PingServer);
	register_method("ReadShort", &NetNode::ReadShort);
	register_method("ReadLong", &NetNode::ReadLong);
	register_method("ReadLongLong", &NetNode::ReadLongLong);
	register_method("ReadString", &NetNode::ReadString);
	register_method("ReadDouble", &NetNode::ReadDouble);

	register_signal<NetNode>((char*)"packet_received", "packet_cmd_id", GODOT_VARIANT_TYPE_INT);
}


NetNode::NetNode() {
	// Initialize your NativeScript classes in their `_init()` method, not their constructor.The constructor can't access the base class's methods.
}

NetNode::~NetNode() {
	if(m_Client.IsConnected())
	m_Client.Disconnect();
}

void NetNode::_init() {
	m_pktIn.header.cmdID = NetLib::CMD::Invalid;



}

void NetNode::_process(float delta) {

	if (m_Client.IsConnected()) {
		if (!m_Client.GetIncomingPktQue().is_empty()) {

			m_pktIn = m_Client.GetIncomingPktQue().get_front().pkt;
			emit_signal("packet_received", static_cast<int> (m_pktIn.header.cmdID));
		
		}
	}
	else {
		//godot::Godot::print("Disconnected to server");
	}

}

void NetNode::Connect(String host, uint16_t port) {
	m_Client.Connect(host.alloc_c_string(), port);
	//m_Client.Connect("127.0.0.1", 2222);
}

void NetNode::Disconnect() {
	m_Client.Disconnect();
}


bool NetNode::PreparePacket() {
	if (!m_Client.IsConnected()) {
		return false;
	}
	m_pktOut.body.clear();
	return true;
}

void NetNode::WriteCmd(uint16_t cmdid) {
	m_pktOut.header.cmdID = static_cast<CMD>(cmdid);
}

void NetNode::SendPacket() {
	m_Client.SendToServer(m_pktOut);
	return;
}

void NetNode::WriteShort(uint16_t data) {
	 m_pktOut << data; 
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
	memcpy(m_pktOut.body.data() + oldBody + 1, data.alloc_c_string(), data.length());
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
	char data[MAX_LEN]{0};
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
	m_Client.GetIncomingPktQue().pop_front();
	m_pktIn.body.clear();
	m_pktIn.header.cmdID = NetLib::CMD::Invalid;

}
