#include "common.hpp"
#include "fiber_pool.hpp"
#include "gui.hpp"
#include "logger.hpp"
#include "hooking.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include "script_mgr.hpp"
#include "settings.hpp"
#include "benchmark.hpp"
#include "event_loop/backend_events.hpp"

#include "services/gui/gui_service.hpp"
#include "services/player/player_service.hpp"
#include "services/notification/notification_service.hpp"

DWORD APIENTRY main_thread(LPVOID)
{
	using namespace big;

	while (!FindWindow(WINDOW_CLASS, WINDOW_NAME))
		std::this_thread::sleep_for(1s);

	auto initialization_benchmark = std::make_unique<benchmark>("Initialization Benchmark");

	auto logger_instance = std::make_unique<logger>("Demo");
	try
	{
		LOG(RAW_GREEN_TO_CONSOLE) << R"kek(               ...
                   ;::::;
                 ;::::; :;
               ;:::::'   :;
              ;:::::;     ;.
             ,:::::'       ;           OOO\
             ::::::;       ;          OOOOO\
             ;:::::;       ;         OOOOOOOO
            ,;::::::;     ;'         / OOOOOOO
          ;:::::::::`. ,,,;.        /  / DOOOOOO
        .';:::::::::::::::::;,     /  /     DOOOO
       ,::::::;::::::;;;;::::;,   /  /        DOOO
      ;`::::::`'::::::;;;::::: ,#/  /          DOOO
      :`:::::::`;::::::;;::: ;::#  /            DOOO
      ::`:::::::`;:::::::: ;::::# /              DOO
      `:`:::::::`;:::::: ;::::::#/               DOO
       :::`:::::::`;; ;:::::::::##                OO
       ::::`:::::::`;::::::::;:::#                OO
       `:::::`::::::::::::;'`:;::#                O
        `:::::`::::::::;' /  / `:#
         ::::::`:::::;'  /  /   `#

)kek";
		auto settings_instance = std::make_unique<settings>();
		g_settings->load();
		LOG(HACKER) << "Settings initialized.";
		
		auto pointers_instance = std::make_unique<pointers>();
		LOG(HACKER) << "Pointers initialized.";

		auto renderer_instance = std::make_unique<renderer>();
		LOG(HACKER) << "Renderer initialized.";

		auto fiber_pool_instance = std::make_unique<fiber_pool>(10);
		LOG(HACKER) << "Fiber pool initialized.";

		auto hooking_instance = std::make_unique<hooking>();
		LOG(HACKER) << "Hooking initialized.";

		auto thread_pool_instance = std::make_unique<thread_pool>();
		LOG(HACKER) << "Thread Pool initialized.";

		auto gui_service_instance  = std::make_unique<gui_service>();
		auto notification_instance = std::make_unique<notification_service>();
		LOG(HACKER) << "Service registered.";

		g_script_mgr.add_script(std::make_unique<script>(&gui::script_func));
		LOG(HACKER) << "Scripts registered.";

		g_hooking->enable();
		LOG(HACKER) << "Hooking enabled.";

		initialization_benchmark->get_runtime();
		initialization_benchmark->reset();
		initialization_benchmark.reset();
		
		while (g_running)
		{
			g_script_mgr.tick();
			std::this_thread::sleep_for(1s);
		}

		g_hooking->disable();
		LOG(HACKER) << "Hooking disabled.";

		std::this_thread::sleep_for(1000ms);

		g_script_mgr.remove_all_scripts();
		LOG(HACKER) << "Scripts unregistered.";

		notification_instance.reset();
		gui_service_instance.reset();
		LOG(HACKER) << "Service unregistered.";

		hooking_instance.reset();
		LOG(HACKER) << "Hooking uninitialized.";

		fiber_pool_instance.reset();
		LOG(HACKER) << "Fiber pool uninitialized.";

		g_thread_pool->destroy();
		LOG(HACKER) << "Destroyed thread pool.";

		thread_pool_instance.reset();
		LOG(HACKER) << "Thread Pool uninitialized.";

		renderer_instance.reset();
		LOG(HACKER) << "Renderer uninitialized.";

		pointers_instance.reset();
		LOG(HACKER) << "Pointers uninitialized.";

		g_settings->attempt_save();
		settings_instance.reset();
		LOG(HACKER) << "Settings saved and uninitialized.";
	}
	catch (std::exception const& ex)
	{
		LOG(WARNING) << ex.what();
		MessageBoxA(nullptr, ex.what(), nullptr, MB_OK | MB_ICONEXCLAMATION);
	}

	LOG(HACKER) << "Farewell!";
	logger_instance.reset();

	CloseHandle(g_main_thread);
	FreeLibraryAndExitThread(g_hmodule, 0);

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID)
{
	using namespace big;

	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hmod);

		g_hmodule = hmod;
		g_main_thread = CreateThread(nullptr, 0, &main_thread, nullptr, 0, &g_main_thread_id);
	}

	return true;
}
