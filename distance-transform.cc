#include "distance-transform.hh"
#include <limits>
#include <algorithm>	// for std::min

namespace
{
	inline namespace distance_transform_detail
	{
		using limits = std::numeric_limits<float>;
		static_assert(limits::is_iec559, "IEEE-754 floating point required for infinity, quiet_NaN and epsilon.");
		constexpr auto inf = limits::infinity();
		constexpr auto nan = limits::quiet_NaN();
		constexpr auto eps = limits::epsilon();


		namespace sqrt2_detail
		{
			constexpr auto impl()
			{
				auto l = 0.0f, r=2.0f;
				while (l+eps < r) {
					auto m = (l+r) / 2.0f;
					if (m*m > 2.0f) r = m;
					else l = m;
				}
				return l;
			}
		}
		constexpr auto sqrt2 = sqrt2_detail::impl();


		void prepare(tue::image & img, bool inside)
		{
			auto  inside_value = (inside ?  inf : 0.0f);
			auto outside_value = (inside ? 0.0f :  inf);
			img.each([&](auto& v, auto, auto) {
				v = (v > 0.5f ? inside_value : outside_value);
			});
		}

		auto get_or_inf(tue::image & img, int x, int y)
		{
			if (x < 0) return inf;
			if (y < 0) return inf;
			if (x >= img.w()) return inf;
			if (y >= img.h()) return inf;
			return img.at(x, y);
		}

		auto transform(tue::image & img, int x, int y, int d)
		{
			auto p00 = get_or_inf(img, x  , y  );
			auto p10 = get_or_inf(img, x+d, y  );
			auto p01 = get_or_inf(img, x  , y+d);
			auto p11 = get_or_inf(img, x+d, y+d);
			auto p21 = get_or_inf(img, x-d, y+d);
			return std::min({
					p00,
					p10+1.0f, p01+1.0f,
					p11+sqrt2, p21+sqrt2});
		}

		void transform(tue::image & img)
		{
			img. each([&](auto& v, auto x, auto y) { v = transform(img, x, y, -1); });
			img.reach([&](auto& v, auto x, auto y) { v = transform(img, x, y, +1); });
		}
	}
}

namespace tue
{
	void distance_transform(image & img)
	{
		auto tmp = img;
		prepare(img, false);
		prepare(tmp,  true);
		transform(img);
		transform(tmp);

		// merge result
		img.each([&](auto& v, auto x, auto y) {
			if (!v) v = sqrt2 - tmp.at(x, y);
		});
	}
}

