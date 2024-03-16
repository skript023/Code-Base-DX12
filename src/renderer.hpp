#pragma once
#include "common.hpp"
#include <imgui.h>

namespace big
{
	using dx_callback      = std::function<void()>;
	using wndproc_callback = std::function<void(HWND, UINT, WPARAM, LPARAM)>;

	class renderer
	{
	public:
		explicit renderer();
		~renderer();

		/**
		 * @brief Add a callback function to draw your ImGui content in
		 * 
		 * @param callback Function
		 * @param priority The higher the priority the value the later it gets drawn on top
		 * @return true 
		 * @return false 
		 */
		bool add_dx_callback(dx_callback callback, std::uint32_t priority);
		/**
		 * @brief Add a callback function on wndproc
		 * 
		 * @param callback Function
		 */
		void add_wndproc_callback(wndproc_callback callback);
		bool init(IDirect3DDevice9* swapchain);
		void imgui_init(IDirect3DDevice9* device);

		void on_present();
		void rescale(float rel_size);
		void pre_reset();
		void post_reset();
		void merge_icon_with_latest_font(float font_size, bool FontDataOwnedByAtlas = false);

		void wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	private:
		void new_frame();
		void end_frame();
	public:
		bool m_init = false;
		ImFont* m_font = nullptr;
		ImFont* m_monospace_font = nullptr;

		HWND m_window = NULL;
	private:
		LPDIRECT3DDEVICE9 m_d3d_device = nullptr;
		
		std::map<std::uint32_t, dx_callback> m_dx_callbacks;
		std::vector<wndproc_callback> m_wndproc_callbacks;
	};

	inline renderer *g_renderer{};
}
