#include "Enginepch.h"
#include "Resource.h"

namespace Iberus {

	Resource::Resource(const std::string& inboundID) {
		ID = inboundID;
	}

	Resource::Resource(const std::string& inboundID, Buffer inboundBuffer) {
		ID = inboundID;
		Load(std::move(inboundBuffer));
	}

}