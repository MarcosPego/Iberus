#include "Enginepch.h"
#include "WindowsWindow.h"

namespace Iberus {
	WindowsWindow::WindowsWindow(const Vec2& resolution, const std::string& title) : Window(resolution, title) {
		SetupGraphicalContext();
	}
	WindowsWindow::WindowsWindow(const WindowProps& newWindowProps) : Window(newWindowProps) {
		SetupGraphicalContext();
	}

	WindowsWindow::~WindowsWindow() {
		Shutdown();
	}

	void WindowsWindow::Update() {
		glClearColor(0.15f, 0.15f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwWaitEvents();
	}

	void WindowsWindow::Shutdown() {
		glfwDestroyWindow(window);
	}

	void WindowsWindow::SetVSync(bool enabled) {
		if (enabled) {
			glfwSwapInterval(1);
		}
		else {
			glfwSwapInterval(0);
		}

		windowData.vsync = enabled;
	}

	void WindowsWindow::SetupGraphicalContext() {
		// TODO(MPP) Review this setup later. The ideal is to also support vulkan later

		// Setup GLFW
		//glfwSetErrorCallback(glfw_error_callback); // TODO(MPP) Implement this
		int gl_major = 4, gl_minor = 3; // TODO(MPP) This needs a proper place

		if (!glfwInit())
		{
			exit(EXIT_FAILURE);
			return;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

		SetupWindow();

		// Setup Glew
		glewExperimental = GL_TRUE;
		GLenum result = glewInit();
		if (result != GLEW_OK)
		{
			std::cerr << "ERROR glewInit: " << glewGetString(result) << std::endl;
			exit(EXIT_FAILURE);
		}
		GLenum err_code = glGetError();

		const GLubyte* renderer = glGetString(GL_RENDERER);
		const GLubyte* vendor = glGetString(GL_VENDOR);
		const GLubyte* version = glGetString(GL_VERSION);
		const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
		std::cerr << "OpenGL Renderer: " << renderer << " (" << vendor << ")" << std::endl;
		std::cerr << "OpenGL version " << version << std::endl;
		std::cerr << "GLSL version " << glslVersion << std::endl;

		// Setup OpenGL
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		glDepthRange(0.0, 1.0);
		glClearDepth(1.0);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glViewport(0, 0, GetWidth(), GetHeight());
	}

	void WindowsWindow::SetupWindow() {
		// TODO(MPP) Review this setup later

		GLFWmonitor* monitor = windowProps.isFullScreen ? glfwGetPrimaryMonitor() : 0;
		GLFWwindow* win = glfwCreateWindow(GetWidth(), GetHeight(), GetTitle().c_str(), monitor, 0);
		if (!win) {
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		window = win;
		glfwMakeContextCurrent(window);
		SetVSync(true);
	}
}

