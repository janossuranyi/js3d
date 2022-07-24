#ifndef JS3D_BUFFER_H
#define JS3D_BUFFER_H

#include <cinttypes>
#include <GL/glew.h>

namespace js3d {

	enum class eBufferUsage { STATIC, DYNAMIC };

	class VertexBuffer {
		friend class DisplayManager;

	public:
		VertexBuffer(eBufferUsage usage, uint32_t size, const void* data);
		VertexBuffer() : _bufferId(0xFFFF), _size(0) {};
		VertexBuffer(VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&&);
		VertexBuffer& operator=(VertexBuffer&) = delete;
		VertexBuffer& operator=(VertexBuffer&&);
		~VertexBuffer() noexcept;
		void init(eBufferUsage usage, uint32_t size, const void* data);
		void bind();
		uint32_t size() const { return _size; }
		void update_data(uint32_t offset, uint32_t size, const void* bytes, bool forceBind = false);
	private:
		GLuint bufferId() const { return _bufferId; }
		GLuint _bufferId;
		uint32_t _size;
	};

	class IndexBuffer {
		friend class DisplayManager;
	public:
		IndexBuffer(eBufferUsage usage, uint32_t size, const void* data);
		IndexBuffer() : _bufferId(0xFFFF), _size(0) {};
		IndexBuffer(IndexBuffer&) = delete;
		IndexBuffer(IndexBuffer&&);
		IndexBuffer& operator=(IndexBuffer&) = delete;
		IndexBuffer& operator=(IndexBuffer&&);
		~IndexBuffer() noexcept;
		void init(eBufferUsage usage, uint32_t size, const void* data);
		void bind();
		uint32_t size() const { return _size; }
		void update_data(uint32_t offset, uint32_t size, const void* bytes, bool forceBind = false);
	private:
		GLuint bufferId() const { return _bufferId; }
		GLuint _bufferId;
		uint32_t _size;
	};

	class UniformBuffer {
		friend class DisplayManager;
	public:
		UniformBuffer(eBufferUsage usage, uint32_t size, const void* data);
		UniformBuffer() : _bufferId(0xFFFF), _size(0) {};
		UniformBuffer(UniformBuffer&) = delete;
		UniformBuffer(UniformBuffer&&);
		UniformBuffer& operator=(UniformBuffer&) = delete;
		UniformBuffer& operator=(UniformBuffer&&);
		~UniformBuffer() noexcept;
		void init(eBufferUsage usage, uint32_t size, const void* data);
		void bind(int index);
		void bind();
		uint32_t size() const { return _size; }
		void bind_range(int index, uint32_t offset, uint32_t size);
		void update_data(uint32_t offset, uint32_t size, const void* bytes, bool forceBind = false);
	private:
		GLuint bufferId() const { return _bufferId; }
		GLuint _bufferId;
		uint32_t _size;
	};
}
#endif // !JS3D_BUFFER_H
