#include <iostream>
#include "NetCommon.h"
#include "NetQueue.h"
#include "NetPacket.h"
#include "NetConnection.h"
#include "SITServer.h"
#include "NetServerApp.h"
#include "World.h"

using namespace SITNet;

int main() {
	uint32_t port = 9313;
	
	SITServer server(port);
	server.Start();

	while (1)
	{
		server.Process(-1, true);
	}

	return 0;

}