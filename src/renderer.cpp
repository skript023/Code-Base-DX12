#include "gui.hpp"
#include "logger.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include "fonts/font_list.hpp"
#include "fonts/icon_list.hpp"

#include <imgui.h>
#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_win32.h>
#include <imgui_internal.h>

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//CW.exe+2D4E21610
namespace big
{
	renderer::renderer()
	{
		g_renderer = this;
	}

	renderer::~renderer()
	{
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX12_Shutdown();
		ImGui::DestroyContext();

		m_d3d_device->Release();
		m_command_queue->Release();
		m_command_list->Release();

		g_renderer = nullptr;
	}

	void renderer::on_present(IDXGISwapChain3* swapchain)
	{
		if (g_gui.m_opened)
		{
			ImGui::GetIO().MouseDrawCursor = true;
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
		else
		{
			ImGui::GetIO().MouseDrawCursor = false;
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		g_gui.dx_on_tick();

		if (g_gui.m_opened)
		{
			g_gui.dx_on_opened();
		}

		_FrameContext& current_frame_context = m_frame_context[swapchain->GetCurrentBackBufferIndex()];
		current_frame_context.m_command_allocator->Reset();

		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = current_frame_context.m_resource;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		m_command_list->Reset(current_frame_context.m_command_allocator, nullptr);
		m_command_list->ResourceBarrier(1, &barrier);
		m_command_list->OMSetRenderTargets(1, &current_frame_context.m_descriptor_handle, FALSE, nullptr);
		m_command_list->SetDescriptorHeaps(1, &m_descriptor_heap_render);

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_command_list);

		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		m_command_list->ResourceBarrier(1, &barrier);
		m_command_list->Close();
		m_command_queue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&m_command_list));
	}

	bool renderer::init(IDXGISwapChain3* swapchain)
	{
		if (!m_init)
		{
			if (SUCCEEDED(swapchain->GetDevice(__uuidof(ID3D12Device), (void**)&m_d3d_device)))
			{
				swapchain->GetDesc(&m_swapchain_desc);
				m_swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
				m_swapchain_desc.OutputWindow = g_pointers->m_hwnd;
				m_swapchain_desc.Windowed = ((GetWindowLongPtr(g_pointers->m_hwnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

				m_buffer_count = m_swapchain_desc.BufferCount;
				m_frame_context = new _FrameContext[m_buffer_count];

				D3D12_DESCRIPTOR_HEAP_DESC descriptor_renderer = {};
				descriptor_renderer.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				descriptor_renderer.NumDescriptors = m_buffer_count;
				descriptor_renderer.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

				if (m_d3d_device->CreateDescriptorHeap(&descriptor_renderer, IID_PPV_ARGS(&m_descriptor_heap_render)) != S_OK)
				{
					LOG(WARNING) << "Error Create Descriptor Heap";

					return false;
				}

				ID3D12CommandAllocator* allocator;
				if (m_d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)) != S_OK)
				{
					LOG(WARNING) << "Error Create Command Allocator";

					return false;
				}

				for (size_t i = 0; i < m_buffer_count; i++) 
				{
					m_frame_context[i].m_command_allocator = allocator;
				}

				if (m_d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, NULL, IID_PPV_ARGS(&m_command_list)) != S_OK || m_command_list->Close() != S_OK)
				{
					LOG(WARNING) << "Error Create Command List";

					return false;
				}

				D3D12_DESCRIPTOR_HEAP_DESC DescriptorBackBuffers;
				DescriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				DescriptorBackBuffers.NumDescriptors = m_buffer_count;
				DescriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				DescriptorBackBuffers.NodeMask = 1;

				if (m_d3d_device->CreateDescriptorHeap(&DescriptorBackBuffers, IID_PPV_ARGS(&m_descriptor_heap_backbuffer)) != S_OK)
				{
					LOG(WARNING) << "Error Create Descriptor Heap";

					return false;
				}

				const auto rvt_descriptor_size = m_d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
				m_rvt_handle = m_descriptor_heap_backbuffer->GetCPUDescriptorHandleForHeapStart();

				for (size_t i = 0; i < m_buffer_count; i++)
				{
					ID3D12Resource* m_back_buffer = nullptr;
					m_frame_context[i].m_descriptor_handle = m_rvt_handle;
					swapchain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_back_buffer);
					m_d3d_device->CreateRenderTargetView(m_back_buffer, nullptr, m_rvt_handle);
					m_frame_context[i].m_resource = m_back_buffer;
					m_rvt_handle.ptr += rvt_descriptor_size;
				}

				this->imgui_init();

			}

			this->m_init = true;

			return true;
		}

		return false;
	}

	void renderer::imgui_init()
	{
		auto file_path = std::filesystem::path(std::getenv("appdata"));
		file_path /= "Ellohim";
		if (!std::filesystem::exists(file_path))
		{
			std::filesystem::create_directory(file_path);
		}
		else if (!std::filesystem::is_directory(file_path))
		{
			std::filesystem::remove(file_path);
			std::filesystem::create_directory(file_path);
		}
		file_path /= "imgui.ini";

		ImGuiContext* ctx = ImGui::CreateContext();

		static std::string path = file_path.make_preferred().string();
		ctx->IO.IniFilename = path.c_str();

		ImGui_ImplDX12_Init(m_d3d_device, m_buffer_count, DXGI_FORMAT_R8G8B8A8_UNORM, m_descriptor_heap_render, m_descriptor_heap_render->GetCPUDescriptorHandleForHeapStart(), m_descriptor_heap_render->GetGPUDescriptorHandleForHeapStart());
		ImGui_ImplWin32_Init(g_pointers->m_hwnd);
		ImGui_ImplDX12_CreateDeviceObjects();

		ImFontConfig font_cfg{};
		font_cfg.FontDataOwnedByAtlas = false;
		std::strcpy(font_cfg.Name, "Rubik");

		m_font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(font_rubik), sizeof(font_rubik), 14.f, &font_cfg);
		merge_icon_with_latest_font(14.f, false);

		g_settings->window.font_sub_title = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(font_rubik), sizeof(font_rubik), 18.f, &font_cfg);
		merge_icon_with_latest_font(16.f, false);

		g_settings->window.font_title = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(font_rubik), sizeof(font_rubik), 24.f, &font_cfg);
		merge_icon_with_latest_font(17.f, false);

		m_monospace_font = ImGui::GetIO().Fonts->AddFontDefault();

		g_gui.dx_init();

		LOG(INFO) << "Swapchain initialized.";
	}

	void renderer::pre_reset()
	{
		ImGui_ImplDX12_InvalidateDeviceObjects();
		for (size_t i{}; i != m_swapchain_desc.BufferCount; ++i)
		{
			m_frame_context[i].m_resource->Release();
		}
	}

	void renderer::post_reset(IDXGISwapChain3* this_)
	{
		ImGui_ImplDX12_CreateDeviceObjects();
		const auto RTVDescriptorSize{m_d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)};
		D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle{m_descriptor_heap_backbuffer->GetCPUDescriptorHandleForHeapStart()};
		for (size_t i{}; i != m_swapchain_desc.BufferCount; ++i)
		{
			comptr<ID3D12Resource> backbuffer{};
			m_frame_context[i].m_descriptor_handle = RTVHandle;
			this_->GetBuffer(i, __uuidof(ID3D12Resource), (void**)backbuffer.GetAddressOf());
			m_d3d_device->CreateRenderTargetView(backbuffer.Get(), nullptr, RTVHandle);
			m_frame_context[i].m_resource = backbuffer.Get();
			RTVHandle.ptr += RTVDescriptorSize;
		}
	}

	void renderer::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (msg == WM_KEYUP && wparam == VK_INSERT)
		{
			//Persist and restore the cursor position between menu instances.
			static POINT cursor_coords{};
			if (g_gui.m_opened)
			{
				GetCursorPos(&cursor_coords);
			}
			else if (cursor_coords.x + cursor_coords.y != 0)
			{
				SetCursorPos(cursor_coords.x, cursor_coords.y);
			}

			g_gui.m_opened ^= true;
		}
		if (msg == WM_QUIT)
		{
			g_running = false;
		}
			

		if (g_gui.m_opened)
		{
			ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
		}
	}

	void renderer::merge_icon_with_latest_font(float font_size, bool FontDataOwnedByAtlas)
	{
		static const ImWchar icons_ranges[3] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		icons_config.FontDataOwnedByAtlas = FontDataOwnedByAtlas;

		g_settings->window.font_icon = ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)font_icons, sizeof(font_icons), font_size, &icons_config, icons_ranges);
	}
}
