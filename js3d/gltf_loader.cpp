#include "gltf_loader.h"

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

        _loaded = result;
        return result;
    }

}
