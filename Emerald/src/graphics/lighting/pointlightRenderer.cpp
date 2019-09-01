#include "stdafx.h"

void PointlightRenderer::Begin() {
	m_renderer->Begin();
}

void PointlightRenderer::End() {
	m_renderer->End();
}
void PointlightRenderer::Draw() {
	m_renderer->Draw();
}

void PointlightRenderer::Submit(Pointlight& pointlight) {
	m_renderer->Submit(pointlight);
}