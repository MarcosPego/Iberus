#include "Enginepch.h"
#include "Noise.h"

#include "Color.h"

#include <FastNoise.h>

namespace Math {

	Noise* Noise::Instance() {
		static Noise noise;
		return &noise;
	}


	NoiseSample Noise::GetNoise(const Vec3& start, const Vec3& offset, int seed, float frequency, int octaves) {
		NoiseSample sample;

		auto size = (std::size_t)((offset.x - start.x) * (offset.y - start.y) * (offset.z - start.z));

		if (size < 0) {
			return sample;
		}

		// TODO(MPP) Evaluate having to delete these
		auto fnSimplex = FastNoise::New<FastNoise::Perlin>();
		auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();

		fnFractal->SetSource(fnSimplex);
		fnFractal->SetGain(0.5f);
		fnFractal->SetOctaveCount(octaves);
		fnFractal->SetLacunarity(2.0f);


		/*noiseAPI->SetNoiseType(FastNoiseSIMD::Perlin); // TODO(MPP) Expose this
		noiseAPI->SetSeed(seed);
		noiseAPI->SetFrequency(frequency);
		noiseAPI->SetFractalOctaves(octaves);

		auto noiseSet = noiseAPI->GetSampledNoiseSet(start.x, start.y, start.z, offset.x, offset.y, offset.z, 1.0f);*/

		sample.noiseFloatBuffer = Iberus::FloatBuffer(size);
		fnFractal->GenUniformGrid3D(sample.noiseFloatBuffer.GetData(), start.x, start.y, start.z, offset.x, offset.y, offset.z, frequency, seed);
	
		sample.start = start;
		sample.offset = offset;

		//noiseAPI->FreeNoiseSet(noiseSet);

		return sample;
	}

	Iberus::Buffer Noise::SampleToTextureBuffer(NoiseSample& noiseSample, int channel) {
		auto* bufferData = noiseSample.noiseFloatBuffer.GetData();
		Iberus::Buffer outData = Iberus::Buffer(noiseSample.noiseFloatBuffer.GetSize() * channel);
		std::fill_n(outData.GetData(), outData.GetSize(), 0); // Create black texture

		const auto width = noiseSample.offset.x - noiseSample.start.x;
		const auto height = noiseSample.offset.y - noiseSample.start.y;
		const auto slice = noiseSample.offset.z - noiseSample.start.z;

		for (int z = 0; z < slice; z++) {
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					uint32_t dstIndex = (z * (width * height) + y * width + x) * channel;
					uint32_t srcIndex = z * (width * height) + y * width + x;
					auto value = std::max( (int)((bufferData[srcIndex] + 1)/2 * 255), 0);
					*(uint32_t*)(outData.GetData() + dstIndex) = Iberus::Color(value, value, value).color;
			
				}
			}
		}

		return std::move(outData);
	}

	Noise::Noise() {
		//noiseAPI = FastNoise::New<FastNoise::Simplex>();
	}

	Noise::~Noise() {
		//delete noiseAPI;
	}
}