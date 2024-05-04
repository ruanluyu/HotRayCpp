#pragma once

#include <vector>
#include <bitset>
#include "s0_numerical.hpp"
#include "s1_global.hpp"

namespace hr::def {
	class bigint {
	public:
		using data_type = ui8;
		using double_sized_data_type = ui16;
		using data_container_type = hr_vector<data_type>;

		const ui8 data_type_bytes = sizeof(data_type);
		const ui8 data_type_bits = data_type_bytes * 8;
		const data_type data_type_max_mask = static_cast<data_type>((1ui64 << data_type_bits) - 1ui64);

		bigint();
		bigint(const bigint& other);
		bigint(bigint&& other) noexcept;
		bigint(const char* formatted_bigint);
		bool is_positive() const;
		bool is_infinity() const;
		bool is_nan() const;

		void set(const char* const& formatted_bigint);
		void set(const i64& val);
		void set(const ui64& val);
		void set_to_zero();

		bigint operator+(const bigint& right) const;

		i8 operator<=>(const bigint& right) const;
		void operator=(const bigint& right);

	private:

		void _trim();
		void _data_set_ui64(const ui64& val);

		void _data_add_from(const data_container_type& other);
		bool _data_sub_from(const data_container_type& other); // return sign;

		void _set_positive(bool is_positive);
		void _set_infinity(i8 val);
		void _set_nan(bool is_nan);

		i8 _data_bigger_than(const data_container_type& other) const;

		data_container_type data;
		std::bitset<3> flags;// is_positive, is_infinity, is_nan
	};
}