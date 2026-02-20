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
		int xCount = static_cast<int>(offset.x - start.x);
		int yCount = static_cast<int>(offset.y - start.y);
		int zCount = static_cast<int>(offset.z - start.z);
		fnFractal->GenUniformGrid3D(sample.noiseFloatBuffer.GetData(), start.x, start.y, start.z, xCount, yCount, zCount, frequency, frequency, frequency, seed);
	
		sample.start = start;
		sample.offset = offset;

		//noiseAPI->FreeNoiseSet(noiseSet);

		return sample;
	}

	void Noise::GetHeightMap2D(std::vector<float>& outHeights, int width, int height, float xOffset, float zOffset,
		float frequency, int seed, int octaves, bool normalizeToZeroOne) {
		outHeights.resize(static_cast<size_t>(width) * height);
		if (outHeights.empty()) {
			return;
		}

		auto fnPerlin = FastNoise::New<FastNoise::Perlin>();
		fnPerlin->SetScale(0.3f);
		auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();
		fnFractal->SetSource(fnPerlin);
		fnFractal->SetGain(0.5f);
		fnFractal->SetOctaveCount(octaves);
		fnFractal->SetLacunarity(2.0f);

		auto minMax = fnFractal->GenUniformGrid2D(outHeights.data(), xOffset, zOffset, width, height,
			frequency, frequency, seed);

		if (normalizeToZeroOne) {
			const float range = minMax.max - minMax.min;
			const float invRange = range > 1e-6f ? 1.0f / range : 1.0f;
			for (float& h : outHeights) {
				h = (h - minMax.min) * invRange;
			}
		}
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
					uint32_t dstIndex = static_cast<uint32_t>((z * (width * height) + y * width + x) * channel);
					uint32_t srcIndex = static_cast<uint32_t>(z * (width * height) + y * width + x);
					int value = std::max(static_cast<int>((bufferData[srcIndex] + 1.0f) / 2.0f * 255.0f), 0);
					value = std::min(value, 255);
					uint8_t v = static_cast<uint8_t>(value);
					*(uint32_t*)(outData.GetData() + dstIndex) = Iberus::Color(v, v, v).color;
			
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