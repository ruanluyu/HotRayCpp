#include "pch.h"
#include "s2_configs.hpp"
#include <sstream>
#include <queue>
#include <vector>
#include <unordered_set>
#include "l0_def/s1_global.hpp"

namespace hr::ray {
	GlobalRayConfigs global_ray_configs;
	
	bool GlobalRayConfigs::AddConfig(const sptr<RayConfig>& config)
	{
		if (config == nullptr)
		{
			using namespace hr::def;
			using namespace std;

			if (has_warning_logger()) {
				log_warning("nullptr is added to configs. At GlobalRayConfigs::AddConfig. ");
			}
			return false;
		}
		if (ray_configs.count(config->unique_name) > 0)
		{
			using namespace hr::def;
			using namespace std;

			if (has_error_logger()) {
				hr_stringstream ss;
				ss << "Unique name of ray config collided. Unique name: " << config->unique_name << " has already existed. At GlobalRayConfigs::AddConfig. ";
				log_error(ss.str().c_str());
			}
			return false;
		}

		this->ray_configs[config->unique_name] = config;

		return true;
	}
	sptr<GlobalRayConfigs::RayConfig> GlobalRayConfigs::GetConfig(const char* unique_name) const
	{
		auto config_ite = ray_configs.find(unique_name);
		if (config_ite == ray_configs.end()) return nullptr;
		return config_ite->second;
	}

	void GlobalRayConfigs::BuildConverterGraph()
	{

		converter_direct_graph.clear();
		converter_direct_graph.reserve(1024);

		for (auto& kv : ray_configs)
		{
			auto& from_converters = kv.second->converter_from_list;
			for (auto& from_kv : from_converters)
			{
				auto key = _CombineFromToUname(from_kv.first, kv.first);
				converter_direct_graph.insert(std::pair(key, from_kv.second));
			}
		}

		for (auto& kv : ray_configs)
		{
			auto& to_converters = kv.second->converter_to_list;
			for (auto& to_kv : to_converters)
			{
				auto key = _CombineFromToUname(kv.first, to_kv.first);
				converter_direct_graph.insert(std::pair(key, to_kv.second));
			}
		}

		converter_direct_graph.rehash(converter_direct_graph.size() + 64);

		hr_unordered_map<hr_string, hr_unordered_set<hr_string>> dag;
		

		for (auto& kv : converter_direct_graph)
		{
			auto direction = kv.first;
			if (dag.count(direction.first) == 0)
			{
				hr_unordered_set<hr_string> v;
				v.insert(direction.second);
				dag.insert(std::pair(direction.first, v));
			}
			else
			{
				dag.at(direction.first).insert(direction.second);
			}
		}

		converter_path_graph.clear();
		converter_path_graph.reserve(converter_direct_graph.size() * 2);

		for (auto& kv : ray_configs)
		{
			auto key = _CombineFromToUname(kv.first, kv.first);
			auto path_ptr = hr_make_shared<BasicConverter>();
			converter_path_graph.insert(std::pair(key, path_ptr));
		}

		for (auto& kv : converter_direct_graph)
		{
			auto path_ptr = hr_make_shared<SingleConverter>(kv.second);
			converter_path_graph.insert(std::pair(kv.first, path_ptr));
		}

		for (auto& dag_kv : dag)
		{
			hr_queue<std::pair<hr_string, hr_vector<hr_string>>> bfs;
			hr_unordered_set<hr_string> searched;
			bfs.push(std::pair(dag_kv.first, hr_vector<hr_string>{dag_kv.first}));

			while (!bfs.empty())
			{
				auto cur_kv = std::move(bfs.front());
				auto& cur_uname_ref = cur_kv.first;
				auto& cur_path_ref = cur_kv.second;
				bfs.pop();

				searched.insert(cur_uname_ref);

				auto& adj = dag.at(cur_uname_ref);

				for (auto& adj_uname : adj)
				{
					if (searched.contains(adj_uname)) continue;

					auto new_path = hr_vector<hr_string>(cur_path_ref);
					new_path.push_back(adj_uname);

					if (new_path.size() >= 2)
					{
						auto direction_key = _CombineFromToUname(new_path[0], adj_uname);
						if (converter_path_graph.count(direction_key) == 0)
						{
							auto combo = hr_make_shared<ComboConverter>();
							
							for (ui64 i = 0; i < new_path.size()-1; i++)
							{
								auto sub_dir_key = _CombineFromToUname(new_path[i], new_path[i + 1]);
								combo->Add(converter_direct_graph.at(sub_dir_key));
							}
							sptr<BasicConverter> converters_ptr = combo;
							converter_path_graph[direction_key] = converters_ptr;
						}
					}
					bfs.push(std::pair(adj_uname, std::move(new_path)));
				}
			}
		}

		converter_path_graph.rehash(converter_path_graph.size() + 64);
	}

	sptr<BasicConverter> GlobalRayConfigs::GetConverter(const sptr<RayConfig>& from_ray, const sptr<RayConfig>& to_ray)
	{
		auto res = converter_path_graph.find(_CombineFromToUname(from_ray->unique_name, to_ray->unique_name));
		if (res == converter_path_graph.end()) return nullptr;
		return res->second;
	}

	std::pair<hr_string, hr_string> GlobalRayConfigs::_CombineFromToUname(const hr_string& from_ray_uname, const hr_string& to_ray_uname)
	{
		return std::pair(from_ray_uname, to_ray_uname);
	}


	static const char* const ray_names[] = {
		"b8",
		"i8",
		"i16",
		"i32",
		"i64",
		"ui8",
		"ui16",
		"ui32",
		"ui64",
		"f32",
		"f64",
		"f32c",
		"f64c",
		"f32v1",
		"f32v2",
		"f32v3",
		"f32v4",
		"f64v1",
		"f64v2",
		"f64v3",
		"f64v4",
		"f32cv1",
		"f32cv2",
		"f32cv3",
		"f32cv4",
		"f64cv1",
		"f64cv2",
		"f64cv3",
		"f64cv4",
		"i8v1",
		"i8v2",
		"i8v3",
		"i8v4",
		"i16v1",
		"i16v2",
		"i16v3",
		"i16v4",
		"i32v1",
		"i32v2",
		"i32v3",
		"i32v4",
		"i64v1",
		"i64v2",
		"i64v3",
		"i64v4",
		"string",
	};


	struct _ConverterInfo {
		const char* from;
		const char* to;
		MoveConverterFunction converter;
	};

#define CONVERTER_INFO_CELL(FROM, TO) {#FROM, #TO, CONVERTER_FUNC_NAME(FROM, TO)},
	static const _ConverterInfo converter_infos[]{
	CONVERTER_INFO_CELL(i8, i16)
	CONVERTER_INFO_CELL(i16, i8)

	CONVERTER_INFO_CELL(i16, i32)
	CONVERTER_INFO_CELL(i32, i16)

	CONVERTER_INFO_CELL(i32, i64)
	CONVERTER_INFO_CELL(i64, i32)

	CONVERTER_INFO_CELL(ui8, ui16)
	CONVERTER_INFO_CELL(ui16, ui8)

	CONVERTER_INFO_CELL(ui16, ui32)
	CONVERTER_INFO_CELL(ui32, ui16)

	CONVERTER_INFO_CELL(ui32, ui64)
	CONVERTER_INFO_CELL(ui64, ui32)


	CONVERTER_INFO_CELL(i16, ui8)
	CONVERTER_INFO_CELL(i32, ui16)
	CONVERTER_INFO_CELL(i64, ui32)

	CONVERTER_INFO_CELL(ui8, i16)
	CONVERTER_INFO_CELL(ui16, i32)
	CONVERTER_INFO_CELL(ui32, i64)
	};




	void init_ray()
	{
		using cfg_t = GlobalRayConfigs::RayConfig;
		for (auto& name : ray_names)
		{
			auto ptr = hr_make_shared<cfg_t>();
			ptr->unique_name = name;
			global_ray_configs.AddConfig(ptr);
		}

		for (auto& info : converter_infos)
		{
			auto config = global_ray_configs.GetConfig(info.from);
			auto& to_list = config->converter_to_list;
			to_list[info.to] = info.converter;
		}
		

		global_ray_configs.BuildConverterGraph();
	}


	static void default_init_function(RayData& data)
	{
		memset(&data, 0, sizeof(RayData));
	}

	static void default_free_function(RayData& data)
	{
		// For performance reason. 
		// memset(&from, 0, sizeof(RayData));
	}

	static void default_copy_function(const RayData& from, RayData& to)
	{
		memcpy(&to, &from, sizeof(RayData));
	}

	static void default_move_function(RayData& from, RayData& to)
	{
		memcpy(&to, &from, sizeof(RayData));
		// For performance reason. 
		// memset(&from, 0, sizeof(RayData));
	}

	GlobalRayConfigs::RayConfig::RayConfig() :
		unique_name(),
		converter_to_list(),
		converter_from_list(),
		copy_function(default_copy_function),
		move_function(default_move_function),
		init_function(default_init_function),
		free_function(default_free_function)
	{
	}




}