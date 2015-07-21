#include "image.hh"
#include "stb_image_resize.inl"
#include <utility>	// for std::swap

namespace
{
	auto clerp(float s, float smin, float smax, float dmin, float dmax)
	{
		if (s < smin) return dmin;
		if (s > smax) return dmax;
		return (s - smin) / (smax - smin) * (dmax - dmin) + dmin;
	}
}

namespace tue
{
	void image::padding(int n, int s, int w, int e)
	{
		auto nw = w_ + w + e;
		auto nh = h_ + n + s;
		image tmp{nw, nh};
		each([&](auto& v, auto x, auto y) {
			auto nx = w + x;
			auto ny = s + y;
			if (nx < 0) return;
			if (ny < 0) return;
			if (nx >= nw) return;
			if (ny >= nh) return;
			tmp.at(nx, ny) = v;
		});
		std::swap(*this, tmp);
	}

	void image::scale_to(int nw, int nh)
	{
		image tmp{nw, nh};
		stbir_resize_float(data(), w_, h_, 0, tmp.data(), nw, nh, 0, 1);
		std::swap(*this, tmp);
	}

	void image::clerp(float smin, float smax, float dmin, float dmax)
	{
		each([&](auto& v, auto, auto) {
			v = ::clerp(v, smin, smax, dmin, dmax);
		});
	}
}

