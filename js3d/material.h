#ifndef JS3D_MATERIAL_H
#define JS3D_MATERIAL_H

#include <cassert>
#include <string>
#include "texture.h"
#include "shader_manager.h"
#include "texture_manager.h"

namespace js3d {

	typedef enum
	{
		MC_OPAQUE,		// completely fills triangle
		MC_PERFORATED,	// have alpha tested holes
		MC_TRANSLUCENT	// blended with background
	} materialCoverage_t;

	class Material
	{
	public:
		Material() : 
			_coverage(MC_OPAQUE),
			_shaderProg(DEFAULT_PBR_SHADER),
			_texture_diffuse(-1),
			_texture_normal(-1),
			_texture_mr(-1) {}

		~Material() noexcept = default;

		void set_shader(int p) { _shaderProg = p; }
		void set_coverage(materialCoverage_t const p) { _coverage = p; }
		void set_textures(int diffuse, int normal, int metallicRoughness)
		{
			_texture_diffuse = diffuse;
			_texture_normal = normal;
			_texture_mr = metallicRoughness;
		}

		int get_diffuse_texture() const { return _texture_diffuse; }
		int get_normal_texture() const { return _texture_normal; }
		int get_metallic_roughness_texture() const { return _texture_mr; }

		void bind_textures(int firstUnit) const
		{
			if (_texture_diffuse != -1)		g_textureManager.bind(_texture_diffuse,		firstUnit + 0);
			if (_texture_mr != -1)			g_textureManager.bind(_texture_mr,			firstUnit + 1);
			if (_texture_normal != -1)		g_textureManager.bind(_texture_normal,		firstUnit + 2);
		}

		materialCoverage_t coverage() const { return _coverage; }
		int shader() const { return _shaderProg; }

	private:
		int _texture_diffuse;
		int _texture_normal;
		int _texture_mr;
		int _shaderProg;
		materialCoverage_t _coverage;

	};
}
#endif // !JS3D_MATERIAL_H
