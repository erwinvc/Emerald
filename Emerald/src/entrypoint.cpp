#include "stdafx.h"


int main() {
	srand((uint)time(NULL));
	Logger::Initialize();
	GetApp()->Initialize();
	GetApp()->Cleanup();
	Logger::Cleanup();
}