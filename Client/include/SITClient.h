#pragma once
#include "NetClient.h"

namespace SITNet {
	class SITClient : public ClientApp<CMD>
	{
	public:
		SITClient(){
	
		}
		~SITClient() {
		
		};


		void PingServer()
		{

			SITNet::Packet <SITNet::CMD> pkt;
			pkt.header.cmdID = SITNet::CMD::Ping;
			std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
			pkt << timeNow;
			//SendToServer(pkt);
		}

	};
}