#ifndef JS3D_FILESYSTEM_H
#define JS3D_FILESYSTEM_H

#include <cinttypes>
#include <filesystem>
#include <functional>
#include <string>

namespace js3d {

	class FileSystem
	{
	public:
		FileSystem();
		void set_working_dir(const std::string&);
		std::string working_dir() const;
		std::string resolve(const std::string&) const;
		bool read_text_file(const std::string&, std::string&);
		bool read_text_file_base(const std::string&, std::string&);
		std::vector<uint8_t> read_binary_file(const std::string& aFileName);
		std::vector<std::string> get_directory_entries(const std::string& dirname, const char* filter = nullptr);
		void get_directory_entries(const std::string& dirname, const std::function<void(const std::string&)>& fn, const char* filter = nullptr);
	private:
		std::filesystem::path m_working_dir;
	};
}
#endif

