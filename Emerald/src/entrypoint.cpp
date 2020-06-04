#include "stdafx.h"

int main(int argc, char** argv) {
	CrashHandler::Initialize();
	bool isServer = false;
	for (int i = 0; i < argc; ++i)
		if (strcmp(argv[i], "server") == 0) isServer = true;

	srand((uint)time(NULL));
	Random::Initialize();
	GetApp()->Initialize(isServer);
}