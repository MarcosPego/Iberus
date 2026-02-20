#pragma once

#include "Core.h"

#include <functional>
#include <memory>
#include <string>
#include <thread>

namespace Iberus {

	/// Watches a directory tree for changes. On change, invokes callback.
	/// Platform-specific: Windows uses ReadDirectoryChangesW.
	class IBERUS_API FileWatcher {
	public:
		using Callback = std::function<void(const std::string& changedPath)>;

		explicit FileWatcher(const std::string& path);
		~FileWatcher();

		void SetCallback(Callback cb) { callback = std::move(cb); }
		void Start();
		void Stop();
		bool IsRunning() const { return running; }

	private:
		void WatchThread();

		std::string path;
		Callback callback;
		std::unique_ptr<std::thread> thread;
		bool running{ false };
#ifdef IB_PLATFORM_WINDOWS
		void* directoryHandle{ nullptr };
		void* overlappedBuffer{ nullptr };
#endif
	};

}
