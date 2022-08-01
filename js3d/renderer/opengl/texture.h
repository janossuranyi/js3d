#ifndef JS3D_TEXTURE_H
#define JS3D_TEXTURE_H

#include <GL/glew.h>
#include <string>
#include <compare>

namespace js3d {

	class Texture 
	{
	public:
		Texture() : Texture("<none>") {}
		Texture(const std::string& name) :_textureId(0xFFFF), _w(-1), _h(-1), _channels(-1), _name(name), _target(GL_TEXTURE_2D) {}
		Texture(Texture&) = delete;
		Texture(Texture&&) noexcept;
		~Texture() noexcept;
		Texture& operator=(Texture&) = delete;
		Texture& operator=(Texture&&) noexcept;
		bool create_2d_default(int w, int h, int channels, const void* bytes);

		void bind(int unit) const;
		void set_name(const std::string& name)
		{
			_name = name;
		}
		std::string name() const { return _name; }
		bool equal(const Texture& other);
		void unbind();

	private:
		GLuint textureId() const { return _textureId; }
		GLuint _textureId;
		GLenum _target;
		int _w, _h, _channels;
		std::string _name;
	};

}
#endif // !JS3D_TEXTURE_H
