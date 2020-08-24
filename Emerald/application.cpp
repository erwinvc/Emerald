#include "stdafx.h"

void Application::Initialize(bool isServer) {
	m_isServer = isServer;

	Network::Initialize();
	Logger::Initialize(GetTitle());

	if (m_isServer) {
		GetServer()->Initialize();
	} else {
		GetClient()->Initialize();
	}

	Cleanup();
}

void Application::Cleanup() {
	if (m_isServer) GetServer()->Cleanup();
	else GetClient()->Cleanup();

	Network::Cleanup();
	Logger::Cleanup();
}