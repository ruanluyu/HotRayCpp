#pragma once

#include <vector>
#include "l0_def/s0_numerical.hpp"
#include "s0_raybase.hpp"

using namespace hr::def;

namespace hr::ray {

	/// <summary>
	/// 
	/// </summary>
	using MoveConverterFunction = void (*)(RayData& from_ray, RayData& to_ray);



	class BasicConverter {
	public:
		virtual void Apply(RayData& from_ray, RayData& to_ray);
	};

	class SingleConverter :public BasicConverter {
	public:
		SingleConverter(MoveConverterFunction func);
		void Apply(RayData& from_ray, RayData& to_ray) override;
	private:
		MoveConverterFunction func;
	};


	class ComboConverter : public BasicConverter {
	public:
		ComboConverter();
		void Add(const MoveConverterFunction& newly);
		void Apply(RayData& from_ray, RayData& to_ray) override;
	private:
		std::vector<MoveConverterFunction> converters;
	};

#define CONVERTER_FUNC_NAME(FROM, TO) FROM ## _ ## TO ## _converter
#define CONVERTER_FUNC_SIGNATURE(FROM, TO) void CONVERTER_FUNC_NAME(FROM,TO)(RayData& from_ray, RayData& to_ray)

#define CREATE_CONVERTER_API(FROM, TO) extern CONVERTER_FUNC_SIGNATURE(FROM, TO)


	CREATE_CONVERTER_API(i8, i16);
	CREATE_CONVERTER_API(i16, i8);

	CREATE_CONVERTER_API(i16, i32);
	CREATE_CONVERTER_API(i32, i16);

	CREATE_CONVERTER_API(i32, i64);
	CREATE_CONVERTER_API(i64, i32);

	CREATE_CONVERTER_API(ui8, ui16);
	CREATE_CONVERTER_API(ui16, ui8);

	CREATE_CONVERTER_API(ui16, ui32);
	CREATE_CONVERTER_API(ui32, ui16);

	CREATE_CONVERTER_API(ui32, ui64);
	CREATE_CONVERTER_API(ui64, ui32);


	CREATE_CONVERTER_API(i16, ui8);
	CREATE_CONVERTER_API(i32, ui16);
	CREATE_CONVERTER_API(i64, ui32);

	CREATE_CONVERTER_API(ui8, i16);
	CREATE_CONVERTER_API(ui16, i32);
	CREATE_CONVERTER_API(ui32, i64);

}