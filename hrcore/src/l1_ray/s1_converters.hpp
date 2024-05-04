#pragma once

#include <vector>
#include "l0_def/s0_numerical.hpp"
#include "s0_raybase.hpp"

using namespace hr::def;

namespace hr::ray {


	using ConverterFunction = void (*)(const RayData& from_ray, RayData& to_ray);



	class BasicConverter {
	public:
		virtual void Apply(const RayData& from_ray, RayData& to_ray);
	};

	class SingleConverter :public BasicConverter {
	public:
		SingleConverter(ConverterFunction func);
		void Apply(const RayData& from_ray, RayData& to_ray) override;
	private:
		ConverterFunction func;
	};


	class ComboConverter : public BasicConverter {
	public:
		ComboConverter();
		void Add(const ConverterFunction& newly);
		void Apply(const RayData& from_ray, RayData& to_ray) override;
	private:
		std::vector<ConverterFunction> converters;
	};

#define CONVERTER_FUNC_NAME(FROM, TO) FROM ## _ ## TO ## _converter
#define CONVERTER_FUNC_SIGNATURE(FROM, TO) void CONVERTER_FUNC_NAME(FROM,TO)(const RayData& from_ray, RayData& to_ray)

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