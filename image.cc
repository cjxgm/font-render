#include "image.hh"
#include "stb_image_resize.inl"
#include <utility>	// for std::swap

namespace tue
{
	void image::padding(int n, int s, int w, int e)
	{
		auto nw = w_ + w + e;
		auto nh = h_ + n + s;
		image tmp{nw, nh};
		for (int y=0; y<h_; y++)
			for (int x=0; x<w_; x++) {
				auto nx = w + x;
				auto ny = s + y;
				if (nx < 0) continue;
				if (ny < 0) continue;
				if (nx >= nw) continue;
				if (ny >= nh) continue;
				tmp.at(nx, ny) = at(x, y);
			}
		std::swap(*this, tmp);
	}

	void image::scale_to(int nw, int nh)
	{
		image tmp{nw, nh};
		stbir_resize_float(data(), w_, h_, 0, tmp.data(), nw, nh, 0, 1);
		std::swap(*this, tmp);
	}
}

