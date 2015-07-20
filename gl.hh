#pragma once
#include "nullable.hh"
#include <memory>
#include <string>
#include <initializer_list>

#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>

namespace gl
{
	///////////////////////////////////////////////////////////////////////
	inline namespace texture_detail
	{
		struct texture_deleter
		{
			using pointer = tue::nullable<GLuint>;
			void operator () (GLuint tex) { glDeleteTextures(1, &tex); }
		};

		using texture = std::unique_ptr<void, texture_deleter>;

		inline auto make_texture()
		{
			GLuint tex;
			glGenTextures(1, &tex);
			return texture{tex};
		}

		inline void bind(texture const& tex)
		{
			glBindTexture(GL_TEXTURE_2D, tex.get());
		}

		inline void upload(texture const& tex, int w, int h, float img[] = nullptr)
		{
			bind(tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0, GL_RED, GL_FLOAT, img);
		}

		inline void upload(texture const& tex, int w, int h, float img[], int x, int y)
		{
			bind(tex);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RED, GL_FLOAT, img);
		}

		inline auto make_texture(int w, int h)
		{
			auto tex = make_texture();
			upload(tex, w, h);
			return tex;
		}
	};


	///////////////////////////////////////////////////////////////////////
	inline namespace shader_detail
	{
		struct shader_deleter
		{
			using pointer = tue::nullable<GLuint>;
			void operator () (GLuint s) { glDeleteShader(s); }
		};

		using shader = std::unique_ptr<void, shader_deleter>;

		inline auto make_shader(GLenum type)
		{
			return shader{glCreateShader(type)};
		}

		inline auto upload(shader const& s, std::string const& src)
		{
			GLchar const* cstr = src.c_str();
			GLint size = src.size();
			glShaderSource(s.get(), 1, &cstr, &size);
		}

		inline auto compile(shader const& s)
		{
			glCompileShader(s.get());
		}

		inline auto make_shader(GLenum type, std::string const& src)
		{
			auto s = make_shader(type);
			upload(s, src);
			compile(s);
			return s;
		}

		inline auto make_vertex_shader(std::string const& src)
		{
			return make_shader(GL_VERTEX_SHADER, src);
		}

		inline auto make_fragment_shader(std::string const& src)
		{
			return make_shader(GL_FRAGMENT_SHADER, src);
		}
	}


	///////////////////////////////////////////////////////////////////////
	inline namespace program_detail
	{
		struct program_deleter
		{
			using pointer = tue::nullable<GLuint>;
			void operator () (GLuint p) { glDeleteProgram(p); }
		};

		using program = std::unique_ptr<void, program_deleter>;

		inline auto make_program()
		{
			return program{glCreateProgram()};
		}

		template <class ...SHADERS>
		inline auto attach(program const& p, SHADERS const& ... ss)
		{
			(void)(int[]){ 0, (glAttachShader(p.get(), ss.get()), 0)... };
		}

		inline auto link(program const& p)
		{
			glLinkProgram(p.get());
		}

		template <class ...SHADERS>
		inline auto make_program_from(SHADERS const& ... ss)
		{
			auto p = make_program();
			attach(p, ss...);
			link(p);
			return p;
		}

		inline auto make_program(std::string const& vs, std::string const& fs)
		{
			return make_program_from(
					make_vertex_shader(vs),
					make_fragment_shader(fs));
		}

		inline auto use(program const& p)
		{
			glUseProgram(p.get());
		}
	}


	///////////////////////////////////////////////////////////////////////
	inline namespace buffer_detail
	{
		struct buffer_deleter
		{
			using pointer = tue::nullable<GLuint>;
			void operator () (GLuint buf) { glDeleteBuffers(1, &buf); }
		};

		using buffer = std::unique_ptr<void, buffer_deleter>;

		inline auto make_buffer()
		{
			GLuint buf;
			glGenBuffers(1, &buf);
			return buffer{buf};
		}

		inline void bind(buffer const& buf)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buf.get());
		}

		inline void upload(buffer const& buf)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buf.get());
		}
	}
}

