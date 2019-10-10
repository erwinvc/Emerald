#include "stdafx.h"

void TileGrid::DrawGeometry() {
	CornerRayPositions& cornersObj = Camera::GetCornerRays();
	//Vector3 corners[4] = { cornersObj.BL, cornersObj.BR, cornersObj.TL, cornersObj.TR };
	m_renderer->Begin();
	//
	//int highestCorner = corners[0].y;
	//int lowestCorner = corners[0].y;
	//int leftCorner = corners[0].x;
	//int rightCorner = corners[0].x;
	//for (int i = 1; i < 4; i++) {
	//	if (corners[i].y > corners[highestCorner].y) highestCorner = i;
	//	if (corners[i].y < corners[lowestCorner].y) lowestCorner = i;
	//	if (corners[i].x > corners[leftCorner].x) leftCorner = i;
	//	if (corners[i].x < corners[rightCorner].x) rightCorner = i;
	//}

	//for (int i = cornersObj.BL.x; i < cornersObj.BR.x; i += 10) {
	//	m_renderer->Submit(TileBufferData((int)(i / 10), 0, 0, 0, 0));
	//}

	//loop(x, 32) {
	//	loop(y, 32) {
	//		m_renderer->Submit(TileBufferData(x, y, 0, 0, 0));
	//	}
	//}
	m_renderer->End();
	m_tileShader->Bind();
	m_tileShader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
	m_tileShader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
	GetTileMaterialManager()->Bind();
	m_renderer->Draw();
}