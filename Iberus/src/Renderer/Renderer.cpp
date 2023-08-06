#include "Enginepch.h"
#include "Renderer.h"

#include "OpenGLRenderer.h"

namespace Iberus {

	Renderer* Renderer::Create() {
		return new OpenGLRenderer();
	}

}