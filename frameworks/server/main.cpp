// server.cpp : �������̨Ӧ�ó������ڵ㡣
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

