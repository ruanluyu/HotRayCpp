#pragma once

#include "s1_converters.hpp"
#include <unordered_map>
#include <string>

namespace hr::ray
{
	using namespace hr::def;
	void init_ray();

	class GlobalRayConfigs {
	public:
		struct RayConfig {
			RayConfig();
			hr_string unique_name; // e.g.: "i8", also called uname. 
			using init_function_type = void(*)(RayData& data);
			using free_function_type = void(*)(RayData& data);
			using copy_function_type = void(*)(const RayData& from, RayData& to);
			using move_function_type = void(*)(RayData& from, RayData& to);

			init_function_type init_function;
			free_function_type free_function;
			copy_function_type copy_function;
			move_function_type move_function;

			static void default_init_function(RayData& data);
			static void default_free_function(RayData& data);
			static void default_copy_function(const RayData& from, RayData& to);
			static void default_move_function(RayData& from, RayData& to);

			hr_unordered_map<hr_string, MoveConverterFunction> converter_to_list;
			hr_unordered_map<hr_string, MoveConverterFunction> converter_from_list;
		};


		bool AddConfig(const sptr<RayConfig>& config);
		
		sptr<RayConfig> GetConfig(const char* unique_name) const;

		void BuildConverterGraph();

		sptr<BasicConverter> GetConverter(const char* from_ray_uname, const char* to_ray_uname);

		
	private:
		hr_unordered_map<hr_string, sptr<RayConfig>> ray_configs;

		std::pair<hr_string, hr_string> _CombineFromToUname(const hr_string& from_ray_uname, const hr_string& to_ray_uname);

		hr_unordered_map<std::pair<hr_string, hr_string>, MoveConverterFunction> converter_direct_graph;
		hr_unordered_map<std::pair<hr_string, hr_string>, sptr<BasicConverter>> converter_path_graph;
	};

	extern GlobalRayConfigs global_configs;

	


	

}