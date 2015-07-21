#include "list-fonts.hh"
#include "font-prober.hh"

namespace tue
{
	namespace
	{
		namespace fp = font_prober;
	}

	font_list list_fonts(std::string const& family)
	{
		font_list fonts;
		for (auto f: fp::list<fp::file, fp::index>(fp::family{family}))
			try { fonts.emplace_back(f.file, f.index); }
			catch (tue::parse_failure const&) {}
		return fonts;
	}
}

