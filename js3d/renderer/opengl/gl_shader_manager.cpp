#include "js3d.h"
#include <memory>
#include <cstdio>

#undef _std
#define _std std::

namespace js3d {

	extern FileSystem g_fileSystem;

	ShaderProgram::ShaderProgram()
	{
		_progId = 0xffff;
		for (unsigned i = 0; i < NUM_INTERNAL_SAMPLERS; ++i)
		{
			_sampler_lut[i] = 0xffff;
		}
	}

	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& moved)
	{
		_progId = moved._progId;
		_std memcpy(_sampler_lut, moved._sampler_lut, sizeof(_sampler_lut));
		_std memset(moved._sampler_lut, 0xFFFF, sizeof(_sampler_lut));

		moved._progId = 0xFFFF;

		return *this;
	}
	ShaderProgram::ShaderProgram(ShaderProgram&& moved)
	{
		_progId = moved._progId;
		_std memcpy(_sampler_lut, moved._sampler_lut, sizeof(_sampler_lut));
		_std memset(moved._sampler_lut, 0xFFFF, sizeof(_sampler_lut));

		moved._progId = 0xFFFF;
	}
	ShaderProgram::~ShaderProgram()
	{
		destroy();
	}
	bool ShaderProgram::create_program_from_shader_source(int numVertShader, const char** vert_sources, int numFragShader, const char** frag_sources)
	{
		if (_progId != 0xFFFF)
		{
			glDeleteProgram(_progId);
		}

		_progId = glCreateProgram();

		GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertShader, numVertShader, vert_sources, nullptr);

		if (!compile_single_stage(vertShader, eShaderType::VERTEX))
		{
			destroy();
			return false;
		}

		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragShader, numFragShader, frag_sources, nullptr);

		if (!compile_single_stage(fragShader, eShaderType::FRAGMENT))
		{
			glDeleteShader(vertShader);
			destroy();
			return false;
		}

		glAttachShader(_progId, vertShader);
		glAttachShader(_progId, fragShader);
		glLinkProgram(_progId);

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);

		GLint result = GL_FALSE;

		glGetProgramiv(_progId, GL_LINK_STATUS, &result);

		if (result == GL_FALSE)
		{
			GLint infologLen;
			glGetProgramiv(_progId, GL_INFO_LOG_LENGTH, &infologLen);
			if (infologLen > 0) {
				std::vector<char> logBuf(infologLen);
				glGetProgramInfoLog(_progId, infologLen, nullptr, logBuf.data());
				error("Linking of shader program failed: %s", logBuf.data());

				destroy();
				return false;
			}
		}

		glUseProgram(_progId);

		char str[100];
		for (unsigned i = 0; i < NUM_INTERNAL_SAMPLERS; ++i)
		{
			::snprintf(str, 100, "samp%d", i);
			int loc = get_uniform_location(str);
			if (loc >= 0)
			{
				_sampler_lut[i] = loc;
			}
		}

		glUseProgram(0);

		return true;
	}
	bool ShaderProgram::set(const std::string name, float p0)
	{
		assert(_progId != 0xFFFF);

		GLint loc = -1;
		if ((loc = get_uniform_location(name)) > -1) {
			glUniform1f(loc, GLfloat(p0));
			return true;
		}
		return false;
	}
	bool ShaderProgram::set(const std::string name, int p0)
	{
		assert(_progId != 0xFFFF);

		GLint loc = -1;
		if ((loc = get_uniform_location(name)) > -1) {
			glUniform1i(loc, GLint(p0));
			return true;
		}
		return false;
	}
	bool ShaderProgram::set(const std::string name, const float* ptr, int numFloats)
	{
		assert(_progId != 0xFFFF);

		GLint loc = -1;
		if ((loc = get_uniform_location(name)) > -1) {
			glUniform1fv(loc, numFloats, (GLfloat*)ptr);
			return true;
		}
		return false;
	}
	bool ShaderProgram::set(const std::string name, const glm::vec2& p0)
	{
		assert(_progId != 0xFFFF);

		GLint loc = -1;
		if ((loc = get_uniform_location(name)) > -1) {
			glUniform2f(loc, GLfloat(p0.x), GLfloat(p0.y));
			return true;
		}
		return false;
	}
	bool ShaderProgram::set(const std::string name, const glm::vec3& p0)
	{
		assert(_progId != 0xFFFF);

		GLint loc = -1;
		if ((loc = get_uniform_location(name)) > -1) {
			glUniform3f(loc, GLfloat(p0.x), GLfloat(p0.y), GLfloat(p0.z));
			return true;
		}
		return false;
	}
	bool ShaderProgram::set(const std::string name, const glm::vec4& p0)
	{
		assert(_progId != 0xFFFF);

		GLint loc = -1;
		if ((loc = get_uniform_location(name)) > -1) {
			glUniform4f(loc, GLfloat(p0.x), GLfloat(p0.y), GLfloat(p0.z), GLfloat(p0.w));
			return true;
		}
		return false;
	}
	bool ShaderProgram::set(const std::string name, const glm::vec4* ptr, int numVec4)
	{
		assert(_progId != 0xFFFF);

		GLint loc = -1;
		if ((loc = get_uniform_location(name)) > -1) {
			glUniform4fv(loc, numVec4, (GLfloat*)ptr);
			return true;
		}
		return false;
	}
	bool ShaderProgram::set(const std::string name, const glm::mat4& p0)
	{
		assert(_progId != 0xFFFF);

		GLint loc = -1;
		if ((loc = get_uniform_location(name)) > -1) {
			glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat*)&p0);
			return true;
		}
		return false;
	}
	bool ShaderProgram::set(const std::string name, const glm::mat3& p0)
	{
		assert(_progId != 0xFFFF);

		GLint loc = -1;
		if ((loc = get_uniform_location(name)) > -1) {
			glUniformMatrix3fv(loc, 1, GL_FALSE, (GLfloat*)&p0);
			return true;
		}
		return false;
	}
	bool ShaderProgram::set(const std::string name, const glm::mat4* p, int numMat)
	{
		assert(_progId != 0xFFFF);

		GLint loc = -1;
		if ((loc = get_uniform_location(name)) > -1) {
			glUniformMatrix4fv(loc, numMat, GL_FALSE, (GLfloat*)p);
			return true;
		}
		return false;
	}
	bool ShaderProgram::set(const std::string name, const glm::mat3* p, int numMat)
	{
		assert(_progId != 0xFFFF);

		GLint loc = -1;
		if ((loc = get_uniform_location(name)) > -1) {
			glUniformMatrix3fv(loc, numMat, GL_FALSE, (GLfloat*)p);
			return true;
		}
		return false;
	}
	void ShaderProgram::set_sampler_unit(int sampler, int unit)
	{
		assert(sampler < 3);
		if (_sampler_lut[sampler] != 0xFFFF)
		{
			glUniform1i(_sampler_lut[sampler], unit);
		}
	}
	void ShaderProgram::use()
	{
		assert(_progId != 0xFFFF);
		glUseProgram(_progId);
	}
	void ShaderProgram::destroy()
	{
		if (_progId != 0xFFFF)
		{
			glDeleteProgram(_progId);
			_progId = 0xFFFF;
		}
	}
	bool ShaderProgram::compile_single_stage(GLuint shaderId, eShaderType type)
	{
		GLint result = GL_FALSE;

		glCompileShader(shaderId);
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);

		if (result == GL_FALSE)
		{
			GLint infologLen;
			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infologLen);
			if (infologLen > 0) {
				std::vector<char> logBuf(infologLen);
				glGetShaderInfoLog(shaderId, infologLen, nullptr, logBuf.data());
				std::string sType = type == eShaderType::VERTEX ? "Vertex" : "Fragment";
				error("%s shader compilation failed: %s", sType.c_str(), logBuf.data());
			}

			glDeleteShader(shaderId);

			return false;
		}

		return true;
	}
	GLint ShaderProgram::get_uniform_location(const std::string& name)
	{
		assert(_progId != 0xFFFF);
		if (g_displayManager.glVersion() >= 430)
		{
			return glGetProgramResourceLocation(_progId, GL_UNIFORM, name.c_str());
		}
		else
		{
			return glGetUniformLocation(_progId, name.c_str());
		}
	}
	ShaderProgram& ShaderManager::get_program(int index)
	{
		assert(index < _programs.size());
		return _programs[index];
	}

	void ShaderManager::init()
	{
		{
			// simple pass-through
			ShaderProgram p;
			_std vector<const char*> vs;
			_std vector<const char*> fs;

			_std string vtx1, vtx2, frag;
			g_fileSystem.read_text_file_base("shaders/drawvert_layout.inc.glsl", vtx1);
			g_fileSystem.read_text_file_base("shaders/basic_vtx.glsl", vtx2);
			g_fileSystem.read_text_file_base("shaders/basic_frag.glsl", frag);

			vs.push_back("#version 330 core\n");
			vs.push_back(vtx1.c_str());
			vs.push_back(vtx2.c_str());
			fs.push_back(frag.c_str());

			if (p.create_program_from_shader_source(vs.size(), vs.data(), fs.size(), fs.data()))
			{
				_programs.push_back(std::move(p));
				int id = int(_programs.size() - 1);
				info("%d: simple pass-through program added", id);
			}
		}
	}
	void ShaderManager::use_program(int index)
	{
		assert(index < _programs.size());
		_programs[index].use();
	}
}
