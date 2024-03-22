#include "common.hpp"
#include "logger.hpp"
#include "pointers.hpp"
#include "memory/all.hpp"

namespace big
{
	pointers::pointers() : m_base_address(memory::module(nullptr).begin().as<uint64_t>()), m_resolution(new ScreenResolution(1920, 1080))
	{
		memory::pattern_batch main_batch;

		if (!this->get_swapchain())
			LOG(WARNING) << "Failed get swapchain";

		// m_resolution = (ScreenResolution*)(m_base_address + 0x3F54D58);//3F54D60
		
		// main_batch.add("Return Address", "FF 23", [this](memory::handle ptr)
		// {
		// 	m_return_address = ptr.as<void*>();
		// });
		
		main_batch.run(memory::module(nullptr));

		this->m_hwnd = FindWindow(WINDOW_CLASS, WINDOW_NAME);
		if (!this->m_hwnd)
			throw std::runtime_error("Failed to find the game's window.");

		g_pointers = this;
	}

	pointers::~pointers()
	{
		g_pointers = nullptr;
	}

	bool pointers::get_swapchain()
	{
		WNDCLASSEX window_class;
		window_class.cbSize = sizeof(WNDCLASSEX);
		window_class.style = CS_HREDRAW | CS_VREDRAW;
		window_class.lpfnWndProc = DefWindowProc;
		window_class.cbClsExtra = 0;
		window_class.cbWndExtra = 0;
		window_class.hInstance = GetModuleHandle(NULL);
		window_class.hIcon = NULL;
		window_class.hCursor = NULL;
		window_class.hbrBackground = NULL;
		window_class.lpszMenuName = NULL;
		window_class.lpszClassName = "Kiero";
		window_class.hIconSm = NULL;

		::RegisterClassEx(&window_class);

		this->m_window = ::CreateWindow(window_class.lpszClassName, "Kiero DirectX Window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, window_class.hInstance, NULL);

		if (this->m_window == NULL)
		{
			return false;
		}

		HMODULE d3d12_module = GetModuleHandle("d3d12.dll");
		HMODULE dxgi_module = GetModuleHandle("dxgi.dll");
		if (d3d12_module == NULL || dxgi_module == NULL)
		{
			::DestroyWindow(this->m_window);
			::UnregisterClass(window_class.lpszClassName, window_class.hInstance);

			LOG(WARNING) << "Failed get dx12 & dxgi module";
			
			return false;
		}

		void* create_dxgi_factory = GetProcAddress(dxgi_module, "CreateDXGIFactory");
		if (create_dxgi_factory == NULL) 
		{
			::DestroyWindow(this->m_window);
			::UnregisterClass(window_class.lpszClassName, window_class.hInstance);

			return false;
		}

		IDXGIFactory* factory;
		if (((long(__stdcall*)(const IID&, void**))(create_dxgi_factory))(__uuidof(IDXGIFactory), (void**)&factory) < 0) 
		{
			::DestroyWindow(this->m_window);
			::UnregisterClass(window_class.lpszClassName, window_class.hInstance);

			return false;
		}

		IDXGIAdapter* adapter;
		if (factory->EnumAdapters(0, &adapter) == DXGI_ERROR_NOT_FOUND) 
		{
			::DestroyWindow(this->m_window);
			::UnregisterClass(window_class.lpszClassName, window_class.hInstance);

			return false;
		}

		void* d3d12_create_device = GetProcAddress(d3d12_module, "D3D12CreateDevice");
		if (d3d12_create_device == NULL) 
		{
			::DestroyWindow(this->m_window);
			::UnregisterClass(window_class.lpszClassName, window_class.hInstance);

			return false;
		}

		ID3D12Device* device;
		if (((long(__stdcall*)(IUnknown*, D3D_FEATURE_LEVEL, const IID&, void**))(D3D12CreateDevice))(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&device) < 0) 
		{
			::DestroyWindow(this->m_window);
			::UnregisterClass(window_class.lpszClassName, window_class.hInstance);

			return false;
		}

		D3D12_COMMAND_QUEUE_DESC queue_desc;
		queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queue_desc.Priority = 0;
		queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queue_desc.NodeMask = 0;

		ID3D12CommandQueue* command_queue;
		if (device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), (void**)&command_queue) < 0) 
		{
			::DestroyWindow(this->m_window);
			::UnregisterClass(window_class.lpszClassName, window_class.hInstance);

			return false;
		}

		ID3D12CommandAllocator* command_allocator;
		if (device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&command_allocator) < 0) 
		{
			::DestroyWindow(this->m_window);
			::UnregisterClass(window_class.lpszClassName, window_class.hInstance);

			return false;
		}

		ID3D12GraphicsCommandList* command_list;
		if (device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&command_list) < 0) 
		{
			::DestroyWindow(this->m_window);
			::UnregisterClass(window_class.lpszClassName, window_class.hInstance);

			return false;
		}

		DXGI_RATIONAL refresh_rate;
		refresh_rate.Numerator = 60;
		refresh_rate.Denominator = 1;

		DXGI_MODE_DESC buffer_desc;
		buffer_desc.Width = 100;
		buffer_desc.Height = 100;
		buffer_desc.RefreshRate = refresh_rate;
		buffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		buffer_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		buffer_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		DXGI_SAMPLE_DESC sample_desc;
		sample_desc.Count = 1;
		sample_desc.Quality = 0;

		DXGI_SWAP_CHAIN_DESC swapchain_desc = {};
		swapchain_desc.BufferDesc = buffer_desc;
		swapchain_desc.SampleDesc = sample_desc;
		swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchain_desc.BufferCount = 2;
		swapchain_desc.OutputWindow = this->m_window;
		swapchain_desc.Windowed = 1;
		swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		if (factory->CreateSwapChain(command_queue, &swapchain_desc, &this->m_swapchain) < 0) {
			::DestroyWindow(this->m_window);
			::UnregisterClass(window_class.lpszClassName, window_class.hInstance);

			return false;
		}

		m_swapchain_methods = (void**)::calloc(150, sizeof(void*));
		::memcpy(this->m_swapchain_methods, *(void***)device, 44 * sizeof(uint64_t));
		::memcpy(this->m_swapchain_methods + 44, *(void***)command_queue, 19 * sizeof(uint64_t));
		::memcpy(this->m_swapchain_methods + 44 + 19, *(void***)command_allocator, 9 * sizeof(uint64_t));
		::memcpy(this->m_swapchain_methods + 44 + 19 + 9, *(void***)command_list, 60 * sizeof(uint64_t));
		::memcpy(this->m_swapchain_methods + 44 + 19 + 9 + 60, *(void***)this->m_swapchain, 18 * sizeof(uint64_t));
		LOG(INFO) << m_swapchain_methods[140];

		device->Release();
		device = NULL;

		command_queue->Release();
		command_queue = NULL;

		command_allocator->Release();
		command_allocator = NULL;

		command_list->Release();
		command_list = NULL;

		m_swapchain->Release();
		m_swapchain = NULL;

		::DestroyWindow(this->m_window);
		::UnregisterClass(window_class.lpszClassName, window_class.hInstance);

		return true;
	}
}