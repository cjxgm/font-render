#include <cstdint>
#include <cstdio>

namespace
{
	auto decode(char const* & s)
	{
		std::uint32_t code;
		switch (auto c = static_cast<unsigned char>(*s++)) {
			case 0b0'0000000 ... 0b0'1111111: code = c; break;
			case 0b110'00000 ... 0b110'11111: code = c & 0b000'11111; break;
			case 0b1110'0000 ... 0b1110'1111: code = c & 0b0000'1111; break;
			case 0b11110'000 ... 0b11110'111: code = c & 0b00000'111; break;
			case 0b111110'00 ... 0b111110'11: code = c & 0b000000'11; break;
			case 0b1111110'0 ... 0b1111110'1: code = c & 0b0000000'1; break;
		}
		for (; (*s & 0b11'000000) == 0b10'000000; s++) code = (code << 6) | (*s & 0b00'111111);
		return code;
	}

#if 0
	auto decode(char const* const& s)
	{
		auto ss = s;
		return decode(ss);
	}
#endif

	auto test(char const* s)
	{
		while (*s) std::printf("U+%8.8X\n", decode(s));
	}
}

int main()
{
	test("A测试😂😹");
	/* output:
	U+00000041
	U+00006D4B
	U+00008BD5
	U+0001F602
	U+0001F639
	*/
}

