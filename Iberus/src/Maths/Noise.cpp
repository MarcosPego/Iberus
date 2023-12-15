#include "Enginepch.h"
#include "Noise.h"

#include <FastNoiseSIMD.h>

namespace Math {

	Noise* Noise::Instance() {
		static Noise noise;
		return &noise;
	}


	NoiseSample Noise::GetNoise(const Vec3 start, const Vec3& offset) {
		NoiseSample sample;

		auto size =  (std::size_t)((offset.x - start.x) + (offset.y - start.y) + (offset.z - start.z));

		if (size < 0) {
			return sample;
		}

		auto noiseSet = noiseAPI->GetSampledNoiseSet(start.x, start.y, start.z, offset.x, offset.y, offset.z, 1.0f);

		sample.noiseFloatBuffer = Iberus::FloatBuffer(size);


		return sample;
	}

	Noise::Noise() {
		noiseAPI = std::make_unique<FastNoiseSIMD>(FastNoiseSIMD::NewFastNoiseSIMD());
	}

	Noise::~Noise() {

	}
}