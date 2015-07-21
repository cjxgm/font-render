#pragma once
#include "font.hh"
#include <vector>
#include <string>

namespace tue
{
	using font_list = std::vector<tue::font>;
	font_list list_fonts(std::string const& family={});
}

