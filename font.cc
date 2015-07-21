#include "font.hh"
#include "stb_truetype.inl"
#include <memory>
#include <cstdlib>	// for "free"

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

	auto font::render(std::uint32_t codepoint, int height) -> image
	{
		int w, h;
		std::unique_ptr<unsigned char[], decltype(&free)> pixmap{
			stbtt_GetCodepointBitmap(this,
					0, stbtt_ScaleForPixelHeight(this, height),
					codepoint, &w, &h, {}, {}),
			&free
		};

		image img{w, h};
		img.each([&](auto& v, auto x, auto y) {
			v = pixmap[(h-y-1)*w + x] / 255.0f;
		});

		return img;
	}
}

