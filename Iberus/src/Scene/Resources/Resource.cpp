#include "Enginepch.h"
#include "Resource.h"

namespace Iberus {

	Resource::Resource(const std::string& inboundID, Buffer inboundBuffer) {
		Load(std::move(inboundBuffer));
	}

}