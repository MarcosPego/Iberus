#pragma once

#include "Core.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Iberus {

	class IBERUS_API Profiler {
	public:
		static Profiler& Instance();

		void RecordFrame(double frameTimeSec);
		void RecordUpdate(double updateTimeSec);

		/// Zone timing for heat map: record time spent in a named zone (e.g. "Update", "Geometry", "Light").
		void RecordZone(const std::string& name, double timeMs);

		double GetFPS() const;
		double GetUPS() const;
		double GetFrameTimeMs() const;
		double GetUpdateTimeMs() const;
		double GetFrameTimeMinMs() const;
		double GetFrameTimeMaxMs() const;
		double GetFrameTimeAvgMs() const;
		size_t GetFrameTimeHistoryCount() const { return frameCount; }
		double GetFrameTimeAtHistoryIndex(size_t idx) const;

		/// Heat map: smoothed avg time per zone (for stable display). Returns (name, avgMs).
		std::vector<std::pair<std::string, double>> GetZoneHeatMap() const;
		/// Enable/disable heat map recording (keeps last values when disabled).
		void SetHeatMapEnabled(bool enabled) { heatMapEnabled = enabled; }
		bool IsHeatMapEnabled() const { return heatMapEnabled; }

	private:
		Profiler();

		static constexpr size_t kHistorySize = 120;
		static constexpr double kZoneSmoothing = 0.1;

		double frameTimes[kHistorySize];
		double updateTimes[kHistorySize];
		size_t frameWriteIdx;
		size_t updateWriteIdx;
		size_t frameCount;
		size_t updateCount;

		struct ZoneData {
			double avgMs{ 0.0 };
			bool hasData{ false };
		};
		std::unordered_map<std::string, ZoneData> zones;
		mutable std::vector<std::pair<std::string, double>> zoneHeatMapCache;
		mutable bool zoneCacheDirty{ true };
		bool heatMapEnabled{ true };
	};

}
