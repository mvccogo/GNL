#include "SITServer.h"
#include "World.h"
#include "Character.h"


using namespace SITNet;
using namespace std;
//using namespace SITGame;


SITServer::SITServer(uint16_t port) : ServerApp(port) {
	m_World = make_shared<World>();
}

SITServer::~SITServer() {


}
void SITServer::OnPacket(std::shared_ptr<Connection<CMD>>& client, Packet<CMD>& pkt) {
	switch (pkt.header.cmdID)
	{
	case CMD::Ping:
	{
		std::cout << "[" << client->GetID() << "]: Server Ping\n";
		client->SendPacket(pkt);
	}
	break;

	case CMD::MessageAll:
	{
		std::cout << "[" << client->GetID() << "]: Message All\n";

		Packet<CMD> pkt;
		pkt.header.cmdID = CMD::ServerMessage;
		pkt << client->GetID();
		SendToAll(pkt, client);

	}
	break;
	case CMD::SelfEnterWorld:
	{

		std::cout << "[" << client->GetID() << "]: Entered world\n";
		auto cha = std::make_shared<Character>();
		cha->SetID(client->GetID());
		cha->SetConnection(client);
		cha->SetWorld(m_World);
		m_World->AddChaToWorld(cha->GetID(), *cha);

		Packet<CMD> pkt;
		pkt.header.cmdID = CMD::SelfEnterWorld;

		for (auto i = m_World->m_chaMap.begin(); i != m_World->m_chaMap.end(); i++) {
			pkt << i->second.GetID();
			pkt << i->second.GetPosX();
			pkt << i->second.GetPosY();
		}
		pkt << static_cast<uint32_t>(m_World->m_chaMap.size());
		pkt << cha->GetID();
		client->SendPacket(pkt);

		pkt.header.cmdID = CMD::PlyEnterWorld;

		for (auto i = m_World->m_chaMap.begin(); i != m_World->m_chaMap.end(); i++) {
			if (cha->GetID() == i->second.GetID()) {
				cout << "Not sending to the same char..." << endl;
			}
			pkt << cha->GetID();
			pkt << cha->GetPosX();
			pkt << cha->GetPosY();
			i->second.GetConnection()->SendPacket(pkt);
		}

	}
	break;
	case CMD::MovePlayer:
	{
		auto cha = m_World->m_chaMap.find(client->GetID());
		if (cha != m_World->m_chaMap.end()) {
			// Player ID found (has entered map)
			double_t x, y;
			uint16_t id;
			pkt >> y;
			pkt >> x;
			pkt >> id;

			if (id != cha->second.GetID()) {
				cout << "Error: ID mismatch! Client: " << id << " Server: " << cha->second.GetID() << endl;
			}
			cha->second.SetPos(x, y);
		}
	}
	}
}