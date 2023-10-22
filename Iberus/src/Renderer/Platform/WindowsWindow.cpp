#include "Enginepch.h"
#include "WindowsWindow.h"

namespace Iberus {
	static void GLFWErrorCallback(int error, const char* description) {
		IB_CORE_ERROR("GLFW Error ({}) : {}", error, description);
	}

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
		glfwSwapBuffers(window);
		glfwPollEvents();; // TODO(MPP) Fix window event manager!
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


	void WindowsWindow::SetEventCallback(const EventCallbackFn& eventCallback) {
		windowData.EventCallback = eventCallback;
	}

	void WindowsWindow::SetupGraphicalContext() {
		// TODO(MPP) Review this setup later. The ideal is to also support vulkan later

		// Setup GLFW
		//glfwSetErrorCallback(glfw_error_callback); // TODO(MPP) Implement this
		int gl_major = 4, gl_minor = 3; // TODO(MPP) This needs a proper place

		if (!glfwInit())
		{
			glfwSetErrorCallback(GLFWErrorCallback);
			exit(EXIT_FAILURE);
			return;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		glfwWindowHint(GLFW_MAXIMIZED, 1);

		SetupWindow();

		// Setup Glew
		glewExperimental = GL_TRUE;
		GLenum result = glewInit();
		if (result != GLEW_OK) {
			glfwSetErrorCallback(GLFWErrorCallback);
			//std::cerr << "ERROR glewInit: " << glewGetString(result) << std::endl;
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
		GLFWmonitor* monitor = windowProps.isFullScreen ? glfwGetPrimaryMonitor() : 0;
		GLFWwindow* win = glfwCreateWindow(GetWidth(), GetHeight(), GetTitle().c_str(), monitor, 0);
		if (!win) {
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		window = win;
		glfwMakeContextCurrent(window);
		glfwSetWindowUserPointer(window, &windowData);
		SetVSync(true);

		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		/// Set Callbacks
		glfwSetWindowSizeCallback(window, [](GLFWwindow* glfwWindow, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(glfwWindow);
			data.resolution.x = width;
			data.resolution.y = height;

			WindowResizeEvent event({ (uint32_t)width, (uint32_t)height });
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* glfwWindow) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(glfwWindow);

			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(window, [](GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(glfwWindow);

			switch (action) {
				case GLFW_PRESS: {
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
				} break;
				case GLFW_RELEASE: {
					KeyReleasedEvent event(key);
					data.EventCallback(event);
				} break;
				case GLFW_REPEAT: {
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
				} break;
				default:
					break;
			}
		});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* glfwWindow, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(glfwWindow);

			switch (action) {
				case GLFW_PRESS: {
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
				} break;
				case GLFW_RELEASE: {
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
				} break;
				case GLFW_REPEAT: {
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
				} break;
				default:
					break;
			}
		});

		glfwSetScrollCallback(window, [](GLFWwindow* glfwWindow, double xOffset, double yOffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(glfwWindow);

			MouseScrolledEvent event({ (float)xOffset , (float)yOffset });
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(window, [](GLFWwindow* glfwWindow, double xPos, double yPos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(glfwWindow);

			MouseMovedEvent event({ (float)xPos , (float)yPos });
			data.EventCallback(event);
		});
	}
}

