#ifndef JS3D_TEXTURE_MANAGER_H
#define JS3D_TEXTURE_MANAGER_H

#include <vector>
#include <type_traits>
#include "texture.h"

namespace js3d {
	class TextureManager
	{
	public:
		TextureManager() = default;
		~TextureManager() noexcept = default;

		int add(Texture& t);
		Texture& get(int i) const;
		
		Texture& operator[](int i);
		Texture& at(int i);

		void bind(int i) const;
		void bind(int t, int u) const;

	private:
		int find_tex(const Texture& t);
		std::vector<Texture> _textures;
	};

	extern TextureManager g_textureManager;
}
#endif // !JS3D_TEXTURE_MANAGER_H
