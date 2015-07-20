#pragma once
#include "constraints.hh"
#include <cstdint>	// for std::size
#include <memory>
#include <string>

namespace tue
{
	struct file_map final : constraints::non_transferable
	{
		using self = file_map;
		using uptr = std::unique_ptr<self>;

		void* data;
		std::size_t size;

	private:
		file_map(void* data, std::size_t size) : data{data}, size{size} {}
		~file_map();

		friend uptr::deleter_type;
		friend uptr map_file(std::string const& path);
	};

	inline namespace map_file_exceptions
	{
		struct open_failure : std::runtime_error { using runtime_error::runtime_error; };
		struct stat_failure : std::runtime_error { using runtime_error::runtime_error; };
		struct  map_failure : std::runtime_error { using runtime_error::runtime_error; };
	}
}

