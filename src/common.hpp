#pragma once

#include <SDKDDKVer.h>
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include <cinttypes>
#include <cstddef>
#include <cstdint>

#include <chrono>
#include <ctime>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <atomic>
#include <mutex>
#include <thread>

#include <memory>
#include <new>

#include <sstream>
#include <string>
#include <string_view>

#include <algorithm>
#include <functional>
#include <utility>

#include <stack>
#include <vector>

#include <typeinfo>
#include <type_traits>

#include <exception>
#include <stdexcept>

#include <any>
#include <optional>
#include <variant>

#include <regex>
#include <tlhelp32.h>

#include <nlohmann/json.hpp>

#include "settings.hpp"
#include "logger.hpp"

// #define RESIDENT_EVIL_4

// #ifdef DEV
// #define WINDOW_CLASS "gfx_test"
// #define DX9_TEST "Renderer: [DirectX9], Input: [Raw input], 64 bits"
// #define DX11_TEST "Renderer: [DirectX11], Input: [Raw input], 64 bits"
// #define DX12_TEST "Renderer: [DirectX12], Input: [Raw input], 64 bits"
// #define DX9_TEST_32 "Renderer: [DirectX9], Input: [Raw input], 32 bits"
// #define DX11_TEST_32 "Renderer: [DirectX11], Input: [Raw input], 32 bits"
// #define DX12_TEST_32 "Renderer: [DirectX12], Input: [Raw input], 32 bits"
// #else
// #define WINDOW_CLASS "Direct3DWindowClass"
// #endif

#ifdef RESIDENT_EVIL_4
	#define WINDOW_CLASS nullptr
	#define WINDOW_NAME "Resident Evil 4"
#elif defined(VALKYRIE_ELYSIUM)
	#define WINDOW_CLASS "UnrealWindow"
	#define WINDOW_NAME nullptr
#else
	#define WINDOW_CLASS "gfx_test"
	#define WINDOW_NAME "Renderer: [DirectX12], Input: [Raw input], 64 bits"
#endif

namespace big
{
	using namespace std::chrono_literals;
	
	template <typename T>
	using comptr = Microsoft::WRL::ComPtr<T>;

	inline HMODULE g_hmodule{};
	inline HANDLE g_main_thread{};
	inline DWORD g_main_thread_id{};
	inline std::atomic_bool g_running{ true };
}
