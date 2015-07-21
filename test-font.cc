#include "font.hh"
#include "font-prober.hh"
#include <string>
#include <vector>
#include <iostream>
// lib: fontconfig
// run: valgrind --leak-check=full $exec
// run-: time $exec

namespace
{
	struct font_info
	{
		font_info(std::string path, int index) : font{path, index}, path{std::move(path)}, index{index} {}
		tue::font font;
		std::string path;
		int index;
	};

	auto list_fonts(std::string const& family)
	{
		std::vector<font_info> fonts;
		namespace fp = font_prober;
		for (auto& f: fp::list<fp::file, fp::index>(fp::family{family}))
			try {
				fonts.emplace_back(f.file, f.index);
			}
			catch (tue::parse_failure const&) { std::cerr << "skip " << f.index << ": " << f.file << "\n"; }
		return fonts;
	}
}

int main()
{
	for (auto& f: list_fonts("Fantasque Sans Mono"))
		std::cerr << f.index << ": " << f.path << "\n";
}


