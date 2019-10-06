#include "stdafx.h"

void TileGrid::DrawGeometry() {
	m_tileShader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
	m_tileShader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
	CornerRayPositions& corners = Camera::GetCornerRays();
	m_renderer->Begin();
	loop(x, 32) {
		loop(y, 32) {
			m_renderer->Submit(TileBufferData(x, y, 0, 0, 0));
		}
	}
	//m_renderer->Submit(TileBufferData(0, 0, 0, 0, 0));
	m_renderer->End();
	GetTileMaterialManager()->Bind();
	m_renderer->Draw();
}