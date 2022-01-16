#pragma once
#include <string>
#include "SITServer.h"

class World;

class Character {

public:
	Character();
	~Character();


	void SetPos(double_t x, double_t y);
	void SetWorld(std::shared_ptr<World> world)			{ m_World = world; }
	double_t GetPosX()									{ return pos[0]; }
	double_t GetPosY() { return pos[1]; }



	void SetID(uint16_t id) { m_ID = id; }
	


	uint16_t GetID() { return m_ID; }
	void SetConnection(std::shared_ptr<SITNet::Connection<SITNet::CMD>>& ptr) { m_Connection = ptr; }
	std::shared_ptr<SITNet::Connection<SITNet::CMD>> GetConnection() { return m_Connection; }

		

private:
	std::shared_ptr<SITNet::Connection<SITNet::CMD>>			m_Connection;
	std::string				m_szName;
	uint16_t				m_ID;
	uint16_t				m_curMapID;
	double_t				pos[2];
	std::shared_ptr<World>  m_World;
};
