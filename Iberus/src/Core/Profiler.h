#pragma once

#include "Core.h"

namespace Iberus {

	class IBERUS_API Profiler {
	public:
		static Profiler& Instance();

		void RecordFrame(double frameTimeSec);
		void RecordUpdate(double updateTimeSec);

		double GetFPS() const;
		double GetUPS() const;
		double GetFrameTimeMs() const;
		double GetUpdateTimeMs() const;
		double GetFrameTimeMinMs() const;
		double GetFrameTimeMaxMs() const;
		double GetFrameTimeAvgMs() const;
		size_t GetFrameTimeHistoryCount() const { return frameCount; }
		double GetFrameTimeAtHistoryIndex(size_t idx) const;

	private:
		Profiler();

		static constexpr size_t kHistorySize = 120;
		double frameTimes[kHistorySize];
		double updateTimes[kHistorySize];
		size_t frameWriteIdx;
		size_t updateWriteIdx;
		size_t frameCount;
		size_t updateCount;
	};

}
