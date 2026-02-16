#include "Enginepch.h"
#include "SceneViewPanel.h"
#include "Editor.h"
#include "Engine.h"

#include "imgui.h"

namespace Iberus {

	SceneViewPanel::SceneViewPanel(Editor& editor) : editor(editor) {
	}

	void SceneViewPanel::OnDraw(IGUIContext& gui) {
		if (!ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoNavInputs)) {
			editor.SetViewportFocused(false);
			return;
		}
		editor.SetViewportFocused(ImGui::IsWindowHovered());
		ImVec2 size = ImGui::GetContentRegionAvail();
		int w = static_cast<int>(size.x);
		int h = static_cast<int>(size.y);
		if (w > 0 && h > 0) {
			if (!fbo) {
				fbo = std::make_unique<SceneViewFBO>();
			}
			fbo->Resize(w, h);
			Engine::Instance()->SetEditorRenderTarget(fbo->GetFBO(), w, h);
			gui.Image(reinterpret_cast<void*>(static_cast<intptr_t>(fbo->GetTextureID())), static_cast<float>(w), static_cast<float>(h), 0.0f, 1.0f, 1.0f, 0.0f);
		}
		ImGui::End();
	}

}
