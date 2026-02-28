#pragma once

#include "Core.h"
#include "MathUtils.h"

namespace Iberus {

	enum class RenderStyle : int {
		Default = 0,
		PixelStyle = 1
	};

	struct IBERUS_API PostProcessSettings {
		// Effect toggles
		bool enableBloom{ true };
		bool enablePixelation{ false };
		bool enableOutlineHighlight{ false };
		bool enableToon{ false };

		RenderStyle renderStyle{ RenderStyle::Default };

		// Bloom params
		float bloomThreshold{ 1.0f };
		float bloomIntensity{ 0.5f };
		float bloomRadius{ 0.4f };
		float bloomSpread{ 24.0f };

		// Pixelation
		int pixelCount{ 0 };

		// Toon params
		int toonCuts{ 4 };
		float toonSteepness{ 1.0f };
		float toonWrap{ 0.0f };
		float toonRimWidth{ 4.0f };

		// Outline params
		float outlineShadowStrength{ 0.5f };
		float outlineHighlightStrength{ 0.5f };
		Math::Vec3 outlineShadowColor{ 0.0f, 0.0f, 0.0f };
		Math::Vec3 outlineHighlightColor{ 1.0f, 1.0f, 1.0f };

		// Stub flags (reserved for future use)
		bool enableFog{ false };
		bool enableSSAO{ false };
		bool enableShadows{ false };

		// SDF raymarch debug: 0=normal, 5=creatureCount, 6=tileCount, 7=rayMayHit, 8=distField, 9=raymarch hit
		int sdfDebugMode{ 0 };
		// SDF radius scale: multiplier for all part radii (1=normal; <1 thins, >1 thickens)
		float sdfRadiusScale{ 1.0f };
		// Force CPU tile build (bypass GPU compute) when GPU path produces empty tiles
		bool sdfForceCPUTiles{ false };
		// Enable coarse 8x8 pre-pass (skip full raymarch where block has no hit)
		bool sdfUseCoarsePass{ false };
		// Coarse debug: 0=hit mask, 1=tileCount/64, 2=gradient, 3=constant red (verify main pass path)
		int sdfCoarseDebugMode{ 0 };
	};
}
