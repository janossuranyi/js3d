#ifndef JS3D_TEXTURE_H
#define JS3D_TEXTURE_H

#include <GL/glew.h>

namespace js3d {

	class Texture
	{
	public:
		Texture() : _textureId(0xFFFF) {}
		Texture(Texture&) = delete;
		Texture(Texture&&);
		~Texture() noexcept;
		Texture& operator=(Texture&) = delete;
		Texture& operator=(Texture&&);
		bool create_2d_default(int w, int h, int channels, const void* bytes);

		void bind(int unit);
	private:
		GLuint textureId() const { return _textureId; }
		GLuint _textureId;
		int _w, _h, _channels;
	};

}
#endif // !JS3D_TEXTURE_H
