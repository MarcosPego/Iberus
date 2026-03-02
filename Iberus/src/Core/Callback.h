#pragma once

#include "Core.h"

#include <functional>
#include <vector>
#include <cstdint>

namespace Iberus {

	/// Multi-subscriber callback: subscribe with a callable, invoke to run all subscribers.
	/// subscribe(fn) = repeatable; subscribe(fn, count) = fire count times then auto-unsubscribe.
	template <typename... Args>
	class Callback {
	public:
		using Function = std::function<void(Args...)>;

		Callback() = default;
		~Callback() = default;

		Callback(const Callback&) = delete;
		Callback& operator=(const Callback&) = delete;

		/// Subscribe repeatable (stays until unsubscribe or teardown).
		uint32_t subscribe(Function f) {
			return subscribeImpl(std::move(f), -1);
		}

		/// Subscribe N-shot: fires at most count times, then auto-unsubscribes.
		uint32_t subscribe(Function f, int count) {
			if (count <= 0) {
				return subscribeImpl(std::move(f), -1);
			}
			return subscribeImpl(std::move(f), count);
		}

		void unsubscribe(uint32_t id) {
			for (size_t i = 0; i < subscribers.size(); ++i) {
				if (subscribers[i].id == id) {
					subscribers.erase(subscribers.begin() + static_cast<std::ptrdiff_t>(i));
					return;
				}
			}
		}

		void Invoke(Args... args) {
			std::vector<Entry> copy = subscribers;
			for (Entry& e : copy) {
				if (!e.fn) {
					continue;
				}
				e.fn(args...);
				if (e.remaining > 0) {
					--e.remaining;
				}
			}
			// Keep only entries that have not reached 0 (remaining != 0 includes infinite)
			subscribers.clear();
			for (Entry& e : copy) {
				if (e.remaining != 0) {
					subscribers.push_back(std::move(e));
				}
			}
		}

	private:
		struct Entry {
			Function fn;
			int remaining; // < 0 = infinite
			uint32_t id;
		};

		uint32_t subscribeImpl(Function f, int count) {
			uint32_t id = nextId++;
			subscribers.push_back(Entry{ std::move(f), count, id });
			return id;
		}

		std::vector<Entry> subscribers;
		uint32_t nextId{ 1 };
	};

	/// No-arg callback alias for "Callback fooCallback;"
	using CallbackNoArgs = Callback<>;

}
