#include "Enginepch.h"
#include "RenderSettingsPanel.h"
#include "Editor.h"
#include "Engine.h"
#include "RenderSettings.h"
#include "Renderer.h"
#include "RenderPass.h"

#include "imgui.h"

namespace Iberus {

	RenderSettingsPanel::RenderSettingsPanel(Editor& editor)
		: editor(editor) {
	}

	void RenderSettingsPanel::DrawPostProcessSettings() {
		PostProcessSettings& settings = Engine::Instance()->GetPostProcessSettings();

		if (ImGui::CollapsingHeader("Post-Process", ImGuiTreeNodeFlags_DefaultOpen)) {
			const char* renderStyleNames[] = { "Default", "Pixel Style" };
			int styleIdx = static_cast<int>(settings.renderStyle);
			if (ImGui::Combo("Render Style##PostProcess", &styleIdx, renderStyleNames, 2)) {
				settings.renderStyle = static_cast<RenderStyle>(styleIdx);
			}

			ImGui::Separator();
			ImGui::Checkbox("Bloom##PostProcess", &settings.enableBloom);
			ImGui::Checkbox("Pixelation##PostProcess", &settings.enablePixelation);
			ImGui::Checkbox("Outline/Highlight##PostProcess", &settings.enableOutlineHighlight);
			ImGui::Checkbox("Toon Shading##PostProcess", &settings.enableToon);

			if (settings.enableBloom) {
				ImGui::Indent();
				ImGui::SliderFloat("Bloom Threshold##PostProcess", &settings.bloomThreshold, 0.1f, 3.0f, "%.2f");
				ImGui::SliderFloat("Bloom Intensity##PostProcess", &settings.bloomIntensity, 0.0f, 2.0f, "%.2f");
				ImGui::SliderFloat("Bloom Radius##PostProcess", &settings.bloomRadius, 0.0f, 1.0f, "%.2f");
				ImGui::Unindent();
			}

			if (settings.enablePixelation) {
				ImGui::Indent();
				ImGui::SliderInt("Pixel Count##PostProcess", &settings.pixelCount, 0, 512, "%d", ImGuiSliderFlags_AlwaysClamp);
				ImGui::Unindent();
			}

			if (settings.enableToon) {
				ImGui::Indent();
				ImGui::SliderInt("Toon Cuts##PostProcess", &settings.toonCuts, 2, 16, "%d", ImGuiSliderFlags_AlwaysClamp);
				ImGui::SliderFloat("Toon Steepness##PostProcess", &settings.toonSteepness, 0.1f, 3.0f, "%.2f");
				ImGui::SliderFloat("Toon Wrap##PostProcess", &settings.toonWrap, -1.0f, 1.0f, "%.2f");
				ImGui::SliderFloat("Toon Rim Width##PostProcess", &settings.toonRimWidth, 1.0f, 16.0f, "%.1f");
				ImGui::Unindent();
			}

			if (settings.enableOutlineHighlight) {
				ImGui::Indent();
				ImGui::SliderFloat("Shadow Strength##PostProcess", &settings.outlineShadowStrength, 0.0f, 1.0f, "%.2f");
				ImGui::SliderFloat("Highlight Strength##PostProcess", &settings.outlineHighlightStrength, 0.0f, 1.0f, "%.2f");
				ImGui::ColorEdit3("Shadow Color##PostProcess", &settings.outlineShadowColor.x);
				ImGui::ColorEdit3("Highlight Color##PostProcess", &settings.outlineHighlightColor.x);
				ImGui::Unindent();
			}

			ImGui::Spacing();
			ImGui::TextDisabled("(Reserved)");
			ImGui::Checkbox("Fog##PostProcessStub", &settings.enableFog);
			ImGui::Checkbox("SSAO##PostProcessStub", &settings.enableSSAO);
			ImGui::Checkbox("Shadows##PostProcessStub", &settings.enableShadows);
		}
	}

	void RenderSettingsPanel::DrawPipelinePasses() {
		if (ImGui::CollapsingHeader("Pipeline Passes", ImGuiTreeNodeFlags_DefaultOpen)) {
			std::vector<RenderPass*> passes = Engine::Instance()->GetRenderer().GetRenderPasses();
			if (passes.empty()) {
				ImGui::TextDisabled("(No passes - forward renderer or not initialized)");
				return;
			}

			ImGui::Text("Order defines execution. Disabled passes are skipped.");
			ImGui::TextDisabled("Swap only occurs after executed passes.");
			ImGui::Spacing();

			for (size_t i = 0; i < passes.size(); ++i) {
				RenderPass* pass = passes[i];
				bool enabled = pass->IsEnabled();
				if (ImGui::Checkbox((std::string("##") + std::to_string(i)).c_str(), &enabled)) {
					pass->SetEnabled(enabled);
				}
				ImGui::SameLine();
				ImGui::Text("%zu. %s", i + 1, pass->GetName().c_str());
			}
		}
	}

	void RenderSettingsPanel::OnDraw(IGUIContext& gui) {
		if (requestOpen) {
			ImGui::SetNextWindowCollapsed(false);
			ImGui::SetNextWindowFocus();
			requestOpen = false;
		}

		if (!ImGui::Begin("Render Settings", nullptr, ImGuiWindowFlags_NoNavInputs)) {
			return;
		}

		DrawPostProcessSettings();
		ImGui::Spacing();
		DrawPipelinePasses();

		ImGui::End();
	}

}
