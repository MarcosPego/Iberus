#pragma once

#include "Enginepch.h"
#include "MathUtils.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

class FastNoiseSIMD;
namespace Math {

	struct NoiseSample {
		Vec3 start{ 0,0,0 };
		Vec3 offset{ 0,0,0 };

		Iberus::FloatBuffer noiseFloatBuffer;

		bool Invalid() const {
			return !valid || noiseFloatBuffer.Invalid();
		}

		bool valid{ true }; // TODO(MPP) Use this?
	};

	class IBERUS_API Noise {
	public:
		static Noise* Instance();

		NoiseSample GetNoise(const Vec3& start, const Vec3& offset, int seed, float frequency, int octaves);

		Iberus::Buffer SampleToTextureBuffer(NoiseSample& noiseSample, int channel = 4);

	private:
		Noise();
		virtual ~Noise();

		//FastNoise::SmartNode* noiseAPI{nullptr};
	};

}


