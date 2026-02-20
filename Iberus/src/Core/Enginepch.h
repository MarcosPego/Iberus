#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)  // class needs dll-interface (STL members in exported types)
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <filesystem>

#include <string>
#include <vector>
#include <unordered_map>
#include <map>

#include <typeindex>

#include <any>

#define _USE_MATH_DEFINES
#include <math.h>

// TODO(MPP) Fix enginepch generation
//#include <FastNoise.h>

#include <iomanip>
#include <cassert>

#ifdef IB_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include <glew.h>
#include <glfw3.h>

// TODO(MPP) Fix enginepch generation
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>

/// Common utils

#include "Log.h"
#include "Buffer.h"