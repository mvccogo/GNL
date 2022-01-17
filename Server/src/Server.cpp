#include "Server.h"
#include "World.h"
#include "Character.h"


using namespace NetLib;
using namespace std;
//using namespace SITGame;


Server::Server(uint16_t port) : ServerApp(port) {
	m_World = make_shared<World>();
}

Server::~Server() {


}
void Server::OnPacket(std::shared_ptr<Connection<CMD>>& client, Packet<CMD>& pkt) {
	switch (pkt.header.cmdID)
	{
	case CMD::Ping:
	{
		std::cout << "[" << client->GetID() << "]: Server Ping\n";
		SendToClient(client, pkt);
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
		std::vector<shared_ptr<NetLib::Connection<NetLib::CMD>>> notifylist;
		for (auto i = m_World->m_chaMap.begin(); i != m_World->m_chaMap.end(); i++) {
			pkt << i->second.GetID();
			pkt << i->second.GetPosX();
			pkt << i->second.GetPosY();
			notifylist.push_back(i->second.GetConnection());
			// Will this always work?
		}

		pkt << static_cast<uint32_t>(m_World->m_chaMap.size());
		pkt << cha->GetID();
		
		Packet<CMD> pkt2; // To be sent for the other players
		pkt2.header.cmdID = CMD::PlyEnterWorld;
		pkt2 << cha->GetID();
		pkt2 << cha->GetPosX();
		pkt2 << cha->GetPosY();
		for (auto i = notifylist.begin(); i != notifylist.end(); i++) {
			SendToClient(*i, pkt2);
		}

		SendToClient(client, pkt);
		/*
		for (auto i = m_World->m_chaMap.begin(); i != m_World->m_chaMap.end(); i++) {
			pkt.header.cmdID = CMD::PlyEnterWorld;
			pkt.body.clear();

			if (cha->GetID() == i->second.GetID()) {
				cout << "Not sending to the same char..." << endl;
				continue;
			}
			pkt << cha->GetID();
			pkt << cha->GetPosX();
			pkt << cha->GetPosY();
			auto client = i->second.GetConnection();
			SendToClient(client, pkt);
		}*/
	
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


void Server::OnClientDisconnect(std::shared_ptr<Connection<CMD>>& client) {
		std::cout << "Removing client [" << client->GetID() << "]\n";
		m_World->RemoveChaFromWorld(client->GetID());
}