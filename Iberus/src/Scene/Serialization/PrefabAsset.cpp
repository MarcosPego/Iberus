#include "Enginepch.h"
#include "PrefabAsset.h"

namespace Iberus {

	PrefabAsset::PrefabAsset(Buffer buf)
		: buffer(std::move(buf)) {
	}

}
