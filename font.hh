#pragma once
#include "map-file.hh"
#include <string>
#include <stdexcept>

namespace tue
{
	struct font
	{
		//XXX: vvvv COPIED FROM stb_truetype ------------------------------
		//XXX: vvvv DO NOT TOUCH ------------------------------------------
		void           * userdata;
		unsigned char  * data;              // pointer to .ttf file
		int              fontstart;         // offset of start of font

		int numGlyphs;                     // number of glyphs, needed for range checking

		int loca,head,glyf,hhea,hmtx,kern; // table locations as offset from start of .ttf
		int index_map;                     // a cmap mapping for our chosen character encoding
		int indexToLocFormat;              // format needed to map from glyph index to glyph
		//XXX: ^^^^ COPIED FROM stb_truetype ------------------------------
		//XXX: ^^^^ DO NOT TOUCH ------------------------------------------

		font(std::string const& path, int index=0);

	private:
		file_map::uptr file;
	};

	inline namespace font_exceptions
	{
		struct parse_failure : std::runtime_error { using runtime_error::runtime_error; };
	};
}

