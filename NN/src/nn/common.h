/*
	Common headers and functions
*/

#pragma once

#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>
#include <cassert>

namespace nn
{
	using byte = unsigned char;
	using uint = unsigned int;

	// print the time
	std::ostream& time_stamp(std::ostream& out);

	template<typename type>
	class span
	{
		type* _begin;
		type* _end;

	public:

		using value_type = type;
		using reference = type&;
		using const_reference = const type&;
		using size_type = size_t;

		using iterator = type*;
		using const_iterator = const type*;

		span(type* begin, const type* end) :
			_begin(begin), _end(end)
		{}

		reference at(size_t i)
		{
			assert((_begin + i) < _end);
			return _begin[i];
		}

		const_reference at(size_t i) const
		{
			assert((_begin + i) < _end);
			return _begin[i];
		}

		size_type length() const { return _end - _begin; }

		iterator begin() { return _begin; }
		iterator end() { return _end; }

		const_iterator begin() const { return _begin; }
		const_iterator end() const { return _end; }

		type operator[](size_type i) { return at(i); }
		type operator[](size_type i) const { return at(i); }
	};
}
