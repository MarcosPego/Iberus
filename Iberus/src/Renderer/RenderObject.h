#pragma once

namespace {

	class RenderObject {
	public:
		RenderObject(const std::string& inboundID, uint32_t inboundHandle) : ID(inboundID), handle(inboundHandle) {

		}

		virtual ~RenderObject() {}

	protected:
		std::string ID;
		uint32_t handle; // Develop this
	};

}


