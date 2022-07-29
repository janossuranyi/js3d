#ifndef JS3D_SHADER_MANAGER_H
#define JS3D_SHADER_MANAGER_H
#include <vector>
#include <string>
#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace js3d {

	enum class eShaderType { VERTEX, FRAGMENT, GEOMETRY, COMPUTE };

	class ShaderProgram
	{
	public:

		static const int NUM_INTERNAL_SAMPLERS = 3;

		ShaderProgram();
		ShaderProgram(ShaderProgram& copied) = delete;
		ShaderProgram& operator=(ShaderProgram&) = delete;
		ShaderProgram& operator=(ShaderProgram&&);
		ShaderProgram(ShaderProgram&& moved);
		~ShaderProgram();

		bool create_program_from_shader_source(int numVertShader, const char** vert_sources, int numFragShader, const char** frag_sources);

		bool set(const std::string name, float p0);
		bool set(const std::string name, int p0);
		bool set(const std::string name, const float* ptr, int numFloats);
		bool set(const std::string name, const glm::vec2& p0);
		bool set(const std::string name, const glm::vec3& p0);
		bool set(const std::string name, const glm::vec4& p0);
		bool set(const std::string name, const glm::vec4* ptr, int numVec4);
		bool set(const std::string name, const glm::mat4& p0);
		bool set(const std::string name, const glm::mat3& p0);
		bool set(const std::string name, const glm::mat4* p, int numMat);
		bool set(const std::string name, const glm::mat3* p, int numMat);

		void set_sampler_unit(int, int);

		void use();
		void destroy();

	private:
		bool compile_single_stage(GLuint shaderId, eShaderType type);
		GLint get_uniform_location(const std::string& name);
		GLuint _progId;
		GLuint _sampler_lut[NUM_INTERNAL_SAMPLERS]{};
	};

	class ShaderManager
	{
	public:
		const static int SHADER_PASSTHROUGH = 0;
		ShaderManager() = default;
		~ShaderManager() noexcept = default;
		ShaderManager(ShaderManager&) = delete;
		ShaderManager(ShaderManager&&) = delete;
		ShaderManager& operator=(ShaderManager&) = delete;
		ShaderManager& operator=(ShaderManager&&) = delete;
		
		ShaderProgram& get_program(int index);

		void init();
		void use_program(int index);

	private:
		std::vector<ShaderProgram> _programs;
	};
}
#endif // !JS3D_SHADER_MANAGER_H
