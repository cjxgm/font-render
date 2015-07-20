#include "map-file.hh"
#include <cstring>	// for strerror
#include <cerrno>

// for open, fstat, close
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// for mmap, munmap
#include <sys/mman.h>


namespace tue
{
	namespace
	{
		struct fatal_unmap_failure {};
	}

	file_map::~file_map()
	{
		if (munmap(data, size) < 0)
			throw fatal_unmap_failure{};
	}

	file_map::uptr map_file(std::string const& path)
	{
		auto fd = open(path.c_str(), O_RDONLY);
		if (fd < 0)
			throw map_file_exceptions::open_failure{
				path + ": " + std::strerror(errno)};

		struct stat st;
		if (fstat(fd, &st) < 0)
			throw map_file_exceptions::stat_failure{
				path + ": " + std::strerror(errno)};

		auto addr = mmap({}, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
		if (addr == MAP_FAILED)
			throw map_file_exceptions::map_failure{
				path + ": " + std::strerror(errno)};
		close(fd);

		return file_map::uptr{
			new file_map{addr, static_cast<size_t>(st.st_size)}};
	}
}

