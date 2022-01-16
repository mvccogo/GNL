#pragma once
#include "NetCommon.h"
#include "NetQueue.h"
#include "NetPacket.h"
#include "NetConnection.h"
#include "NetServerApp.h"

class World;
class Character;

namespace SITNet {
	class SITServer : public ServerApp<CMD> {
	public:
		SITServer(uint16_t port);
		~SITServer();

	protected:
		virtual bool OnClientConnect(std::shared_ptr<Connection<CMD>>& client) override {
			Packet<CMD> pkt;
			pkt.header.cmdID = CMD::AcceptConnection;
			client->SendPacket(pkt);
			return true;
		}
		virtual void OnClientDisconnect(std::shared_ptr<Connection<CMD>>& client) override {
			std::cout << "Removing client [" << client->GetID() << "]\n";
		}
		virtual void OnPacket(std::shared_ptr<Connection<CMD>>& client, Packet<CMD>& pkt) override;
		

	private:
		std::shared_ptr<World>        m_World;
		

	};
}