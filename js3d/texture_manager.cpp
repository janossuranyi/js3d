
#include <cassert>
#include "texture_manager.h"

namespace js3d {

	int TextureManager::add(Texture& t)
	{
		int x = find_tex(t);
		if (x == -1)
		{
			_textures.push_back(std::move(t));
			return _textures.size() - 1;
		}
		else
		{
			return x;
		}
	}

	Texture& TextureManager::operator[](int i)
	{
		assert(i < _textures.size());
		return _textures[i];
	}

	Texture& TextureManager::at(int i)
	{
		return _textures.at(i);
	}

	void TextureManager::bind(int i) const
	{
		assert(i < _textures.size());
		_textures[i].bind(i);
	}

	void TextureManager::bind(int t, int u) const
	{
		assert(t < _textures.size());
		_textures[t].bind(u);
	}

	int TextureManager::find_tex(const Texture& t)
	{
		int i = 0;
		for (; i < _textures.size(); ++i)
		{
			if (_textures[i].equal(t)) return i;
		}

		return -1;
	}

	TextureManager g_textureManager;
}