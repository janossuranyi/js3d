#ifndef JS3D_NODE_H
#define JS3D_NODE_H

#include <vector>
#include <functional>
#include <cinttypes>

namespace js3d {

	enum nodeType_t
	{
		EMPTY_NODE = -1,
		MESH_NODE,
		LIGHT_NODE,
		CAMERA_NODE,
	};

	class Node
	{
	public:

		using childNodesType = std::vector<int>;
		using childNodesIt = childNodesType::iterator;

		Node() = default;
		~Node() noexcept = default;

		int parent() const { return _parent; }
		int mesh() const { return _mesh; }
		int light() const { return _light; }
		int camera() const { return _camera; }
		size_t size() const { return _children.size(); }
		const childNodesType& children() const { return _children; }
		void for_each(std::function<void(int)> fn);

		nodeType_t type() const;
	private:

		int _parent{ -1 };
		int _mesh{ -1 };
		int _light{ -1 };
		int _camera{ -1 };
		bool _updated{ false };

		float _position[3]{ 0.0f,0.0f,0.0f };
		float _rotation[4]{ 0.0f,0.0f,0.0f,1.0f };
		float _scale[3]{ 1.0f,1.0f,1.0f };

		float _localMtx[16]{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		float _worldMtx[16]{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		childNodesType _children;
	};

}
#endif // !JS3D_NODE_H
