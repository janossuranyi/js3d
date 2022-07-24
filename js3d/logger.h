#ifndef JS3D_LOGGER_H
#define JS3D_LOGGER_H

#include <string>
#include <cstdio>

namespace js3d {

	class LogWriter
	{
	public:
		LogWriter(const std::string& pFilename);
		~LogWriter();

		void write(const std::string& pMessage);
		void set_file_name(const std::string& pFilename);
		void clear();
	private:
		void reopen_file();
		FILE* pFile;
		std::string fileName;
	};

	void info(const char* fmt, ...);
	void warning(const char* fmt, ...);
	void error(const char* fmt, ...);
	void fatal_error(const char* fmt, ...);
}

#endif
