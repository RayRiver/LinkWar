// server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "Server.h"

int _tmain(int argc, _TCHAR* argv[])
{
	auto server = new Server;

	if (server->init(1/60.0))
	{
		server->run();
	}

	return 0;
}

