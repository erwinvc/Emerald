#include "stdafx.h"

void TileGrid::DrawGeometry() {
	Rasterization::Scan scan;
	Rasterization::PlotCamera(scan, Camera::GetCornerRays(0), GetCamera(), 10.0f);

	m_renderer->Begin();
	for (int y = scan.minY; y <= scan.maxY; y++) {
		for (int x = scan.yValues[y].minX; x <= scan.yValues[y].maxX; x++) {
			m_renderer->Submit(TileBufferData((float)x, (float)y, 0, 0, 0));
		}
	}

	m_renderer->End();
	m_tileShader->Bind();
	m_tileShader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
	m_tileShader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
	GetTileMaterialManager()->Bind();
	m_renderer->Draw();
}