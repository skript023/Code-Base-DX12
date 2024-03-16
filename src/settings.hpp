#pragma once
#include "logger.hpp"
#include "imgui.h"

namespace big
{
	class settings;
	inline settings* g_settings{};

	class settings {
		nlohmann::json default_options;
		nlohmann::json options;

		struct self
		{
			bool rapid_skill { false };
			bool godmode { false };
			bool auto_kill { false };
			bool crit_dmg { false };
			bool crit_rate { false };
			bool atk_speed { false };
			float damage_crit = 1.5f;
			float rate_crit = 85.5f;
			float speed_atk = 1.2f;
		};

		struct window
		{
			ImU32 color = 3357612055;
			float gui_scale = 1.f;

			ImFont* font_title = nullptr;
			ImFont* font_sub_title = nullptr;
			ImFont* font_small = nullptr;
			ImFont* font_icon = nullptr;

			ImU32 background_color = 3696311571;
			ImU32 text_color = 4294967295;
			ImU32 button_color = 2947901213;
			ImU32 frame_color = 2942518340;

			bool switched_view = true;
		};

		struct hotkeys
		{
			bool editing_menu_toggle = false;
			int menu_toggle = VK_INSERT;
		};

	public:
		settings()
		{
			g_settings = this;
		}

		~settings()
		{
			g_settings = nullptr;
		}

		self self{};
		window window{};
		hotkeys hotkeys{};

		void from_json(const nlohmann::json& j)
		{
			this->self.rapid_skill = j["self"]["rapid_skill"];
			this->self.godmode = j["self"]["godmode"];
			this->self.auto_kill = j["self"]["auto_kill"];
			this->self.crit_dmg = j["self"]["crit_dmg"];
			this->self.crit_rate = j["self"]["crit_rate"];
			this->self.atk_speed = j["self"]["atk_speed"];

			this->window.color = j["window"]["color"];
			this->window.gui_scale = j["window"]["gui_scale"];

			this->hotkeys.editing_menu_toggle = j["hotkeys"]["editing_menu_toggle"];
			this->hotkeys.menu_toggle = j["hotkeys"]["menu_toggle"];
		}

		nlohmann::json to_json()
		{
			return nlohmann::json{
				{
					"self", {
						{ "rapid_skill", this->self.rapid_skill },
						{ "godmode", this->self.godmode },
						{ "auto_kill", this->self.auto_kill },
						{ "crit_dmg", this->self.crit_dmg },
						{ "crit_rate", this->self.crit_rate },
						{ "atk_speed", this->self.atk_speed }
					}
				},
				{
					"window", {
						{ "color", this->window.color },
						{ "gui_scale", this->window.gui_scale }
					}
				},
				{
					"hotkeys", {
						{ "editing_menu_toggle", this->hotkeys.editing_menu_toggle },
						{ "menu_toggle", this->hotkeys.menu_toggle }
					}
				}
			};
		}

		void attempt_save()
		{
			nlohmann::json j = this->to_json();

			if (deep_compare(this->options, j, true))
				this->save();
		}

		bool load()
		{
			this->default_options = this->to_json();

			std::string settings_file = std::getenv("appdata");
			settings_file += this->settings_location;

			std::ifstream file(settings_file);

			if (!file.is_open())
			{
				this->write_default_config();

				file.open(settings_file);
			}

			try
			{
				file >> this->options;
			}
			catch (const std::exception&)
			{
				LOG(HACKER) << "Detected corrupt settings, writing default config...";

				this->write_default_config();

				return this->load();
			}

			bool should_save = this->deep_compare(this->options, this->default_options);

			this->from_json(this->options);

			if (should_save)
			{
				LOG(HACKER) << "Updating settings...";
				save();
			}

			return true;
		}

	private:
		const char* settings_location = "\\Scarlet Nexus Trainer\\settings.json";

		bool deep_compare(nlohmann::json& current_settings, const nlohmann::json& default_settings, bool compare_value = false)
		{
			bool should_save = false;

			for (auto& e : default_settings.items())
			{
				const std::string& key = e.key();

				if (current_settings.count(key) == 0 || (compare_value && current_settings[key] != e.value()))
				{
					current_settings[key] = e.value();

					should_save = true;
				}
				else if (current_settings[key].is_structured() && e.value().is_structured())
				{
					if (deep_compare(current_settings[key], e.value(), compare_value))
						should_save = true;
				}
				else if (!current_settings[key].is_structured() && e.value().is_structured()) {
					current_settings[key] = e.value();

					should_save = true;
				}
			}

			return should_save;
		}

		bool save()
		{
			std::string settings_file = std::getenv("appdata");
			settings_file += this->settings_location;

			std::ofstream file(settings_file, std::ios::out | std::ios::trunc);
			file << this->to_json().dump(4);
			file.close();

			return true;
		}

		bool write_default_config()
		{
			std::string settings_file = std::getenv("appdata");
			settings_file += this->settings_location;

			std::ofstream file(settings_file, std::ios::out);
			file << this->to_json().dump(4);
			file.close();

			return true;
		}
	};
}