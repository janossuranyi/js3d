#ifndef JS3D_SHADER_MANAGER_H
#define JS3D_SHADER_MANAGER_H
#include <vector>
#include <string>
#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "render_common.h"

namespace js3d {

	const int DEFAULT_PBR_SHADER = 0;
	const int DEFAULT_DEPTH_SHADER = 1;

	class ShaderProgram
	{
		friend class DisplayManager;
	public:

		static const int NUM_INTERNAL_SAMPLERS = 3;

		ShaderProgram();
		ShaderProgram(ShaderProgram& copied) = delete;
		ShaderProgram& operator=(ShaderProgram&) = delete;
		ShaderProgram& operator=(ShaderProgram&&);
		ShaderProgram(ShaderProgram&& moved);
		~ShaderProgram();

		bool create_program_from_shader_source(int numVertShader, const char** vert_sources, int numFragShader, const char** frag_sources);

		bool set(const std::string name, float p0) const;
		bool set(const std::string name, int p0) const;
		bool set(const std::string name, const float* ptr, int numFloats) const;
		bool set(const std::string name, const glm::vec2& p0) const;
		bool set(const std::string name, const glm::vec3& p0) const;
		bool set(const std::string name, const glm::vec4& p0) const;
		bool set(const std::string name, const glm::vec4* ptr, int numVec4) const;
		bool set(const std::string name, const glm::mat4& p0) const;
		bool set(const std::string name, const glm::mat3& p0) const;
		bool set(const std::string name, const glm::mat4* p, int numMat) const;
		bool set(const std::string name, const glm::mat3* p, int numMat) const;

		void set_sampler_unit(int, int) const;

		void use() const;
		void destroy();

	private:
		GLuint programId() const { return _progId; }
		bool compile_single_stage(GLuint shaderId, shaderType_t type);
		GLint get_uniform_location(const std::string& name) const;
		GLuint _progId;
		GLuint _sampler_lut[NUM_INTERNAL_SAMPLERS]{};
	};

	class ShaderManager
	{
	public:
		const static int SHADER_DEFAULT_PBR = 0;
		ShaderManager() = default;
		~ShaderManager() noexcept = default;
		ShaderManager(ShaderManager&) = delete;
		ShaderManager(ShaderManager&&) = delete;
		ShaderManager& operator=(ShaderManager&) = delete;
		ShaderManager& operator=(ShaderManager&&) = delete;
		
		ShaderProgram const& get_program(int index) const;

		void init();
		void use_program(int index);

	private:
		std::vector<ShaderProgram> _programs;
	};

	extern ShaderManager g_shaderManager;
}
#endif // !JS3D_SHADER_MANAGER_H
