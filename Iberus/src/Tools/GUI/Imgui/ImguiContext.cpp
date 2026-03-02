#include "Enginepch.h"
#include "ImguiContext.h"
#include "FileSystem.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "IconsFontAwesome6.h"

#include <GLFW/glfw3.h>
#include <filesystem>

namespace Iberus {

	ImguiContext::~ImguiContext() {
		Shutdown();
	}

	bool ImguiContext::Init(void* nativeWindow) {
		if (initialized) {
			return true;
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// Cursor panel palette: #141414, #181818 - dark, minimal blue
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 2.0f;
		style.ChildRounding = 2.0f;
		style.FrameRounding = 2.0f;
		style.PopupRounding = 2.0f;
		style.ScrollbarRounding = 4.0f;
		style.GrabRounding = 2.0f;
		style.TabRounding = 2.0f;
		style.FramePadding = ImVec2(8.0f, 5.0f);
		style.WindowBorderSize = 1.0f;
		style.FrameBorderSize = 1.0f;
		style.ChildBorderSize = 1.0f;

		// Cursor panel palette: #141414, #181818 - dark, minimal blue
		ImVec4* colors = style.Colors;
		const ImVec4 bgDarkest(0.078f, 0.078f, 0.078f, 1.0f);    // #141414
		const ImVec4 bgPanel(0.094f, 0.094f, 0.094f, 1.0f);      // #181818
		const ImVec4 bgRaised(0.11f, 0.11f, 0.11f, 1.0f);        // #1C1C1C
		const ImVec4 bgInput(0.125f, 0.125f, 0.125f, 1.0f);      // #202020
		const ImVec4 bgHover(0.16f, 0.16f, 0.16f, 1.0f);        // #292929
		const ImVec4 bgSelect(0.18f, 0.18f, 0.19f, 1.0f);       // #2E2E2F list/header
		const ImVec4 border(0.22f, 0.22f, 0.22f, 1.0f);         // #383838
		const ImVec4 textMain(0.82f, 0.82f, 0.82f, 1.0f);       // #D1D1D1
		const ImVec4 textMuted(0.5f, 0.5f, 0.5f, 1.0f);        // #808080
		const ImVec4 accentBlue(0.0f, 0.478f, 0.8f, 1.0f);       // #007ACC - active tab only

		colors[ImGuiCol_Text] = textMain;
		colors[ImGuiCol_TextDisabled] = textMuted;
		colors[ImGuiCol_WindowBg] = ImVec4(bgDarkest.x, bgDarkest.y, bgDarkest.z, 0.98f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_PopupBg] = ImVec4(bgPanel.x, bgPanel.y, bgPanel.z, 0.98f);
		colors[ImGuiCol_Border] = border;
		colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_FrameBg] = ImVec4(bgInput.x, bgInput.y, bgInput.z, 0.9f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(bgHover.x, bgHover.y, bgHover.z, 0.95f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
		colors[ImGuiCol_TitleBg] = ImVec4(bgPanel.x, bgPanel.y, bgPanel.z, 1.0f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(bgPanel.x, bgPanel.y, bgPanel.z, 1.0f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(bgDarkest.x, bgDarkest.y, bgDarkest.z, 0.75f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(bgPanel.x, bgPanel.y, bgPanel.z, 1.0f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(bgDarkest.x, bgDarkest.y, bgDarkest.z, 0.6f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.55f, 0.55f, 0.55f, 1.0f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.4f, 0.4f, 0.42f, 1.0f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5f, 0.5f, 0.52f, 1.0f);
		colors[ImGuiCol_Button] = ImVec4(bgInput.x, bgInput.y, bgInput.z, 0.9f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(bgHover.x, bgHover.y, bgHover.z, 1.0f);
		colors[ImGuiCol_ButtonActive] = ImVec4(bgRaised.x, bgRaised.y, bgRaised.z, 1.0f);
		// Headers = tree/list selection - gray, no blue
		colors[ImGuiCol_Header] = ImVec4(bgSelect.x, bgSelect.y, bgSelect.z, 0.9f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(bgHover.x, bgHover.y, bgHover.z, 0.95f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.2f, 0.2f, 0.21f, 1.0f);
		colors[ImGuiCol_Separator] = border;
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.35f, 0.35f, 0.35f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.45f, 0.45f, 0.45f, 0.6f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.5f, 0.5f, 0.5f, 0.95f);
		// Tabs - blue only for active tab overline
		colors[ImGuiCol_Tab] = ImVec4(bgRaised.x, bgRaised.y, bgRaised.z, 0.9f);
		colors[ImGuiCol_TabHovered] = ImVec4(bgHover.x, bgHover.y, bgHover.z, 0.95f);
		colors[ImGuiCol_TabSelected] = ImVec4(bgDarkest.x, bgDarkest.y, bgDarkest.z, 1.0f);
		colors[ImGuiCol_TabSelectedOverline] = accentBlue;
		colors[ImGuiCol_TabDimmed] = ImVec4(bgDarkest.x, bgDarkest.y, bgDarkest.z, 0.8f);
		colors[ImGuiCol_TabDimmedSelected] = ImVec4(bgDarkest.x, bgDarkest.y, bgDarkest.z, 0.95f);
		colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(accentBlue.x, accentBlue.y, accentBlue.z, 0.5f);
		colors[ImGuiCol_DockingPreview] = ImVec4(bgHover.x, bgHover.y, bgHover.z, 0.4f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(border.x, border.y, border.z, 1.0f);
		colors[ImGuiCol_TextLink] = ImVec4(0.6f, 0.65f, 0.75f, 1.0f);  // muted, not blue
		colors[ImGuiCol_TextSelectedBg] = ImVec4(bgSelect.x, bgSelect.y, bgSelect.z, 0.5f);
		colors[ImGuiCol_NavCursor] = ImVec4(0.5f, 0.5f, 0.52f, 0.9f);

		// Load default font and merge Font Awesome icons
		io.Fonts->AddFontDefault();
		std::string fontPath = FileSystem::GetAppDirectory() + "/fonts/fa-solid-900.ttf";
		if (std::filesystem::exists(fontPath)) {
			ImFontConfig config;
			config.MergeMode = true;
			config.GlyphMinAdvanceX = 13.0f;
			static const ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
			io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 13.0f, &config, iconRanges);
		}

		GLFWwindow* window = static_cast<GLFWwindow*>(nativeWindow);
		if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
			return false;
		}
		if (!ImGui_ImplOpenGL3_Init("#version 460")) {
			ImGui_ImplGlfw_Shutdown();
			return false;
		}

		this->nativeWindow = nativeWindow;
		initialized = true;
		return true;
	}

	void ImguiContext::Shutdown() {
		if (!initialized) {
			return;
		}

		nativeWindow = nullptr;
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		initialized = false;
	}

	void ImguiContext::BeginFrame() {
		if (!initialized) {
			return;
		}
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	bool ImguiContext::IsMinimized() const {
		if (!initialized || !nativeWindow) {
			return false;
		}
		return glfwGetWindowAttrib(static_cast<GLFWwindow*>(nativeWindow), GLFW_ICONIFIED) == GLFW_TRUE;
	}

	void ImguiContext::EndFrame(bool render, bool clearBackdrop) {
		if (!initialized) {
			return;
		}
		ImGui::EndFrame();
		if (render) {
			ImGui::Render();
			ImDrawData* drawData = ImGui::GetDrawData();
			if (drawData) {
				int w = (int)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
				int h = (int)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
				if (w > 0 && h > 0) {
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
					glViewport(0, 0, w, h);
					if (clearBackdrop) {
						glClearColor(0.078f, 0.078f, 0.078f, 1.0f);  // #141414
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					}
				}
				ImGui_ImplOpenGL3_RenderDrawData(drawData);
			}
		}
	}

	bool ImguiContext::BeginWindow(const char* title, bool* p_open) {
		return ImGui::Begin(title, p_open);
	}

	void ImguiContext::EndWindow() {
		ImGui::End();
	}

	void ImguiContext::Text(const char* text) {
		ImGui::Text("%s", text);
	}

	bool ImguiContext::Button(const char* label) {
		return ImGui::Button(label);
	}

	void ImguiContext::BeginDockSpace() {
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
	}

	void ImguiContext::EndDockSpace() {
	}

	void ImguiContext::Image(void* textureId, float w, float h, float uv0x, float uv0y, float uv1x, float uv1y) {
		ImGui::Image(reinterpret_cast<ImTextureID>(textureId), ImVec2(w, h), ImVec2(uv0x, uv0y), ImVec2(uv1x, uv1y));
	}

}
