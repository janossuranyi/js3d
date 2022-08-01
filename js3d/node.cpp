#include "node.h"

namespace js3d {
    void Node::for_each(std::function<void(int)> fn)
    {
        for (int x : _children) fn(x);
    }

    nodeType_t Node::type() const
    {
        if (_mesh > -1) return MESH_NODE;
        else if (_light > -1) return LIGHT_NODE;
        else if (_camera > -1) return CAMERA_NODE;
        else return EMPTY_NODE;
    }
   
}
