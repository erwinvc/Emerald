#include "stdafx.h"

void ClientSession::Initialize(ENetPeer* peer, uint32 salt, uint32 playerId) {
	m_clientConnection.m_salt = salt;
	m_salt = salt;
	m_clientConnection.m_peer = peer;
	m_playerId = playerId;
	m_isActive = true;
}

void ClientSession::Disconnect() {
	if (m_isActive) {
		m_clientConnection.Disconnect();
		m_isActive = false;
	}
}

void ClientSession::tick() {
	
}