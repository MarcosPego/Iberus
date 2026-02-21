#include "Enginepch.h"
#include "ProfilerPanel.h"
#include "Editor.h"
#include "Profiler.h"

#include "imgui.h"

namespace Iberus {

	ProfilerPanel::ProfilerPanel(Editor& editor)
		: editor(editor) {
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

		ImGui::End();
	}

}
