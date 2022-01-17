#pragma once
#include <unordered_map>
#include "Character.h"


class Character;

class World {

	public:

	World() {
		m_chaMap.clear();
	}
	~World() {

	}


	void AddChaToWorld(uint32_t chaID, Character& cha);
	void RemoveChaFromWorld(uint32_t chaID);
	void FlagCharUpdate(Character& cha);
	void FlagMoveUpdate(Character& cha);

	
	std::unordered_map<uint32_t, Character>			m_chaMap;
	std::recursive_mutex							m_mux;
};

	

