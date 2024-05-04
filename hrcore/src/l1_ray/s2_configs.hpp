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
			using init_function = void(*)(RayData& data);
			using set_null_function = void(*)(RayData& data);

			init_function init_func;
			set_null_function set_null_func;

			hr_unordered_map<hr_string, ConverterFunction> converter_to_list;
			hr_unordered_map<hr_string, ConverterFunction> converter_from_list;
		};


		bool AddConfig(const sptr<RayConfig>& config);
		
		sptr<RayConfig> GetConfig(const char* unique_name) const;

		void BuildConverterGraph();

		sptr<BasicConverter> GetConverter(const char* from_ray_uname, const char* to_ray_uname);

		
	private:
		hr_unordered_map<hr_string, sptr<RayConfig>> ray_configs;

		std::pair<hr_string, hr_string> _CombineFromToUname(const hr_string& from_ray_uname, const hr_string& to_ray_uname);

		hr_unordered_map<std::pair<hr_string, hr_string>, ConverterFunction> converter_direct_graph;
		hr_unordered_map<std::pair<hr_string, hr_string>, sptr<BasicConverter>> converter_path_graph;
	};

	extern GlobalRayConfigs global_configs;

	


	

}