#include "Enginepch.h"
#include "FileWatcher.h"

#ifdef IB_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include <chrono>

namespace Iberus {

#ifdef IB_PLATFORM_WINDOWS
	// Buffer size for ReadDirectoryChangesW (must be DWORD-aligned)
	static constexpr DWORD kBufferSize = 4096;

	FileWatcher::FileWatcher(const std::string& path) : path(path) {
	}

	FileWatcher::~FileWatcher() {
		Stop();
	}

	void FileWatcher::Stop() {
		running = false;
		if (thread && thread->joinable()) {
			thread->join();
			thread.reset();
		}
		if (directoryHandle && directoryHandle != INVALID_HANDLE_VALUE) {
			CloseHandle(static_cast<HANDLE>(directoryHandle));
			directoryHandle = nullptr;
		}
		if (overlappedBuffer) {
			delete[] static_cast<char*>(overlappedBuffer);
			overlappedBuffer = nullptr;
		}
	}

	void FileWatcher::Start() {
		if (running) {
			return;
		}
		running = true;
		thread = std::make_unique<std::thread>(&FileWatcher::WatchThread, this);
	}

	void FileWatcher::WatchThread() {
		std::wstring wpath(path.begin(), path.end());
		HANDLE hDir = CreateFileW(
			wpath.c_str(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			nullptr
		);

		if (hDir == INVALID_HANDLE_VALUE) {
			running = false;
			return;
		}

		directoryHandle = hDir;
		overlappedBuffer = new char[kBufferSize];
		OVERLAPPED overlapped{};
		overlapped.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

		while (running) {
			overlapped.Offset = 0;
			overlapped.OffsetHigh = 0;
			ResetEvent(overlapped.hEvent);

			DWORD bytesReturned = 0;
			BOOL result = ReadDirectoryChangesW(
				hDir,
				overlappedBuffer,
				kBufferSize,
				TRUE,
				FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE,
				&bytesReturned,
				&overlapped,
				nullptr
			);

			if (!result) {
				Sleep(500);
				continue;
			}

			// Wait for completion with timeout to allow stopping
			DWORD waitResult = WaitForSingleObject(overlapped.hEvent, 500);
			if (waitResult == WAIT_TIMEOUT || !running) {
				continue;
			}
			if (waitResult != WAIT_OBJECT_0) {
				continue;
			}

			GetOverlappedResult(hDir, &overlapped, &bytesReturned, FALSE);

			if (callback && bytesReturned > 0) {
				FILE_NOTIFY_INFORMATION* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(overlappedBuffer);
				do {
					std::wstring wname(info->FileName, info->FileNameLength / sizeof(wchar_t));
					std::string name(wname.begin(), wname.end());
					std::string fullPath = (std::filesystem::path(path) / name).string();
					callback(fullPath);
					if (info->NextEntryOffset == 0) {
						break;
					}
					info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<char*>(info) + info->NextEntryOffset);
				} while (running);
			}
		}

		CloseHandle(overlapped.hEvent);
		CloseHandle(hDir);
		directoryHandle = nullptr;
		delete[] static_cast<char*>(overlappedBuffer);
		overlappedBuffer = nullptr;
	}

#else
	// Stub for non-Windows: poll-based fallback
	FileWatcher::FileWatcher(const std::string& path) : path(path) {
	}

	FileWatcher::~FileWatcher() {
		Stop();
	}

	void FileWatcher::Stop() {
		running = false;
		if (thread && thread->joinable()) {
			thread->join();
		}
		thread.reset();
	}

	void FileWatcher::Start() {
		if (running) {
			return;
		}
		running = true;
		thread = std::make_unique<std::thread>(&FileWatcher::WatchThread, this);
	}

	void FileWatcher::WatchThread() {
		auto lastCheck = std::filesystem::last_write_time(path);
		while (running) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			try {
				auto now = std::filesystem::last_write_time(path);
				if (now != lastCheck && callback) {
					lastCheck = now;
					callback(path);
				}
			} catch (...) {}
		}
	}
#endif
}
