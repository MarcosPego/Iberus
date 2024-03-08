#pragma once

namespace Iberus {
	struct IBERUS_API Color {
		union {
			struct {
				uint8_t r;
				uint8_t g;
				uint8_t b;
				uint8_t a;
			};
			uint32_t color;
		};

		Color() : r(255), g(255), b(255), a(255) {};
		Color(uint8_t inR, uint8_t inG, uint8_t inB, uint8_t inA = 255) : r(inR), g(inG), b(inB), a(inA) {};
	};
}
