#include "Enginepch.h"
#include "GameViewPanel.h"
#include "Editor.h"
#include "Engine.h"
#include "Application.h"

#include "imgui.h"

namespace Iberus {

	GameViewPanel::GameViewPanel(Editor& editor) : editor(editor) {
	}

	void GameViewPanel::OnDraw(IGUIContext& gui) {
		if (!ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoNavInputs)) {
			return;
		}

		if (Application::Get()->IsGameFullscreen()) {
			ImGui::End();
			return;
		}

		if (ImGui::IsWindowCollapsed()) {
			ImGui::End();
			return;
		}

		ImVec2 size = ImGui::GetContentRegionAvail();
		int w = static_cast<int>(size.x);
		int h = static_cast<int>(size.y);
		const int minViewportSize = 8;
		if (w < minViewportSize || h < minViewportSize || w > 16384 || h > 16384) {
			ImGui::End();
			return;
		}
		{
			if (!gameFbo) {
				gameFbo = std::make_unique<SceneViewFBO>();
			}
			gameFbo->Resize(w, h);
			Engine::Instance()->AddEditorView(gameFbo->GetFBO(), w, h, nullptr);
			unsigned int texId = gameFbo->GetTextureID();
			if (texId != 0) {
				gui.Image(reinterpret_cast<void*>(static_cast<intptr_t>(texId)), static_cast<float>(w), static_cast<float>(h), 0.0f, 1.0f, 1.0f, 0.0f);
			}
		}
		ImGui::End();
	}

}
