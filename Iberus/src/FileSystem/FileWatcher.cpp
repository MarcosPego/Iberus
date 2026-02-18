#include "Enginepch.h"
#include "FileWatcher.h"

#include <string_view>

#ifdef IB_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include <chrono>

namespace Iberus {

#ifdef IB_PLATFORM_WINDOWS
	// Buffer size for ReadDirectoryChangesW (must be DWORD-aligned)
	static constexpr DWORD kBufferSize = 4096;

	static std::wstring Utf8ToWide(const std::string& str) {
		if (str.empty()) {
			return {};
		}
		int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
		if (size <= 0) {
			return {};
		}
		std::wstring result(static_cast<size_t>(size), L'\0');
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), result.data(), size);
		return result;
	}

	static std::string WideToUtf8(const std::wstring& wstr) {
		if (wstr.empty()) {
			return {};
		}
		int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
		if (size <= 0) {
			return {};
		}
		std::string result(static_cast<size_t>(size), '\0');
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), result.data(), size, nullptr, nullptr);
		return result;
	}

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
		std::wstring wpath = Utf8ToWide(path);
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
					std::string name = WideToUtf8(wname);
					std::u8string_view pathU8(reinterpret_cast<const char8_t*>(path.data()), path.size());
					std::u8string_view nameU8(reinterpret_cast<const char8_t*>(name.data()), name.size());
					auto u8Full = (std::filesystem::path(pathU8) / std::filesystem::path(nameU8)).u8string();
					std::string fullPath(reinterpret_cast<const char*>(u8Full.data()), u8Full.size());
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
