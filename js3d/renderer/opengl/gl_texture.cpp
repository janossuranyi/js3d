#include "texture.h"
#include <glm/glm.hpp>

#undef _glm
#define _glm glm::

namespace js3d {

	Texture::Texture(Texture&& moved) noexcept
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

	Texture& Texture::operator=(Texture&& moved) noexcept
	{
		_textureId = moved.textureId();
		moved._textureId = 0xffff;

		return *this;
	}

	bool Texture::create_2d_default(int w, int h, int channels, const void* bytes)
	{
		GLuint texid;
		GLenum er;
		GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
		const _glm vec4 b_Color{0.0f, 0.0f, 0.0f, 1.0f};

		while (glGetError() != GL_NO_ERROR) {};

		glGenTextures(1, &texid);
		glBindTexture(GL_TEXTURE_2D, texid);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &b_Color[0]);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, format, GL_UNSIGNED_BYTE, bytes);

		er = glGetError();

		if (bytes && er == GL_NO_ERROR)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		
		_textureId = texid;
		_target = GL_TEXTURE_2D;

		return er == GL_NO_ERROR;
	}

	void Texture::bind(int unit) const
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, _textureId);
	}

	bool Texture::equal(const Texture& other)
	{
		return _textureId == other._textureId;
	}
	void Texture::unbind()
	{
		glBindTexture(_target, 0);
	}
}
