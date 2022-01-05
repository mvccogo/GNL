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


	void AddChaToWorld(uint16_t chaID, Character& cha);
	void FlagCharUpdate(Character& cha);
	void FlagMoveUpdate(Character& cha);

	
	std::unordered_map<uint16_t, Character>			m_chaMap;
};

	

