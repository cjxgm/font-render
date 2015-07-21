#pragma once
#include <vector>

namespace tue
{
	struct image
	{
		image(int w, int h) : w_{w}, h_{h}, img(w_*h_) {}

		auto& at(int x, int y)       { return img[y*w_ + x]; }
		auto& at(int x, int y) const { return img[y*w_ + x]; }
		auto data()       { return img.data(); }
		auto data() const { return img.data(); }

		auto w() const { return w_; }
		auto h() const { return h_; }

		void padding(int n, int s, int w, int e);
		void padding(int ns, int we) { padding(ns, ns, we, we); }
		void padding(int nswe) { padding(nswe, nswe); }

		void scale_to(int nw, int nh);
		void scale_to_height(float nh) { scale_to(nh * w_/h_, nh); };
		void scale(float sx, float sy) { scale_to(w_*sx, h_*sy); }
		void scale(float s) { scale(s, s); };

	private:
		int w_;
		int h_;
		std::vector<float> img;
	};
}

