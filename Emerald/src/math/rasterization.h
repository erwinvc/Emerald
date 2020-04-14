#pragma once

class Rasterization {
private:
	struct XMinMax {
		int minX = Math::MAX_INT;
		int maxX = Math::MIN_INT;
	};

public:
	struct Scan {
		map<int, XMinMax> yValues;
		int minY = Math::MAX_INT;
		int maxY = Math::MIN_INT;
	};

	static void PlotLineHigh(Scan& scan, float x0, float y0, float x1, float y1) {
		float dx = x1 - x0;
		float dy = y1 - y0;
		float xi = 1;
		if (dx < 0) {
			xi = -1;
			dx = -dx;
		}
		float D = 2 * dx - dy;
		float x = x0;

		for (int y = (int)y0; y <= y1; y++) {
			scan.yValues[y].maxX = Math::Max((int)x, scan.yValues[y].maxX);
			scan.yValues[y].minX = Math::Min((int)x, scan.yValues[y].minX);
			scan.minY = Math::Min((int)y, scan.minY);
			scan.maxY = Math::Max((int)y, scan.maxY);
			if (D > 0) {
				x = x + xi;
				D = D - 2 * dy;
			}
			D = D + 2 * dx;
		}
	}

	static void PlotLineLow(Scan& scan, float x0, float y0, float x1, float y1) {
		float dx = x1 - x0;
		float dy = y1 - y0;
		float yi = 1;
		if (dy < 0) {
			yi = -1;
			dy = -dy;
		}
		float D = 2 * dy - dx;
		float y = y0;

		for (int x = (int)x0; x <= x1; x++) {
			scan.yValues[y].maxX = (int)Math::Max((int)x, scan.yValues[y].maxX);
			scan.yValues[y].minX = (int)Math::Min((int)x, scan.yValues[y].minX);
			scan.minY = Math::Min((int)y, scan.minY);
			scan.maxY = Math::Max((int)y, scan.maxY);
			if (D > 0) {
				y = y + yi;
				D = D - 2 * dx;
			}
			D = D + 2 * dy;
		}
	}

public:
	static void PlotLine(Scan& scan, float scale, float x0, float y0, float x1, float y1) {
		x0 = Math::Round((x0) / scale);
		y0 = Math::Round((y0) / scale);
		x1 = Math::Round((x1) / scale);
		y1 = Math::Round((y1) / scale);
		if (Math::Abs(y1 - y0) < Math::Abs(x1 - x0)) {
			if (x0 > x1) PlotLineLow(scan, x1, y1, x0, y0);
			else PlotLineLow(scan, x0, y0, x1, y1);
		} else {
			if (y0 > y1) PlotLineHigh(scan, x1, y1, x0, y0);
			else PlotLineHigh(scan, x0, y0, x1, y1);
		}
	}

	static void PlotCamera(Scan& scan, const CornerRayPositions& cornersObj, Camera* camera, float scale) {
		const glm::vec3* corners = cornersObj.corners;

		glm::vec2 cam(camera->transform.m_position.x, camera->transform.m_position.z);
		glm::vec2 a(corners[0].x, corners[0].z);
		glm::vec2 b(corners[1].x, corners[1].z);
		glm::vec2 c(corners[2].x, corners[2].z);
		glm::vec2 d(corners[3].x, corners[3].z);

		if (glm::distance(cam, a) > 1000)
			a = cam + glm::normalize(a - cam) * 1000.0f;
		if (glm::distance(cam, b) > 1000)
			b = cam + glm::normalize(b - cam) * 1000.0f;
		if (glm::distance(cam, c) > 1000)
			c = cam + glm::normalize(c - cam) * 1000.0f;
		if (glm::distance(cam, d) > 1000)
			d = cam + glm::normalize(d - cam) * 1000.0f;

		Rasterization::PlotLine(scan, scale, a.x, a.y, b.x, b.y);
		Rasterization::PlotLine(scan, scale, b.x, b.y, c.x, c.y);
		Rasterization::PlotLine(scan, scale, c.x, c.y, d.x, d.y);
		Rasterization::PlotLine(scan, scale, d.x, d.y, a.x, a.y);
	}
};