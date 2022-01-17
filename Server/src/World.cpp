#include "World.h"
#include "Character.h"

using namespace NetLib;
using namespace std;
//
//World::World() {
//
//
//
//}
//World::~World() {
//
//
//}

void World::AddChaToWorld(uint32_t chaID, Character& cha) {
	m_chaMap.insert(std::make_pair(chaID, cha));
}

void World::RemoveChaFromWorld(uint32_t chaID) {
	m_chaMap.erase(chaID);
}

void World::FlagMoveUpdate(Character& cha) {
	Packet<CMD> pkt;
	pkt.header.cmdID = CMD::MovePlayer;
	pkt << cha.GetID();
	pkt << cha.GetPosX();
	pkt << cha.GetPosY();
	
	for (auto i = m_chaMap.begin(); i != m_chaMap.end(); i++) {
		if (i->second.GetID() == cha.GetID()) continue;
		i->second.GetTCPConnection()->SendPacket(pkt);
	}
	
}