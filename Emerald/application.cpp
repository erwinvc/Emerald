#include "stdafx.h"

void Application::Initialize(bool isServer) {
	m_isServer = isServer;

	ENet::Initialize();
	Logger::Initialize(GetTitle());

	if (m_isServer) {
		LOG("Size: %d", sizeof(temp));
		GetServer()->Initialize();
	} else {
		GetClient()->Initialize();
	}

	Cleanup();
}

void Application::Cleanup() {

	if (m_isServer) GetServer()->Cleanup();
	else GetClient()->Cleanup();

	ENet::Cleanup();
	Logger::Cleanup();
}