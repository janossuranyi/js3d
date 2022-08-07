#include "gltf_loader.h"
#include "logger.h"

namespace js3d {
    
    bool GltfLoader::load(const std::string& filename, std::string& err, std::string& warn)
    {
        auto found= filename.find(".glb");
        bool result{ false };

        if (found == std::string::npos)
        {
            result = _loader.LoadASCIIFromFile(&_model, &err, &warn, filename, 0);
        }
        else
        {
            result = _loader.LoadBinaryFromFile(&_model, &err, &warn, filename, 0);
        }

		info("Scene warning: %s, scene error: %s", warn.c_str(), err.c_str());

		if (result)
		{
			info("==================================================");
			info("Scene info: %s", filename.c_str());
			info("==================================================");
			info("asset.generator: %s", _model.asset.generator.c_str());
			info("asset.version:   %s", _model.asset.version.c_str());
			info("extension.used:  %d", _model.extensionsUsed.size());
			for (auto e : _model.extensionsUsed)
			{
				info("extension: %s", e.c_str());
			}

			info("scenes:    %d", _model.scenes.size());
			info("nodes:     %d", _model.nodes.size());
			info("ligths:    %d", _model.lights.size());
			info("cameras:   %d", _model.cameras.size());
			info("meshes:    %d", _model.meshes.size());
			info("materials: %d", _model.materials.size());
			info("textures:  %d", _model.textures.size());
			info("binary data size: %d", _model.buffers[0].data.size());
			info("==================================================");

		}

        _loaded = result;
        return result;
    }

}
