#pragma once
#include "NetClient.h"

namespace NetLib {
	class Client : public ClientApp<CMD>
	{
	public:
		Client(){
	
		}
		~Client() {
		
		};


		void PingServer()
		{

			NetLib::Packet <NetLib::CMD> pkt;
			pkt.header.cmdID = NetLib::CMD::Ping;
			std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
			pkt << timeNow;
			SendToServer(pkt);
		}

	};
}