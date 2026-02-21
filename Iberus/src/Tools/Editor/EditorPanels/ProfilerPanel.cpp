#include "Enginepch.h"
#include "ProfilerPanel.h"
#include "Editor.h"
#include "Profiler.h"

#include "imgui.h"

namespace Iberus {

	ProfilerPanel::ProfilerPanel(Editor& editor)
		: editor(editor) {
	}

	static ImU32 HeatColor(float t) {
		if (t <= 0.0f) {
			return IM_COL32(80, 80, 128, 255);
		}
		if (t >= 1.0f) {
			return IM_COL32(255, 64, 64, 255);
		}
		float r, g, b;
		if (t < 0.5f) {
			float x = t * 2.0f;
			r = 0.2f + 0.6f * x;
			g = 0.2f + 0.8f * x;
			b = 0.5f + 0.5f * x;
		} else {
			float x = (t - 0.5f) * 2.0f;
			r = 0.8f + 0.2f * x;
			g = 1.0f - 0.6f * x;
			b = 1.0f - 1.0f * x;
		}
		return IM_COL32(
			static_cast<int>(r * 255),
			static_cast<int>(g * 255),
			static_cast<int>(b * 255),
			255);
	}

	void ProfilerPanel::OnDraw(IGUIContext& gui) {
		if (!ImGui::Begin("Profiler", nullptr, ImGuiWindowFlags_NoNavInputs)) {
			return;
		}

		Profiler& profiler = Profiler::Instance();

		ImGui::Text("FPS: %.1f", profiler.GetFPS());
		ImGui::Text("UPS: %.1f", profiler.GetUPS());
		ImGui::Separator();
		ImGui::Text("Frame: %.2f ms", profiler.GetFrameTimeMs());
		ImGui::Text("Update: %.2f ms", profiler.GetUpdateTimeMs());
		ImGui::Text("Frame (min/avg/max): %.2f / %.2f / %.2f ms",
			profiler.GetFrameTimeMinMs(),
			profiler.GetFrameTimeAvgMs(),
			profiler.GetFrameTimeMaxMs());

		size_t historyCount = profiler.GetFrameTimeHistoryCount();
		if (historyCount > 0) {
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Text("Frame time (ms)");
			struct PlotUserData {
				Profiler* p;
			};
			PlotUserData plotData = { &profiler };
			ImGui::PlotLines("##FrameTime",
				[](void* data, int idx) -> float {
					PlotUserData* ud = static_cast<PlotUserData*>(data);
					return static_cast<float>(ud->p->GetFrameTimeAtHistoryIndex(static_cast<size_t>(idx)));
				},
				&plotData,
				static_cast<int>(historyCount),
				0,
				nullptr,
				0.0f,
				0.0f,
				ImVec2(-1, 80.0f));
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Checkbox("Heat map", &showHeatMap);
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Shows where the engine spends time (smoothed, cold=fast hot=slow)");
		}

		if (showHeatMap) {
			auto zones = profiler.GetZoneHeatMap();
			if (zones.empty()) {
				ImGui::TextDisabled("(no zone data yet)");
			} else {
				double totalMs = 0.0;
				double maxMs = 0.0;
				for (const auto& [name, ms] : zones) {
					totalMs += ms;
					if (ms > maxMs) {
						maxMs = ms;
					}
				}
				const float barHeight = 24.0f;
				float avail = ImGui::GetContentRegionAvail().x;
				ImVec2 barMin = ImGui::GetCursorScreenPos();
				ImVec2 barMax = ImVec2(barMin.x + avail, barMin.y + barHeight);
				ImGui::GetWindowDrawList()->AddRectFilled(barMin, barMax, IM_COL32(40, 40, 48, 255));
				float x = barMin.x;
				for (const auto& [name, ms] : zones) {
					float t = (maxMs > 0.0) ? static_cast<float>(ms / maxMs) : 0.0f;
					ImU32 col = HeatColor(t);
					float segW = (totalMs > 0.0) ? static_cast<float>(ms / totalMs) * avail : 0.0f;
					if (segW >= 1.0f) {
						ImGui::GetWindowDrawList()->AddRectFilled(
							ImVec2(x, barMin.y), ImVec2(x + segW, barMax.y), col);
					}
					x += segW;
				}
				ImGui::Dummy(ImVec2(avail, barHeight));
				ImGui::Spacing();
				for (const auto& [name, ms] : zones) {
					float t = (maxMs > 0.0) ? static_cast<float>(ms / maxMs) : 0.0f;
					ImU32 col = HeatColor(t);
					ImGui::ColorButton("##heat", ImGui::ColorConvertU32ToFloat4(col), 0, ImVec2(12, 12));
					ImGui::SameLine();
					ImGui::Text("%s: %.2f ms (%.1f%%)", name.c_str(), ms,
						(totalMs > 0.0) ? (100.0 * ms / totalMs) : 0.0);
				}
			}
		}

		ImGui::End();
	}

}
