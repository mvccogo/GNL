#include "World.h"
#include "Character.h"

using namespace SITNet;
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

void World::AddChaToWorld(uint16_t chaID, Character& cha) {
	m_chaMap.insert(std::make_pair(chaID, cha));

}

void World::FlagMoveUpdate(Character& cha) {
	Packet<CMD> pkt;
	pkt.header.cmdID = CMD::MovePlayer;
	pkt << cha.GetID();
	pkt << cha.GetPosX();
	pkt << cha.GetPosY();
	for (auto i = m_chaMap.begin(); i != m_chaMap.end(); i++) {
		if (i->second.GetID() == cha.GetID()) continue;
		i->second.GetConnection()->SendPacket(pkt);
	}
}