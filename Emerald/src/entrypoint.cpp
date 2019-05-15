#include "stdafx.h"

int main() {
    srand((uint)time(NULL));
    Logger::Initialize();
	GetApplication()->Run();
	GetApplication()->Cleanup();
    Logger::Cleanup();
}