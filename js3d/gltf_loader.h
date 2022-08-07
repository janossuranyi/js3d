#ifndef JS3D_GLTF_LOADER_H
#define JS3D_GLTF_LOADER_H

#include <string>
#include <vector>
#include <tiny_gltf.h>

#include "draw_vert.h"

#undef _tiny
#define _tiny tinygltf::

namespace js3d {
	class GltfLoader
	{
	public:
		GltfLoader() {}
		~GltfLoader() {}
		bool load(const std::string& filename, std::string& err, std::string& warn);
	private:
		std::string _loadedFile;
		_tiny Model _model;
		_tiny TinyGLTF _loader;
		bool _loaded{ false };
	};
}
#endif // !JS3D_GLTF_LOADER_H
