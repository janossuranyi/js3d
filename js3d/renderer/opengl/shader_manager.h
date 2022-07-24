#ifndef JS3D_SHADER_MANAGER_H
#define JS3D_SHADER_MANAGER_H
#include <vector>
#include <string>
#include <memory>
#include <GL/glew.h>

namespace js3d {

	enum class eShaderType { VERTEX, FRAGMENT, GEOMETRY, COMPUTE };

	class ShaderProgram
	{
	public:
		ShaderProgram() : _progId(0xFFFF) {};
		ShaderProgram(ShaderProgram& copied) = delete;
		ShaderProgram(ShaderProgram&& moved);
		~ShaderProgram();

		bool create_program_from_shader_source(const std::vector<std::string>& vert_sources, const std::vector<std::string>& frag_sources);

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

		void use();
		void destroy();

	private:
		bool compile_single_stage(GLuint shaderId, eShaderType type);
		GLint get_uniform_location(const std::string& name);
		GLuint _progId;
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
		
		void init();
		void use_program(int index);

	private:
		std::vector<ShaderProgram> _programs;
	};
}
#endif // !JS3D_SHADER_MANAGER_H
