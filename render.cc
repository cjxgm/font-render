// run: apitrace trace -o $exec.trace $exec && qapitrace $exec.trace
// run-: valgrind --leak-check=full $exec
#include <string>
#include <memory>
#include <cmath>
#include "list-fonts.hh"	// lib: fontconfig
#include "distance-transform.hh"

// lib: gl
#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>

// lib: glfw3
#include <GLFW/glfw3.h>

#include <iostream>
using std::cerr;


namespace font
{
	/*
	template <int MAGNITUDE=10>
	struct atlas
	{
		static constexpr auto size() { return (1 << MAGNITUDE); }
		atlas() : tex{make_texture(size(), size())} {}

	private:
		gl::texture tex;
	};
	*/
}

namespace
{
	auto make_shader(GLenum type, std::string const& src)
	{
		auto s = glCreateShader(type);
		auto c = src.c_str();
		glShaderSource(s, 1, &c, {});
		glCompileShader(s);
		return s;
	}

	auto make_program(std::string const& vss, std::string const& fss)
	{
		auto p = glCreateProgram();
		glAttachShader(p, make_shader(GL_VERTEX_SHADER, vss));
		glAttachShader(p, make_shader(GL_FRAGMENT_SHADER, fss));
		glLinkProgram(p);
		return p;
	}

	float invalid_pattern(int x, int y, int size)
	{
		constexpr auto len = 8;
		auto cx = x - size/2;
		auto cy = y - size/2;
		auto dist = (float(cx*cx + cy*cy)) / len / 35;
		if (dist > 1) dist = 1;
		x = (x + y) % (len * 2);
		x = std::abs(x - len);
		return 1 - std::min(x / (float)len, dist);
	}

	auto invalid_pattern(int magnitude)
	{
		const auto size = (1 << magnitude);
		std::unique_ptr<float[]> p{new float[size*size]};
		for (int y=0; y<size; y++)
			for (int x=0; x<size; x++)
				p[(y<<magnitude)|x] = invalid_pattern(x, y, size);
		return p;
	}
}

int main()
{
	auto fonts = tue::list_fonts("WenQuanYi Micro Hei");
	cerr << fonts.size() << " fonts available\n";

	auto fa = fonts[0].render(U'猫', 1024);
	fa.padding(fa.h()/8, fa.w()/8);
	tue::distance_transform(fa);
	fa.clerp(-20, 50, 1, 0);
	fa.scale_to_height(64);

	// init host
	glfwInit();
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	auto win = glfwCreateWindow(16*40, 16*40, "demo", {}, {});
	glfwMakeContextCurrent(win);
	glfwSwapInterval(1);

	// setup debugging
	struct programmers_fault {};
	glDebugMessageCallback([](auto, auto, auto, auto, auto, auto msg, auto) {
		static std::string last;
		if (last == msg) throw programmers_fault{};
		printf("\e[1;31m%s\e[0m\n", msg);
		last = msg;
	}, {});

	// init gl
	glClearColor(1, 1, 1, 1);
	{	// array buffer
		GLuint id;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}
	{	// array object
		GLuint id;
		glGenVertexArrays(1, &id);
		glBindVertexArray(id);
	}
	{	// texture
		GLuint id;
		glGenTextures(1, &id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	constexpr auto tex_mag = 8;
	constexpr auto tex_size = float(1 << tex_mag);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F,
			1<<tex_mag, 1<<tex_mag, 0, GL_RED, GL_FLOAT,
			invalid_pattern(tex_mag).get());
	glTexSubImage2D(GL_TEXTURE_2D, 0,
			0, 0, fa.w(), fa.h(),
			GL_RED, GL_FLOAT, fa.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	static float data[] = {
		// points
		-1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0,
		-0.4, -0.4, 0.4, -0.4, -0.4, 0.4, 0.4, 0.4,
		// uvs
		0.4, 0.4, 0.7, 0.4, 0.4, 0.7, 0.7, 0.7,
		0, 0, fa.w()/tex_size, 0, 0, fa.h()/tex_size, fa.w()/tex_size, fa.h()/tex_size,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, (void*)(8*2*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// enable blending
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// create program
	auto filler = make_program(
			R"vertex(#version 330 core
layout(location=0) in vec2 pos;
layout(location=1) in vec2 _uv;
out vec2 uv_;
out vec2 x_;
out vec2 y_;
uniform float time;
uniform int rotv;	// rotate vertex

void main()
{
	float c = cos(time);
	float s = sin(time);
	x_ = vec2(c, s);
	y_ = vec2(-s, c);
	if (rotv == 1) gl_Position = vec4(pos.x*x_ + pos.y*y_, 0, 1);
	else gl_Position = vec4(pos, 0, 1);
	uv_ = _uv;
}
			)vertex",
			R"fragment(#version 330 core
in vec2 uv_;
in vec2 x_;
in vec2 y_;
out vec4 color;
uniform sampler2D tex;
uniform int rotv;	// rotate vertex

void main()
{
	//vec2 p = (gl_FragCoord.x * x_ + gl_FragCoord.y * y_) / 1000;
	vec2 p;
	if (rotv == 1) p = uv_;
	else p = uv_.x * x_ + uv_.y * y_;

	float a = texture(tex, p).r;
	float ina = smoothstep(0.8-fwidth(a)+0.07, 0.8+0.07, a);
	float outa = smoothstep(0.8-fwidth(a), 0.8, a);
	color.rgb = mix(vec3(0, 0, 0), vec3(1, 0.5, 0), ina);
	color.a = outa;
}
			)fragment");
	glUseProgram(filler);
	glUniform1i(glGetUniformLocation(filler, "tex"), 0);
	int utime = glGetUniformLocation(filler, "time");
	int urotv = glGetUniformLocation(filler, "rotv");

	// mainloop
	while (!glfwWindowShouldClose(win)) {
		// resize
		int w, h;
		glfwGetFramebufferSize(win, &w, &h);
		glViewport(0, 0, w, h);

		// render
		glClear(GL_COLOR_BUFFER_BIT);
		glUniform1f(utime, glfwGetTime()/10);
		glUniform1i(urotv, false);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glUniform1i(urotv, true);
		glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);


		// event dispatching
		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	glfwTerminate();
}

