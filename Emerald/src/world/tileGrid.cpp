#include "stdafx.h"

struct LineScan {
	map<int, int> points;

	int minY;
	int maxY;
};

struct XMinMax {
	int minX = Math::MAX_INT;
	int maxX = Math::MIN_INT;
};
struct GlobalScan {
	map<int, XMinMax> values;
	int minY = Math::MAX_INT;
	int maxY = Math::MIN_INT;
};

void PlotLineHigh(GlobalScan& scan, float x0, float y0, float x1, float y1) {
	float dx = x1 - x0;
	float dy = y1 - y0;
	float xi = 1;
	if (dx < 0) {
		xi = -1;
		dx = -dx;
	}
	float D = 2 * dx - dy;
	float x = x0;

	for (float y = y0; y <= y1; y++) {
		scan.values[y].maxX = Math::Max((int)x, scan.values[y].maxX);
		scan.values[y].minX = Math::Min((int)x, scan.values[y].minX);
		scan.minY = Math::Min((int)y, scan.minY);
		scan.maxY = Math::Max((int)y, scan.maxY);
		if (D > 0) {
			x = x + xi;
			D = D - 2 * dy;
		}
		D = D + 2 * dx;
	}
}

void PlotLineLow(GlobalScan& scan, float x0, float y0, float x1, float y1) {
	float dx = x1 - x0;
	float dy = y1 - y0;
	float yi = 1;
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	}
	float D = 2 * dy - dx;
	float y = y0;

	for (float x = x0; x <= x1; x++) {
		scan.values[y].maxX = Math::Max((int)x, scan.values[y].maxX);
		scan.values[y].minX = Math::Min((int)x, scan.values[y].minX);
		scan.minY = Math::Min((int)y, scan.minY);
		scan.maxY = Math::Max((int)y, scan.maxY);
		if (D > 0) {
			y = y + yi;
			D = D - 2 * dx;
		}
		D = D + 2 * dy;
	}
}

void DrawLine(GlobalScan& scan, float x0, float y0, float x1, float y1) {
	x0 = Math::Round((x0) / 10);
	y0 = Math::Round((y0) / 10);
	x1 = Math::Round((x1) / 10);
	y1 = Math::Round((y1) / 10);
	if (Math::Abs(y1 - y0) < Math::Abs(x1 - x0)) {
		if (x0 > x1) PlotLineLow(scan, x1, y1, x0, y0);
		else PlotLineLow(scan, x0, y0, x1, y1);
	} else {
		if (y0 > y1) PlotLineHigh(scan, x1, y1, x0, y0);
		else PlotLineHigh(scan, x0, y0, x1, y1);
	}
}

LineScan scans[4];
void TileGrid::DrawGeometry() {
	CornerRayPositions& cornersObj = Camera::GetCornerRays();
	Vector3* corners = cornersObj.corners;

	Vector3 cameraPositionGround = GetCamera()->m_position;
	cameraPositionGround.y = 0;
	//if (corners[0].Distance(cameraPositionGround) > 1000) {
	//	Vector3 forward = cameraPositionGround - corners[0];
	//	corners[0] = forward.Normalized() * 1000;
	//}
	//
	//if (corners[1].Distance(cameraPositionGround) > 1000) {
	//	LOG("a");
	//	Vector3 forward = cameraPositionGround - corners[1];
	//	corners[1] = forward.Normalized() * 1000;
	//}
	GlobalScan scan;
	DrawLine(scan, corners[0].x, corners[0].z, corners[1].x, corners[1].z);
	DrawLine(scan, corners[1].x, corners[1].z, corners[2].x, corners[2].z);
	DrawLine(scan, corners[2].x, corners[2].z, corners[3].x, corners[3].z);
	DrawLine(scan, corners[3].x, corners[3].z, corners[0].x, corners[0].z);

	m_renderer->Begin();
	for (int y = scan.minY; y <= scan.maxY; y++) {
		for (int x = scan.values[y].minX; x <= scan.values[y].maxX; x++) {
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