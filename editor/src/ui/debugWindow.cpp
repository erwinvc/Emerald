#include "eepch.h"
#include "core/application/application.h"
#include "debugWindow.h"
#include "engine/input/keyboard.h"
#include "Implot/implot.h"
#include "metrics/metrics.h"
#include "ui/imguiManager.h"

namespace emerald {
	static const uint32_t maxRenderSamples = 1000;
	static const float overlayUpdateInterval = 0.1f;

	struct ScrollingBuffer {
		int m_maxSize;
		int m_offset;
		ImVector<ImVec2> m_data;
		ScrollingBuffer(int max_size = maxRenderSamples) {
			m_maxSize = max_size;
			m_offset = 0;
			m_data.reserve(m_maxSize);
		}
		void AddPoint(float x, float y) {
			if (m_data.size() < m_maxSize)
				m_data.push_back(ImVec2(x, y));
			else {
				m_data[m_offset] = ImVec2(x, y);
				m_offset = (m_offset + 1) % m_maxSize;
			}
		}
		void Erase() {
			if (m_data.size() > 0) {
				m_data.shrink(0);
				m_offset = 0;
			}
		}
	};

	struct MetricData {
		float m_samples[maxRenderSamples] = {};
		uint32_t m_frame = 0;
		uint32_t m_index = 0;
		float m_previousSample = 0.0f;
		float m_maxSample = 0.0f;
		std::string m_overlayText;
		float m_timeSinceLastUpdate = 0.0f;
		ScrollingBuffer m_scrollingBuffer;

		MetricData() {
			m_scrollingBuffer = ScrollingBuffer(maxRenderSamples);
			m_scrollingBuffer.AddPoint(0, 0);
		}
	};

	static std::unordered_map<Metric, MetricData> s_metricDataMap;

	float updateSamples(Metric metric) {
		auto& data = s_metricDataMap[metric];

		//Check if we need to update the sample
		uint32_t frame = Metrics::getFrame(metric);
		if (data.m_frame == frame) return data.m_previousSample;
		data.m_frame = frame;

		float newSample = Metrics::getElapsedTime(metric);

		data.m_scrollingBuffer.AddPoint((float)frame, newSample);

		data.m_samples[data.m_index] = newSample;
		data.m_index = (data.m_index + 1) % maxRenderSamples;

		data.m_maxSample = *std::max_element(data.m_samples, data.m_samples + maxRenderSamples);
		data.m_maxSample *= 1.2f; //Slight offset so the text stays visible
		data.m_previousSample = newSample;
		return data.m_previousSample;
	}

	void updateOverlayText(Metric metric, const char* label, float value) {
		auto& data = s_metricDataMap[metric];

		// Update the overlay text if the time since the last update exceeds the interval
		data.m_timeSinceLastUpdate += ImGui::GetIO().DeltaTime;
		if (data.m_timeSinceLastUpdate >= overlayUpdateInterval) {
			data.m_overlayText = std::format("{} {:.3} ms", label, value);
			data.m_timeSinceLastUpdate = 0.0f;
		}
	}

	void drawMetricHistogram(Metric metric, const char* label, uint32_t amountToRender) {
		float value = updateSamples(metric);
		updateOverlayText(metric, label, value);
		ImGui::Text("%s", s_metricDataMap[metric].m_overlayText.c_str());
		ImGui::PlotHistogram("", s_metricDataMap[metric].m_samples, maxRenderSamples, s_metricDataMap[metric].m_index, nullptr, 0.0f, s_metricDataMap[metric].m_maxSample, ImVec2(-1, 80.0f));
	}

	void plotLine(const char* label, Metric metric) {
		auto& data = s_metricDataMap[metric];
		ImPlot::PlotLine(label, &data.m_scrollingBuffer.m_data[0].x, &data.m_scrollingBuffer.m_data[0].y, data.m_scrollingBuffer.m_data.size(), 0, data.m_scrollingBuffer.m_offset, 2 * sizeof(float));
	}
	void DebugWindow::draw() {
		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoNav)) {
			ImGui::DrawGradientBackgroundForWindow(ImGui::GradientDirection::TOP);

			// FPS and frame time display
			ImGui::Text("Frames per second:  %1d", App->getFPS());
			ImGui::Text("Updates per second: %1d", App->getUPS());

			uint32_t amountToRender = std::min((uint32_t)1000, (uint32_t)ImGui::GetWindowWidth());

			// Draw histograms for each metric
			drawMetricHistogram(Metric::RENDER, "Render", amountToRender);
			drawMetricHistogram(Metric::RENDERWAIT, "Render Wait", amountToRender);
			drawMetricHistogram(Metric::LOGIC, "Logic", amountToRender);
			drawMetricHistogram(Metric::LOGICWAIT, "Logic Wait", amountToRender);
			drawMetricHistogram(Metric::GPU, "GPU", amountToRender);

			static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;
			if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, 300), ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMenus)) {
				ImPlot::SetupAxes(nullptr, "ms", flags);
				//ImPlot::SetupLegend(ImPlotLocation_NorthWest, ImPlotLegendFlags_Horizontal);
				ImPlot::SetupAxisLimits(ImAxis_X1, App->getFrameCount() - (float)maxRenderSamples, (float)App->getFrameCount(), ImGuiCond_Always);
				ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
				ImPlot::SetupAxisLimits(ImAxis_Y1, 0.001f, 50, ImGuiCond_Always);
				ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
				plotLine("Render", Metric::RENDER);
				plotLine("Render wait", Metric::RENDERWAIT);
				plotLine("Logic", Metric::LOGIC);
				plotLine("Logic wait", Metric::LOGICWAIT);
				plotLine("GPU", Metric::GPU);
				ImPlot::EndPlot();
			}
		}
		ImGui::End();
	}
}