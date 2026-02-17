#include "Enginepch.h"
#include "JsonValue.h"
#include "FileSystem/FileSystem.h"
#include "Log.h"

#include <nlohmann/json.hpp>

#include <cstring>
#include <fstream>

namespace Iberus {

	struct JsonValueImpl {
		nlohmann::json j;
	};

	JsonValue::JsonValue() : impl(nullptr) {
	}

	JsonValue::~JsonValue() = default;

	JsonValue::JsonValue(JsonValue&&) noexcept = default;
	JsonValue& JsonValue::operator=(JsonValue&&) noexcept = default;

	JsonValue::JsonValue(const JsonValue& other) : impl(other.impl ? std::make_unique<JsonValueImpl>(nlohmann::json(other.impl->j)) : nullptr) {
	}

	JsonValue& JsonValue::operator=(const JsonValue& other) {
		if (this != &other) {
			if (other.impl) {
				impl = std::make_unique<JsonValueImpl>(nlohmann::json(other.impl->j));
			} else {
				impl.reset();
			}
		}
		return *this;
	}

	JsonValue::JsonValue(std::unique_ptr<JsonValueImpl> impl_) : impl(std::move(impl_)) {
	}

	bool JsonValue::IsValid() const {
		return impl != nullptr;
	}

	bool JsonValue::IsNull() const {
		return impl && impl->j.is_null();
	}

	bool JsonValue::IsBool() const {
		return impl && impl->j.is_boolean();
	}

	bool JsonValue::IsNumber() const {
		return impl && impl->j.is_number();
	}

	bool JsonValue::IsString() const {
		return impl && impl->j.is_string();
	}

	bool JsonValue::IsObject() const {
		return impl && impl->j.is_object();
	}

	bool JsonValue::IsArray() const {
		return impl && impl->j.is_array();
	}

	bool JsonValue::AsBool() const {
		return impl ? impl->j.get<bool>() : false;
	}

	int JsonValue::AsInt() const {
		return impl ? impl->j.get<int>() : 0;
	}

	int64_t JsonValue::AsInt64() const {
		return impl ? impl->j.get<int64_t>() : 0;
	}

	float JsonValue::AsFloat() const {
		return impl ? static_cast<float>(impl->j.get<double>()) : 0.0f;
	}

	double JsonValue::AsDouble() const {
		return impl ? impl->j.get<double>() : 0.0;
	}

	std::string JsonValue::AsString() const {
		return impl ? impl->j.get<std::string>() : std::string();
	}

	JsonValue JsonValue::operator[](const std::string& key) const {
		if (!impl || !impl->j.is_object() || !impl->j.contains(key)) {
			return JsonValue();
		}
		return JsonValue(std::make_unique<JsonValueImpl>(impl->j[key]));
	}

	JsonValue JsonValue::operator[](size_t index) const {
		if (!impl || !impl->j.is_array() || index >= impl->j.size()) {
			return JsonValue();
		}
		return JsonValue(std::make_unique<JsonValueImpl>(impl->j[index]));
	}

	bool JsonValue::Contains(const std::string& key) const {
		return impl && impl->j.is_object() && impl->j.contains(key);
	}

	std::vector<std::string> JsonValue::Keys() const {
		std::vector<std::string> keys;
		if (!impl || !impl->j.is_object()) {
			return keys;
		}
		for (auto it = impl->j.begin(); it != impl->j.end(); ++it) {
			keys.push_back(it.key());
		}
		return keys;
	}

	size_t JsonValue::Size() const {
		return impl ? impl->j.size() : 0;
	}

	void JsonValue::Set(const std::string& key, const JsonValue& value) {
		if (!impl) {
			impl = std::make_unique<JsonValueImpl>();
			impl->j = nlohmann::json::object();
		}
		if (value.impl) {
			impl->j[key] = value.impl->j;
		} else {
			impl->j[key] = nullptr;
		}
	}

	void JsonValue::Set(const std::string& key, const std::string& value) {
		if (!impl) {
			impl = std::make_unique<JsonValueImpl>();
			impl->j = nlohmann::json::object();
		}
		impl->j[key] = value;
	}

	void JsonValue::Set(const std::string& key, int value) {
		if (!impl) {
			impl = std::make_unique<JsonValueImpl>();
			impl->j = nlohmann::json::object();
		}
		impl->j[key] = value;
	}

	void JsonValue::Set(const std::string& key, int64_t value) {
		if (!impl) {
			impl = std::make_unique<JsonValueImpl>();
			impl->j = nlohmann::json::object();
		}
		impl->j[key] = value;
	}

	void JsonValue::Set(const std::string& key, float value) {
		if (!impl) {
			impl = std::make_unique<JsonValueImpl>();
			impl->j = nlohmann::json::object();
		}
		impl->j[key] = value;
	}

	void JsonValue::Set(const std::string& key, double value) {
		if (!impl) {
			impl = std::make_unique<JsonValueImpl>();
			impl->j = nlohmann::json::object();
		}
		impl->j[key] = value;
	}

	void JsonValue::Set(const std::string& key, bool value) {
		if (!impl) {
			impl = std::make_unique<JsonValueImpl>();
			impl->j = nlohmann::json::object();
		}
		impl->j[key] = value;
	}

	void JsonValue::Set(size_t index, const JsonValue& value) {
		if (!impl) {
			impl = std::make_unique<JsonValueImpl>();
			impl->j = nlohmann::json::array();
		}
		if (!impl->j.is_array()) {
			return;
		}
		while (impl->j.size() <= index) {
			impl->j.push_back(nullptr);
		}
		if (value.impl) {
			impl->j[index] = value.impl->j;
		} else {
			impl->j[index] = nullptr;
		}
	}

	void JsonValue::Push(const JsonValue& value) {
		if (!impl) {
			impl = std::make_unique<JsonValueImpl>();
			impl->j = nlohmann::json::array();
		}
		if (!impl->j.is_array()) {
			return;
		}
		if (value.impl) {
			impl->j.push_back(value.impl->j);
		} else {
			impl->j.push_back(nullptr);
		}
	}

	JsonValue JsonValue::Parse(const Buffer& buffer) {
		if (buffer.Invalid()) {
			return JsonValue();
		}
		try {
			std::string str(reinterpret_cast<const char*>(buffer.GetData()), buffer.GetSize());
			auto j = nlohmann::json::parse(str);
			return JsonValue(std::make_unique<JsonValueImpl>(std::move(j)));
		} catch (const nlohmann::json::exception& e) {
			if (Log::GetCoreLogger()) {
				Log::GetCoreLogger()->error("JsonValue::Parse failed: {}", e.what());
			}
			return JsonValue();
		}
	}

	JsonValue JsonValue::ParseFile(const std::string& path) {
		Buffer buf = FileSystem::GetRawFileBuffer(path);
		return Parse(buf);
	}

	Buffer JsonValue::ToBuffer(bool pretty) const {
		if (!impl) {
			return Buffer();
		}
		std::string str = pretty ? impl->j.dump(2) : impl->j.dump();
		Buffer buf(str.size());
		std::memcpy(buf.GetData(), str.data(), str.size());
		return buf;
	}

	bool JsonValue::WriteFile(const std::string& path, bool pretty) const {
		Buffer buf = ToBuffer(pretty);
		if (buf.Invalid()) {
			return false;
		}
		std::ofstream f(path, std::ios::binary);
		if (!f) {
			return false;
		}
		f.write(reinterpret_cast<const char*>(buf.GetData()), static_cast<std::streamsize>(buf.GetSize()));
		return f.good();
	}

	JsonValue JsonValue::Object() {
		return JsonValue(std::make_unique<JsonValueImpl>(nlohmann::json::object()));
	}

	JsonValue JsonValue::Array() {
		return JsonValue(std::make_unique<JsonValueImpl>(nlohmann::json::array()));
	}

	JsonValue JsonValue::String(const std::string& value) {
		return JsonValue(std::make_unique<JsonValueImpl>(value));
	}

	JsonValue JsonValue::Number(double value) {
		return JsonValue(std::make_unique<JsonValueImpl>(value));
	}

	JsonValue JsonValue::Bool(bool value) {
		return JsonValue(std::make_unique<JsonValueImpl>(value));
	}

	bool IBERUS_API operator==(const JsonValue& a, const JsonValue& b) {
		if (!a.impl && !b.impl) {
			return true;
		}
		if (!a.impl || !b.impl) {
			return false;
		}
		return a.impl->j == b.impl->j;
	}

}
