#include "texture.h"
namespace js3d {

	Texture::Texture(Texture&& moved)
	{
		_textureId = moved.textureId();
		moved._textureId = 0xffff;
	}

	Texture::~Texture() noexcept
	{
		if (textureId() != 0xFFFF)
		{
			glDeleteTextures(1, &_textureId);
		}
	}

	Texture& Texture::operator=(Texture&& moved)
	{
		_textureId = moved.textureId();
		moved._textureId = 0xffff;

		return *this;
	}

	bool Texture::create_2d_default(int w, int h, int channels, const void* bytes)
	{
		GLuint texid;
		
		glGenTextures(1, &texid);
		glBindTexture(GL_TEXTURE_2D, texid);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, bytes);
		if (bytes)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		
		_textureId = texid;

		return true;
	}

}
