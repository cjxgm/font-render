#pragma once
#include <cstddef>	// for std::nullptr_t

namespace tue
{
	// fulfill NullablePointer requirement
	template <class T, T NIL=T{}>
	struct nullable
	{
		nullable() = default;
		nullable(T const& x) : value{x} {}
		nullable(std::nullptr_t) {};

		operator auto& () const { return value; }
		operator bool () const { return (value != NIL); }
		bool operator != (std::nullptr_t) { return *this; }
		bool operator == (std::nullptr_t) { return !*this; }

	private:
		T value{NIL};
	};
}

