#pragma once

namespace Iberus {

	class RenderObject {
	public:
		RenderObject(const std::string& inboundID) : ID(inboundID) {

		}

		RenderObject(const std::string& inboundID, uint32_t inboundHandle) : ID(inboundID), handle(inboundHandle) {

		}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual ~RenderObject() {}

	protected:
		std::string ID;
		uint32_t handle{ 0 }; // Develop this
	};

}


