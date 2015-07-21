#include "font.hh"
#include "stb_truetype.inl"

namespace tue
{
	font::font(std::string const& path, int index)
		: file{map_file(path)}
	{
		auto buf = reinterpret_cast<unsigned char*>(file->data);
		auto offset = stbtt_GetFontOffsetForIndex(buf, index);
		if (offset < 0) throw parse_failure{path};
		if (!stbtt_InitFont(this, buf, offset)) throw parse_failure{path};
	}
}

