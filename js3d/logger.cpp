#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <time.h>

#include "SDL.h"
#include "logger.h"

namespace js3d {
	static LogWriter g_LogWriter("js3d.log");

	void LogWriter::write(const std::string& msg)
	{
		if (!pFile) reopen_file();
		if (pFile)
		{
			fprintf(pFile, "%s\n", msg.c_str());
			fprintf(stderr, "%s\n", msg.c_str());
			fflush(pFile);
		}
	}

	LogWriter::LogWriter(const std::string& fileName_)
	{
		fileName = fileName_;
		pFile = nullptr;
	}

	LogWriter::~LogWriter()
	{
		if (pFile) {
			fclose(pFile);
		}
	}

	void LogWriter::clear()
	{
		reopen_file();
		if (pFile) fflush(pFile);
	}

	void LogWriter::reopen_file()
	{
		if (pFile) {
			fclose(pFile);
			pFile = nullptr;
		}

		pFile = fopen(fileName.c_str(), "a");
	}

	void LogWriter::set_file_name(const std::string& fileName_)
	{
		if (fileName_ == fileName)
			return;

		fileName = fileName_;
		if (pFile) reopen_file();
	}

	static std::string jse_get_time()
	{
		char text[100];

		time_t now = time(NULL);
		const tm* my_time = localtime(&now);
		strftime(text, 100, "[%Y-%m-%d %H:%M:%S]", my_time);

		return std::string(text);
	}

	void info(const char* fmt, ...)
	{
		char text[2048];
		va_list ap;

		if (fmt == NULL)
			return;

		va_start(ap, fmt);
		vsnprintf(text, 2048, fmt, ap);
		va_end(ap);

		g_LogWriter.write(jse_get_time() + "[INFO] " + std::string(text));
	}

	void warning(const char* fmt, ...)
	{
		char text[2048];
		va_list ap;

		if (fmt == NULL)
			return;

		va_start(ap, fmt);
		vsnprintf(text, 2048, fmt, ap);
		va_end(ap);

		g_LogWriter.write(jse_get_time() + "[WARN] " + std::string(text));
	}

	void error(const char* fmt, ...)
	{
		char text[2048];
		va_list ap;

		if (fmt == NULL)
			return;

		va_start(ap, fmt);
		vsnprintf(text, 2048, fmt, ap);
		va_end(ap);

		g_LogWriter.write(jse_get_time() + "[ERROR] " + std::string(text));
	}

	void fatal_error(const char* fmt, ...)
	{
		char text[2048];
		va_list ap;

		if (fmt == NULL)
			return;

		va_start(ap, fmt);
		vsnprintf(text, 2048, fmt, ap);
		va_end(ap);

		g_LogWriter.write(jse_get_time() + "[FATAL] " + std::string(text));

		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "JSE ERROR", text, NULL);
		SDL_Quit();

		exit(1);
	}

}