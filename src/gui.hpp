#pragma once
#include "common.hpp"

namespace big
{
	class gui
	{
	public:
		gui();
		virtual ~gui();
		
		gui(const gui&)                = delete;
		gui(gui&&) noexcept            = delete;
		gui& operator=(const gui&)     = delete;
		gui& operator=(gui&&) noexcept = delete;


		void dx_init();
		void dx_on_tick();
		void dx_on_opened();

		void script_init();
		void script_on_tick();
		static void script_func();
		void wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		void save_default_style();
		void restore_default_style();

		void push_theme_colors();
		void pop_theme_colors();

		bool is_open();
		void toggle(bool toggle);

		bool mouse_override() const
		{
			return m_override_mouse;
		}

		void override_mouse(bool override);
	private:
		void toggle_mouse();
	private:
		bool m_override_mouse{};
		bool m_opened{};
		ImGuiStyle m_default_config;
	};

	inline gui* g_gui;
}
