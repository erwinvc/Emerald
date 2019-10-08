#include "stdafx.h"

void TileGrid::DrawGeometry() {
	CornerRayPositions& corners = Camera::GetCornerRays();
	m_renderer->Begin();
	loop(x, 1) {
		loop(y, 1) {
			m_renderer->Submit(TileBufferData(x, y, 0, 0, 0));
		}
	}
	m_renderer->End();
	m_tileShader->Bind();
	m_tileShader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
	m_tileShader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
	m_tileShader->Set("_Albedo", 0);
	m_tileShader->Set("_Normal", 1);
	m_tileShader->Set("_Roughness", 2);
	m_tileShader->Set("_Metallic", 3);
	m_tileShader->Set("_Emission", 4);
	GetTileMaterialManager()->Bind();
	m_renderer->Draw();
}