
#pragma once

#include <concepts>
#include <ostream>
#include <istream>
#include <complex>
#include <limits>
#include <bit>

// hash pair support
namespace std
{
	template<class A, class B>
	struct hash<pair<A, B>> {
		size_t operator() (const pair<A, B>& p) const {
			return std::rotl(hash<A>{}(p.first), 4) ^
				hash<B>{}(p.second);
		}
	};
}

#define HR_ARRAY_LEN(arr) (sizeof((arr))/sizeof((arr)[0]))

namespace hr::def {
	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	const i8 i8_min = 0x80i8;
	const i8 i8_max = 0x7Fi8;

	const i16 i16_min = 0x8000i16;
	const i16 i16_max = 0x7FFFi16;

	const i32 i32_min = 0x80000000i32;
	const i32 i32_max = 0x7FFFFFFFi32;

	const i64 i64_min = 0x8000000000000000i64;
	const i64 i64_max = 0x7FFFFFFFFFFFFFFFi64;

	using ui8 = uint8_t;
	using ui16 = uint16_t;
	using ui32 = uint32_t;
	using ui64 = uint64_t;

	const ui8 ui8_min = 0x00ui8;
	const ui8 ui8_max = 0xFFui8;

	const ui16 ui16_min = 0x0000ui16;
	const ui16 ui16_max = 0xFFFFui16;

	const ui32 ui32_min = 0x00000000ui32;
	const ui32 ui32_max = 0xFFFFFFFFui32;

	const ui64 ui64_min = 0x0000000000000000ui64;
	const ui64 ui64_max = 0xFFFFFFFFFFFFFFFFui64;

	using f32 = float;
	using f64 = double;

	using b8 = bool;

	template<typename T>
	concept Numerical = requires(T & t, std::ostream& os, std::istream & is) {
		{t + t}->std::convertible_to<T>;
		{t - t}->std::convertible_to<T>;
		{- t}->std::convertible_to<T>;
		{t* t}->std::convertible_to<T>;
		{t / t}->std::convertible_to<T>;
		{t == t}->std::convertible_to<b8>;
		{t != t}->std::convertible_to<b8>;
		{os << t}->std::convertible_to<std::ostream&>;
		{is >> t}->std::convertible_to<std::istream&>;
	};


	template<typename T>
	std::istream& operator>>(std::istream& is, std::complex<T>& right)
	{
		char buffer;
		std::stringstream ss;
		ui16 read_count = 0;

		T real_val = 0;
		T imag_val = 0;

		typename std::istream::iostate state = std::istream::goodbit;

		bool set_real = false;
		bool set_imag = false;
		while (true)
		{
			if (is.peek() == EOF) {
				state |= std::istream::eofbit;
				break;
			}
			is.read(&buffer, 1);
			if (buffer == '+' || buffer == '-')
			{
				if (read_count == 0)
				{
					// nothing
				}
				else
				{
					ss >> real_val;
					ss.str("");
					ss.clear(); // clear error states
					set_real = true;
				}
			}
			else if (buffer == 'i')
			{
				if (ss.str().empty()) imag_val = 1;
				else {
					ss >> imag_val;
					ss.str("");
					ss.clear(); // clear error states
				}
				set_imag = true;
				break;
			}
			ss << buffer;
			++read_count;
		}


		if ((read_count > 0) && (!set_real) && (!set_imag))
		{
			ss >> real_val;
		}

		right = std::complex<T>(real_val, imag_val);

		return is;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const std::complex<T>& right)
	{
		std::stringstream ss;
		T real_v = right.real();
		T imag_v = right.imag();

		b8 real_zero = real_v == 0;
		b8 imag_zero = imag_v == 0;

		if (!real_zero)
		{
			ss << real_v;
		}

		if (!imag_zero)
		{
			ss << std::showpos << imag_v << "i";
		}

		if (real_zero && imag_zero) ss << "0";


		os << ss.str();
		return os;
	}

	using f32c = std::complex<float>;
	using f64c = std::complex<double>;




#ifdef HR_DEBUG
	static_assert(sizeof(i8) == 1);
	static_assert(sizeof(i16) == 2);
	static_assert(sizeof(i32) == 4);
	static_assert(sizeof(i64) == 8);

	static_assert(sizeof(f32) == 4);
	static_assert(sizeof(f64) == 8);

	static_assert(sizeof(b8) == 1);

	static_assert(sizeof(f32c) == 4 * 2);
	static_assert(sizeof(f64c) == 8 * 2);
#endif
}