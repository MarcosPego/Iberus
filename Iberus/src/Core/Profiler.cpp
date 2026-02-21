#include "Enginepch.h"
#include "Profiler.h"

#include <algorithm>

namespace Iberus {

	void Profiler::RecordZone(const std::string& name, double timeMs) {
		if (!heatMapEnabled || name.empty()) {
			return;
		}
		auto it = zones.find(name);
		if (it == zones.end()) {
			zones[name] = ZoneData{ timeMs, true };
		} else {
			double a = 1.0 - kZoneSmoothing;
			it->second.avgMs = a * it->second.avgMs + kZoneSmoothing * timeMs;
			it->second.hasData = true;
		}
		zoneCacheDirty = true;
	}

	std::vector<std::pair<std::string, double>> Profiler::GetZoneHeatMap() const {
		if (!zoneCacheDirty) {
			return zoneHeatMapCache;
		}
		zoneHeatMapCache.clear();
		zoneHeatMapCache.reserve(zones.size());
		for (const auto& [name, data] : zones) {
			if (data.hasData && data.avgMs > 0.0) {
				zoneHeatMapCache.emplace_back(name, data.avgMs);
			}
		}
		std::sort(zoneHeatMapCache.begin(), zoneHeatMapCache.end(),
			[](const auto& a, const auto& b) { return a.second > b.second; });
		zoneCacheDirty = false;
		return zoneHeatMapCache;
	}

	Profiler::Profiler()
		: frameWriteIdx(0)
		, updateWriteIdx(0)
		, frameCount(0)
		, updateCount(0) {
		std::fill(std::begin(frameTimes), std::end(frameTimes), 0.0);
		std::fill(std::begin(updateTimes), std::end(updateTimes), 0.0);
	}

	Profiler& Profiler::Instance() {
		static Profiler instance;
		return instance;
	}

	void Profiler::RecordFrame(double frameTimeSec) {
		frameTimes[frameWriteIdx] = frameTimeSec * 1000.0;
		frameWriteIdx = (frameWriteIdx + 1) % kHistorySize;
		if (frameCount < kHistorySize) {
			++frameCount;
		}
	}

	void Profiler::RecordUpdate(double updateTimeSec) {
		updateTimes[updateWriteIdx] = updateTimeSec * 1000.0;
		updateWriteIdx = (updateWriteIdx + 1) % kHistorySize;
		if (updateCount < kHistorySize) {
			++updateCount;
		}
	}

	double Profiler::GetFPS() const {
		if (frameCount == 0) {
			return 0.0;
		}
		size_t n = frameCount;
		double sum = 0.0;
		for (size_t i = 0; i < n; ++i) {
			sum += frameTimes[i];
		}
		double avgMs = sum / static_cast<double>(n);
		if (avgMs <= 0.0) {
			return 0.0;
		}
		return 1000.0 / avgMs;
	}

	double Profiler::GetUPS() const {
		if (updateCount == 0) {
			return 0.0;
		}
		size_t n = updateCount;
		double sum = 0.0;
		for (size_t i = 0; i < n; ++i) {
			sum += updateTimes[i];
		}
		double avgMs = sum / static_cast<double>(n);
		if (avgMs <= 0.0) {
			return 0.0;
		}
		return 1000.0 / avgMs;
	}

	double Profiler::GetFrameTimeMs() const {
		if (frameCount == 0) {
			return 0.0;
		}
		return frameTimes[(frameWriteIdx + kHistorySize - 1) % kHistorySize];
	}

	double Profiler::GetUpdateTimeMs() const {
		if (updateCount == 0) {
			return 0.0;
		}
		return updateTimes[(updateWriteIdx + kHistorySize - 1) % kHistorySize];
	}

	double Profiler::GetFrameTimeMinMs() const {
		if (frameCount == 0) {
			return 0.0;
		}
		size_t n = frameCount;
		double minVal = frameTimes[0];
		for (size_t i = 1; i < n; ++i) {
			if (frameTimes[i] < minVal) {
				minVal = frameTimes[i];
			}
		}
		return minVal;
	}

	double Profiler::GetFrameTimeMaxMs() const {
		if (frameCount == 0) {
			return 0.0;
		}
		size_t n = frameCount;
		double maxVal = frameTimes[0];
		for (size_t i = 1; i < n; ++i) {
			if (frameTimes[i] > maxVal) {
				maxVal = frameTimes[i];
			}
		}
		return maxVal;
	}

	double Profiler::GetFrameTimeAvgMs() const {
		if (frameCount == 0) {
			return 0.0;
		}
		size_t n = frameCount;
		double sum = 0.0;
		for (size_t i = 0; i < n; ++i) {
			sum += frameTimes[i];
		}
		return sum / static_cast<double>(n);
	}

	double Profiler::GetFrameTimeAtHistoryIndex(size_t idx) const {
		if (idx >= frameCount) {
			return 0.0;
		}
		size_t pos;
		if (frameCount < kHistorySize) {
			pos = idx;
		} else {
			pos = (frameWriteIdx + idx) % kHistorySize;
		}
		return frameTimes[pos];
	}

}
