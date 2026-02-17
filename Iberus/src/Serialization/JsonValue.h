#pragma once

#include "Core.h"
#include "Buffer.h"

#include <memory>
#include <string>
#include <vector>

namespace Iberus {

	struct JsonValueImpl;

	class IBERUS_API JsonValue {
	public:
		JsonValue();
		~JsonValue();
		JsonValue(JsonValue&&) noexcept;
		JsonValue& operator=(JsonValue&&) noexcept;
		JsonValue(const JsonValue&);
		JsonValue& operator=(const JsonValue&);

		bool IsValid() const;
		bool IsNull() const;
		bool IsBool() const;
		bool IsNumber() const;
		bool IsString() const;
		bool IsObject() const;
		bool IsArray() const;

		bool AsBool() const;
		int AsInt() const;
		int64_t AsInt64() const;
		float AsFloat() const;
		double AsDouble() const;
		std::string AsString() const;

		JsonValue operator[](const std::string& key) const;
		JsonValue operator[](size_t index) const;
		bool Contains(const std::string& key) const;
		std::vector<std::string> Keys() const;
		size_t Size() const;

		void Set(const std::string& key, const JsonValue& value);
		void Set(const std::string& key, const std::string& value);
		void Set(const std::string& key, int value);
		void Set(const std::string& key, int64_t value);
		void Set(const std::string& key, float value);
		void Set(const std::string& key, double value);
		void Set(const std::string& key, bool value);
		void Set(size_t index, const JsonValue& value);
		void Push(const JsonValue& value);

		static JsonValue Parse(const Buffer& buffer);
		static JsonValue ParseFile(const std::string& path);
		Buffer ToBuffer(bool pretty = true) const;
		bool WriteFile(const std::string& path, bool pretty = true) const;

		static JsonValue Object();
		static JsonValue Array();
		static JsonValue String(const std::string& value);
		static JsonValue Number(double value);
		static JsonValue Bool(bool value);

	private:
		explicit JsonValue(std::unique_ptr<JsonValueImpl> impl);
		std::unique_ptr<JsonValueImpl> impl;

		friend bool IBERUS_API operator==(const JsonValue& a, const JsonValue& b);
	};

}
