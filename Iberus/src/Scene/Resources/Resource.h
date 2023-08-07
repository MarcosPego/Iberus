#pragma once

#include "Buffer.h"

namespace Iberus {
	class Resource {
	public:
		explicit Resource(const std::string& inboundID);

		virtual ~Resource() {}

		void SetSource(const std::string& inboundSource) { source = inboundSource; }

		const std::string& GetID() const { return ID; }

	protected:
		virtual bool Load(Buffer inboundBuffer) { return false; }

		std::string ID;
		std::string source;
	};

}

