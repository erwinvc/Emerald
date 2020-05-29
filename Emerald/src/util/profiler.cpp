#include "stdafx.h"

void ProfilerQuery::End() {
	if (m_active) {
		m_profiler->EndProfilerQuery(m_dataType, m_timer.Get(Timer::MILLISECONDS));
	}
}

void GLProfilerQueryHandle::End() {
	if (m_active) {
		GetProfiler()->EndGLProfilerQuery(m_startHandle);
	}
}