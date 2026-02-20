#pragma once

#include "IGUIContext.h"

struct GLFWwindow;

namespace Iberus {

	class ImguiContext : public IGUIContext {
	public:
		ImguiContext() = default;
		~ImguiContext() override;

		bool Init(void* nativeWindow);
		void Shutdown();

		void BeginFrame();
		void EndFrame(bool render = true, bool clearBackdrop = true);

		bool IsMinimized() const;

		bool BeginWindow(const char* title) override;
		void EndWindow() override;
		void Text(const char* text) override;
		bool Button(const char* label) override;

		void BeginDockSpace() override;
		void EndDockSpace() override;
		void Image(void* textureId, float w, float h, float uv0x = 0.0f, float uv0y = 0.0f, float uv1x = 1.0f, float uv1y = 1.0f) override;

	private:
		bool initialized{ false };
		void* nativeWindow{ nullptr };
	};

}
