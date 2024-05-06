#include "pch.h"
#include "s1_converters.hpp"


void hr::ray::BasicConverter::Apply(RayData& from_ray, RayData& to_ray)
{
	memcpy(to_ray.data, from_ray.data, sizeof(to_ray.data));
}

hr::ray::SingleConverter::SingleConverter(const MoveConverterFunction& func) :func(func)
{
}

void hr::ray::SingleConverter::Apply(RayData& from_ray, RayData& to_ray)
{
	func(from_ray, to_ray);
}

hr::ray::ComboConverter::ComboConverter() :converters()
{
}

void hr::ray::ComboConverter::Add(const MoveConverterFunction& newly)
{
	converters.push_back(newly);
}


void hr::ray::ComboConverter::Apply(RayData& from_ray, RayData& to_ray)
{
	RayData buffer;
	ui64 offset = 0;

	if (converters.size() % 2 == 0)
	{
		converters[0](from_ray, buffer);
	}
	else
	{
		converters[0](from_ray, to_ray);
		offset = 1u;
	}

	for (ui64 i = 1; i < converters.size(); i++)
	{
		if ((i + offset) % 2 == 0)
		{
			converters[i](to_ray, buffer);
		}
		else
		{
			converters[i](buffer, to_ray);
		}
	}
}






namespace hr::ray {




	template<typename T1, typename T2>
		requires (std::convertible_to<T1, T2> && 
		sizeof(T1) <= sizeof(RayData) && 
		sizeof(T2) <= sizeof(RayData))
	void cast_converter(const RayData& from_ray, RayData& to_ray) {
		to_ray.Set<T2>((T2)from_ray.Get<T1>());
	}

#define CREATE_CAST_CONVERTER_IMP(FROM, TO) CONVERTER_FUNC_SIGNATURE(FROM, TO){cast_converter<FROM,TO>(from_ray, to_ray);}
#define CREATE_CLAMPED_CAST_CONVERTER_IMP(FROM, TO) CONVERTER_FUNC_SIGNATURE(FROM, TO){\
auto from = from_ray.Get<FROM>();\
if (from < TO##_min) to_ray.Set(TO##_min);\
else if (from > TO##_max) to_ray.Set(TO##_max);\
else to_ray.Set((TO)from); }

#define CREATE_CLAMPED_CAST_I2U_CONVERTER_IMP(FROM, TO) CONVERTER_FUNC_SIGNATURE(FROM, TO){\
auto from = from_ray.Get<FROM>();\
if (from < 0) to_ray.Set(0);\
else to_ray.Set((TO)from); }
#define CREATE_CLAMPED_CAST_U2I_CONVERTER_IMP(FROM, TO) CONVERTER_FUNC_SIGNATURE(FROM, TO){\
auto from = from_ray.Get<FROM>();\
if (from > (FROM) TO##_max) to_ray.Set(TO##_max);\
else to_ray.Set((TO)from); }



	CREATE_CAST_CONVERTER_IMP(i8,i16)
	CREATE_CLAMPED_CAST_CONVERTER_IMP(i16, i8)

	CREATE_CAST_CONVERTER_IMP(i16,i32)
	CREATE_CLAMPED_CAST_CONVERTER_IMP(i32, i16)

	CREATE_CAST_CONVERTER_IMP(i32,i64)
	CREATE_CLAMPED_CAST_CONVERTER_IMP(i64, i32)

	CREATE_CAST_CONVERTER_IMP(ui8, ui16)
	CREATE_CLAMPED_CAST_CONVERTER_IMP(ui16, ui8)

	CREATE_CAST_CONVERTER_IMP(ui16, ui32)
	CREATE_CLAMPED_CAST_CONVERTER_IMP(ui32, ui16)

	CREATE_CAST_CONVERTER_IMP(ui32, ui64)
	CREATE_CLAMPED_CAST_CONVERTER_IMP(ui64, ui32)


	CREATE_CLAMPED_CAST_I2U_CONVERTER_IMP(i16, ui8)
	CREATE_CLAMPED_CAST_I2U_CONVERTER_IMP(i32, ui16)
	CREATE_CLAMPED_CAST_I2U_CONVERTER_IMP(i64, ui32)

	CREATE_CLAMPED_CAST_U2I_CONVERTER_IMP(ui8, i16)
	CREATE_CLAMPED_CAST_U2I_CONVERTER_IMP(ui16, i32)
	CREATE_CLAMPED_CAST_U2I_CONVERTER_IMP(ui32, i64)
}