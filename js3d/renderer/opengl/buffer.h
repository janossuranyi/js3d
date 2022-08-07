#ifndef JS3D_BUFFER_H
#define JS3D_BUFFER_H

#include <cinttypes>
#include <GL/glew.h>

namespace js3d {

	enum class eBufferUsage { STATIC, DYNAMIC };

	class VertexBuffer {
		friend class RenderSystem;

	public:
		VertexBuffer(eBufferUsage usage, uint32_t size, const void* data);
		VertexBuffer() : _bufferId(0xFFFF), _size(0), _mapped(false), _reference(false), _mappedPtr(), _usage(eBufferUsage::STATIC), _offset(0) {};
		VertexBuffer(VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&&) noexcept;
		VertexBuffer& operator=(VertexBuffer&) = delete;
		VertexBuffer& operator=(VertexBuffer&&) noexcept;
		~VertexBuffer() noexcept;
		void init(eBufferUsage usage, uint32_t size, const void* data);
		void bind() const;
		uint32_t size() const { return _size; }
		uint32_t offset() const { return _offset; }
		void update_data(uint32_t offset, uint32_t size, const void* bytes, bool forceBind = false);
		uint8_t* map_write();
		//uint8_t* map_write_range(uint32_t offset, uint32_t size);
		void unmap();

		eBufferUsage usage() const { return _usage; }
		void create_reference(uint32_t offset, uint32_t size, VertexBuffer& ref);
	private:
		void move(VertexBuffer& other);
		GLuint bufferId() const { return _bufferId; }
		GLuint _bufferId;
		eBufferUsage _usage;

		uint32_t _size;
		uint32_t _offset;
		uint8_t* _mappedPtr;

		bool _mapped;
		bool _reference;
	};

	class IndexBuffer {
		friend class RenderSystem;
	public:
		IndexBuffer(eBufferUsage usage, uint32_t size, const void* data);
		IndexBuffer() : _bufferId(0xFFFF), _size(0), _mapped(false), _reference(false), _mappedPtr(), _usage(eBufferUsage::STATIC) {};
		IndexBuffer(IndexBuffer&) = delete;
		IndexBuffer(IndexBuffer&&) noexcept;
		IndexBuffer& operator=(IndexBuffer&) = delete;
		IndexBuffer& operator=(IndexBuffer&&) noexcept;
		~IndexBuffer() noexcept;
		void init(eBufferUsage usage, uint32_t size, const void* data);
		void bind() const;
		uint32_t size() const { return _size; }
		uint32_t offset() const { return _offset; }
		void update_data(uint32_t offset, uint32_t size, const void* bytes, bool forceBind = false);
		uint8_t* map_write();
		//uint8_t* map_write_range(uint32_t offset, uint32_t size);
		void unmap();
		eBufferUsage usage() const { return _usage; }
		void create_reference(uint32_t offset, uint32_t size, IndexBuffer& ref);
	private:
		GLuint bufferId() const { return _bufferId; }
		GLuint _bufferId;
		uint32_t _size;
		uint32_t _offset;
		uint8_t* _mappedPtr;
		eBufferUsage _usage;

		bool _mapped;
		bool _reference;
	};

	class UniformBuffer {
		friend class RenderSystem;
	public:
		UniformBuffer(eBufferUsage usage, uint32_t size, const void* data);
		UniformBuffer() : _bufferId(0xFFFF), _size(0), _mapped(false), _reference(false), _mappedPtr() {};
		UniformBuffer(UniformBuffer&) = delete;
		UniformBuffer(UniformBuffer&&);
		UniformBuffer& operator=(UniformBuffer&) = delete;
		UniformBuffer& operator=(UniformBuffer&&) noexcept;
		~UniformBuffer() noexcept;
		void init(eBufferUsage usage, uint32_t size, const void* data);
		void bind(int index) const;
		void bind() const;
		uint32_t size() const { return _size; }
		void bind_range(int index, uint32_t offset, uint32_t size);
		void update_data(uint32_t offset, uint32_t size, const void* bytes, bool forceBind = false);
	private:
		GLuint bufferId() const { return _bufferId; }
		GLuint _bufferId;
		uint32_t _size;
		uint8_t* _mappedPtr;

		bool _mapped;
		bool _reference;
	};
}
#endif // !JS3D_BUFFER_H
