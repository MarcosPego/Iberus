#pragma once

#include "Enginepch.h"
#include "MathUtils.h"

class FastNoiseSIMD;

namespace Math {


	struct NoiseSample {
		Vec3 start;
		Vec3 offset;

		Iberus::FloatBuffer noiseFloatBuffer;

		bool Invalid() const {
			return !valid || noiseFloatBuffer.Invalid();
		}

		bool valid{ false };
	};

	class Noise {
	public:
		static Noise* Instance();

		NoiseSample GetNoise(const Vec3 start, const Vec3& offset);

	private:
		Noise();
		virtual ~Noise();

		std::unique_ptr<FastNoiseSIMD> noiseAPI;
	
	};

}


