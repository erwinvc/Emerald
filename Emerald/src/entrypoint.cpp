#include "stdafx.h"


int main() {
	srand((uint)time(NULL));
	Logger::Initialize();
	GetApplication()->Initialize();
	GetApplication()->Cleanup();
	Logger::Cleanup();
}