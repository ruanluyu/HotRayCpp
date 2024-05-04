#pragma once
#include <ostream>
#include <istream>
#include "s0_numerical.hpp"
#include "s1_global.hpp"
namespace hr::def {

	template<Numerical T, ui64 L>
	struct vec {
		T v[L];

		class iterator {
		public:
			iterator(T* ptr): ptr(ptr) {}
			iterator& operator++() {
				++ptr;
				return *this;
			}
			iterator operator++(i32) {
				iterator old = *this;
				++ptr;
				return old;
			}
			iterator& operator--() {
				--ptr;
				return *this;
			}
			iterator operator--(i32) {
				iterator old = *this;
				--ptr;
				return old;
			}
			b8 operator==(const iterator& other){
				return other.ptr == ptr;
			}
			b8 operator!=(const iterator& other) {
				return other.ptr != ptr;
			}
			T& operator*() { return *ptr; }
			T* operator->() { return ptr; }
		private:
			T* ptr;
		};

		iterator begin() {
			return iterator(v);
		}

		iterator end() {
			return iterator(v + L);
		}


		constexpr ui64 size() { return L; }



		b8 operator==(const vec<T, L>& other) {
			for (ui64 i = 0; i < L; i++)
			{
				if (v[i] != other.v[i]) return false;
			}
			return true;
		}

		b8 operator!=(const vec<T, L>& other) {
			for (ui64 i = 0; i < L; i++)
			{
				if (v[i] == other.v[i]) return true;
			}
			return false;
		}

		vec<T, L> operator+(const vec<T, L>& other) {
			vec<T, L> res;
			for (ui64 i = 0; i < L; i++)
			{
				res.v[i] = v[i] + other.v[i];
			}
			return res;
		}

		vec<T, L> operator+(const T& other) {
			vec<T, L> res;
			for (ui64 i = 0; i < L; i++)
			{
				res.v[i] = v[i] + other;
			}
			return res;
		}

		vec<T, L>& operator++() {
			for (ui64 i = 0; i < L; i++)
			{
				v[i] = v[i] + 1;
			}
			return *this;
		}
		vec<T, L> operator++(i32) {
			auto old = *this;
			for (ui64 i = 0; i < L; i++)
			{
				v[i] = v[i] + 1;
			}
			return old;
		}

		friend vec<T, L> operator+(const T& left, const vec<T, L>& right) {
			vec<T, L> res;
			for (ui64 i = 0; i < L; i++)
			{
				res.v[i] = left + right.v[i];
			}
			return res;
		}


		vec<T, L> operator-(const vec<T, L>& other) {
			vec<T, L> res;
			for (ui64 i = 0; i < L; i++)
			{
				res.v[i] = v[i] - other.v[i];
			}
			return res;
		}

		vec<T, L> operator-() {
			vec<T, L> res;
			for (ui64 i = 0; i < L; i++)
			{
				res.v[i] = - v[i];
			}
			return res;
		}


		vec<T, L> operator-(const T& other) {
			vec<T, L> res;
			for (ui64 i = 0; i < L; i++)
			{
				res.v[i] = v[i] - other;
			}
			return res;
		}

		friend vec<T, L> operator-(const T& left, const vec<T, L>& right) {
			vec<T, L> res;
			for (ui64 i = 0; i < L; i++)
			{
				res.v[i] = left - right.v[i];
			}
			return res;
		}

		vec<T, L>& operator--() {
			for (ui64 i = 0; i < L; i++)
			{
				v[i] = v[i] - 1;
			}
			return *this;
		}
		vec<T, L> operator--(i32) {
			auto old = *this;
			for (ui64 i = 0; i < L; i++)
			{
				v[i] = v[i] - 1;
			}
			return old;
		}

		vec<T, L> operator*(const vec<T, L>& other) {
			vec<T, L> res;
			for (ui64 i = 0; i < L; i++)
			{
				res.v[i] = v[i] * other.v[i];
			}
			return res;
		}

		vec<T, L> operator*(const T& other) {
			vec<T, L> res;
			for (ui64 i = 0; i < L; i++)
			{
				res.v[i] = v[i] * other;
			}
			return res;
		}

		friend vec<T, L> operator*(const T& left, const vec<T, L>& right) {
			vec<T, L> res;
			for (ui64 i = 0; i < L; i++)
			{
				res.v[i] = left * right.v[i];
			}
			return res;
		}

		vec<T, L> operator/(const vec<T, L>& other) {
			vec<T, L> res;
			for (ui64 i = 0; i < L; i++)
			{
				res.v[i] = v[i] / other.v[i];
			}
			return res;
		}

		vec<T, L> operator/(const T& other) {
			vec<T, L> res;
			for (ui64 i = 0; i < L; i++)
			{
				res.v[i] = v[i] / other;
			}
			return res;
		}

		friend vec<T, L> operator/(const T& left, const vec<T, L>& right) {
			vec<T, L> res;
			for (ui64 i = 0; i < L; i++)
			{
				res.v[i] = left / right.v[i];
			}
			return res;
		}

		

		void fill(const T& val)
		{
			for (ui64 i = 0; i < L; i++)
			{
				v[i] = val;
			}
		}




		friend std::ostream& operator<<(std::ostream& os, const vec<T,L>& t)
		{
			os << "(";
			for (ui64 i = 0; i < L; i++)
			{
				os << t.v[i];
				if (i != L - 1) os << ",";
			}
			os << ")";
			return os;
		}

		friend std::istream& operator>>(std::istream& is, vec<T, L>& t)
		{
			char buffer;
			std::stringstream ss2;

			std::basic_stringstream<char, std::char_traits<char>, mem::allocator<char>> ss;

			ui16 index = 0;

			typename std::istream::iostate state = std::istream::goodbit;

			auto w = is.width();


			while (true) {
				is.read(&buffer, 1);
				b8 end_mark = false;
				b8 convert_mark = false;
				b8 record_mark = false;

				if (buffer == ',')
				{
					convert_mark = true;
				}
				else if (buffer == '(')
				{
					// nothing
				}
				else if (buffer == ')')
				{
					end_mark = true;
					convert_mark = true;
				}
				else
				{
					record_mark = true;
				}
				

				if (convert_mark)
				{
					if (index >= L)
					{
						state |= std::istream::failbit;
						break;
					}
					T cur_val;
					ss >> cur_val;
					ss.str("");
					ss.clear();
					t.v[index] = cur_val;
					++index;
				}
				
				if(record_mark)
				{
					ss << buffer;
				}

				if (is.peek() == EOF) {
					state |= std::istream::eofbit;
					break;
				}

				if (end_mark) break;
			}


			is.setstate(state);

			return is;
		}
	};


	using f32v1 = vec<f32, 1>;
	using f32v2 = vec<f32, 2>;
	using f32v3 = vec<f32, 3>;
	using f32v4 = vec<f32, 4>;

	using f64v1 = vec<f64, 1>;
	using f64v2 = vec<f64, 2>;
	using f64v3 = vec<f64, 3>;
	using f64v4 = vec<f64, 4>;

	using i8v1 = vec<i8, 1>;
	using i8v2 = vec<i8, 2>;
	using i8v3 = vec<i8, 3>;
	using i8v4 = vec<i8, 4>;
	
	using i16v1 = vec<i16, 1>;
	using i16v2 = vec<i16, 2>;
	using i16v3 = vec<i16, 3>;
	using i16v4 = vec<i16, 4>;

	using i32v1 = vec<i32, 1>;
	using i32v2 = vec<i32, 2>;
	using i32v3 = vec<i32, 3>;
	using i32v4 = vec<i32, 4>;

	using i64v1 = vec<i64, 1>;
	using i64v2 = vec<i64, 2>;
	using i64v3 = vec<i64, 3>;
	using i64v4 = vec<i64, 4>;


	using f32cv1 = vec<f32c, 1>;
	using f32cv2 = vec<f32c, 2>;
	using f32cv3 = vec<f32c, 3>;
	using f32cv4 = vec<f32c, 4>;

}