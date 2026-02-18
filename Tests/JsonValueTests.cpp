#include <iostream>

#include "Log.h"
#include "Serialization/JsonValue.h"
#include "Scene/Resources/Buffer.h"

#define TEST_ASSERT(cond, msg) do { \
	if (!(cond)) { \
		std::cerr << "FAIL: " << (msg) << " (at " << __FILE__ << ":" << __LINE__ << ")\n"; \
		return 1; \
	} \
} while(0)

int RunJsonValueTests() {
	int failures = 0;

	// Object creation
	auto obj = Iberus::JsonValue::Object();
	TEST_ASSERT(obj.IsValid() && obj.IsObject(), "JsonValue::Object()");

	// Set/Get primitives
	obj.Set("key", "value");
	TEST_ASSERT(obj["key"].IsString() && obj["key"].AsString() == "value", "Set/Get string");
	obj.Set("num", 42);
	TEST_ASSERT(obj["num"].IsNumber() && obj["num"].AsInt() == 42, "Set/Get int");
	obj.Set("flag", true);
	TEST_ASSERT(obj["flag"].IsBool() && obj["flag"].AsBool() == true, "Set/Get bool");

	// Array
	auto arr = Iberus::JsonValue::Array();
	TEST_ASSERT(arr.IsValid() && arr.IsArray(), "JsonValue::Array()");
	arr.Push(Iberus::JsonValue::Number(1));
	arr.Push(Iberus::JsonValue::Number(2));
	TEST_ASSERT(arr.Size() == 2, "Array size");
	TEST_ASSERT(arr[0].AsInt() == 1 && arr[1].AsInt() == 2, "Array elements");

	// Round-trip: serialize to buffer, parse back
	Iberus::Buffer buf = obj.ToBuffer(false);
	TEST_ASSERT(!buf.Invalid(), "ToBuffer produces valid buffer");
	auto parsed = Iberus::JsonValue::Parse(buf);
	TEST_ASSERT(parsed.IsValid(), "Parse produces valid value");
	TEST_ASSERT(parsed["key"].AsString() == "value", "Round-trip string");
	TEST_ASSERT(parsed["num"].AsInt() == 42, "Round-trip int");

	// Invalid/missing key
	TEST_ASSERT(!obj["nonexistent"].IsValid() || obj["nonexistent"].IsNull(), "Missing key");

	std::cout << "JsonValueTests: passed\n";
	return 0;
}
