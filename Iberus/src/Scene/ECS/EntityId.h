#pragma once

#include "Core.h"

namespace Iberus {

	/// Stable handle to an entity. Used instead of raw pointers.
	using EntityId = uint64_t;

	static constexpr EntityId NullEntity = 0;

}
