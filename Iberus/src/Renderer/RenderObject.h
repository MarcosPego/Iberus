#pragma once

namespace {

	class RenderObject {
	public:
		RenderObject(const std::string& inboundID, uint32_t inboundHandle) : ID(inboundID), handle(inboundHandle) {

		}

	protected:
		std::string ID;
		uint32_t handle; // Develop this
	};

}


