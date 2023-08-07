#pragma once

#include "Buffer.h"

namespace Iberus {
	class Resource {
	public:
		Resource(const std::string& inboundID, Buffer inboundBuffer);

		void SetSource(const std::string& inboundSource) { source = inboundSource; }

	protected:
		virtual bool Load(Buffer inboundBuffer) { return false; }

		std::string ID;
		std::string source;
	};

}

