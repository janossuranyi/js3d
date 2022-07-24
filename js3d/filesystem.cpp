#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include "filesystem.h"
#include "logger.h"

namespace js3d {

	namespace fs = std::filesystem;

	FileSystem::FileSystem()
	{
		m_working_dir = fs::current_path();
	}

	void FileSystem::set_working_dir(const std::string& p0)
	{
		m_working_dir = fs::path{ p0 }.make_preferred();
	}

	std::string FileSystem::working_dir() const
	{
		return m_working_dir.generic_string();
	}

	std::string FileSystem::resolve(const std::string& aPath) const
	{
		const fs::path p = m_working_dir / aPath;
		return p.string();
	}

	bool FileSystem::read_text_file(const std::string& fileName, std::string& output)
	{

		std::ifstream ifs(fileName.c_str(), std::ios::in);
		if (ifs.is_open())
		{
			std::stringstream sstr;
			sstr << ifs.rdbuf();
			output = sstr.str();
			ifs.close();
		}
		else
		{
			error("Cannot load file %s", fileName.c_str());

			return false;
		}

		return true;
	}

	bool FileSystem::read_text_file_base(const std::string& filename, std::string& output)
	{
		return read_text_file(resolve(filename), output);
	}

	std::vector<uint8_t> FileSystem::read_binary_file(const std::string& filename)
	{
		std::vector<uint8_t> result;

		std::ifstream input{ filename, std::ios::binary };

		if (input.good())
		{
			// copies all data into buffer
			result = std::vector<uint8_t>{ std::istreambuf_iterator<char>(input), {} };

			input.close();
		}

		return result;
	}

	std::vector<std::string> FileSystem::get_directory_entries(const std::string& dirname, const char* filter)
	{
		std::vector<std::string> result;
		const fs::path path{ dirname };
		if (!filter)
		{
			for (auto const& e : fs::directory_iterator{ path })
			{
				if (e.is_regular_file()) {
					result.push_back(e.path().generic_string());
				}
			}
		}
		else
		{
			try
			{
				std::regex fr{ filter, std::regex::icase };
				for (auto const& e : fs::directory_iterator{ path })
				{
					if (e.is_regular_file() && std::regex_match(e.path().generic_string(), fr)) {
						result.push_back(e.path().generic_string());
					}
				}
			}
			catch (const std::exception& ex)
			{
				error("Invalid regexp (%s) %s", filter, ex.what());
			}

		}
		return result;
	}

	void FileSystem::get_directory_entries(const std::string& dirname, const std::function<void(const std::string&)>& fn, const char* filter)
	{
		for (auto e : get_directory_entries(dirname, filter)) {
			fn(e); 
		}
	}
}