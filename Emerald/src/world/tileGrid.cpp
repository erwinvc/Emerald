#include "stdafx.h"

void TileGrid::RenderGeometry() {
	Rasterization::Scan scan;
	Rasterization::PlotCamera(scan, Camera::GetCornerRays(0), GetCamera(), 10.0f);

	m_renderer->Begin();
	for (int y = scan.minY; y < scan.maxY; y++) {
		for (int x = scan.yValues[y].minX; x < scan.yValues[y].maxX; x++) {
			if (Math::Within(x, 0, GRIDSIZE - 1) && Math::Within(y, 0, GRIDSIZE - 1)) {
				if (m_tiles[y * GRIDSIZE + x].Exists())
					m_renderer->Submit(TileBufferData((float)x, (float)y, 0, 0, m_tiles[y * GRIDSIZE + x].m_textureID));
			}
			//m_renderer->Submit(TileBufferData((float)x, (float)y, 0, 0, 0));
		}
	}
	m_renderer->End();

	m_tileShader->Bind();
	m_tileShader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
	m_tileShader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
	GetTileMaterialManager()->Bind();
	m_renderer->Draw();
}