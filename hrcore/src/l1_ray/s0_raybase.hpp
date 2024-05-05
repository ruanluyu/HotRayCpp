#pragma once

#include "l0_def/s2_vec.hpp"
#include <vector>
namespace hr::ray {

	using namespace hr::def;

	const ui32 RAY_MEM_SIZE = 8u; // [bytes]

	struct RayData {
		i8 data[RAY_MEM_SIZE];

		constexpr ui8 status_inited_bit() { return 1ui8 << 0ui8; }
		constexpr ui8 status_freed_bit() { return 1ui8 << 1ui8;}

		constexpr ui8 status_before_init() { return 0ui8; }
		constexpr ui8 status_after_init() { return status_inited_bit(); }
		constexpr ui8 status_after_free() { return status_inited_bit() | status_freed_bit(); }

		ui8 status = status_before_init();


		RayData() { memset(data, 0, RAY_MEM_SIZE); }

		RayData(const RayData& other) { memcpy(data, other.data, RAY_MEM_SIZE); }

		template<typename T>
		requires (sizeof(T) <= RAY_MEM_SIZE)
		RayData(const T& val) : RayData() {
			Set(val);
		}

		template<typename T>
		requires (sizeof(T) <= RAY_MEM_SIZE)
		void Set(const T& val) 
		{
			*(reinterpret_cast<T*>(data)) = val;
		}

		template<typename T>
		requires (sizeof(T) <= RAY_MEM_SIZE)
		const T& Get() const
		{
			return *reinterpret_cast<const T*>(data);
		}

	};



}