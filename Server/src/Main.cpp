#include <iostream>
#include "NetCommon.h"
#include "NetQueue.h"
#include "NetPacket.h"
#include "NetTCPConnection.h"
#include "Server.h"
#include "NetServerApp.h"
#include "World.h"

using namespace NetLib;

int main() {
	uint32_t port = 2222;
	
	Server server(port);
	server.Start();

	while (1)
	{
		server.Process(-1, true);
	}

	return 0;

}