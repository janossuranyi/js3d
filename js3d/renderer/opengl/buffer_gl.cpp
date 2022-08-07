#include <cstring>
#include <cassert>
#include "buffer.h"
#include "render_system.h"

namespace js3d {

	static inline GLenum gl_buffer_usage_cast(eBufferUsage usage)
	{
		GLenum bufferUsage;
		switch (usage) {
		case eBufferUsage::STATIC:
			bufferUsage = GL_STATIC_DRAW;
			break;
		case eBufferUsage::DYNAMIC:
			bufferUsage = GL_DYNAMIC_DRAW;
			break;
		default:
			bufferUsage = GL_STATIC_DRAW;
		}

		return bufferUsage;
	}
	VertexBuffer::VertexBuffer(eBufferUsage usage, uint32_t size, const void* data) : _bufferId(0xffff)
	{
		init(usage, size, data);
	}
	VertexBuffer::VertexBuffer(VertexBuffer&& moved) noexcept
	{
		move(moved);
	}
	void VertexBuffer::init(eBufferUsage usage, uint32_t size, const void* data)
	{
		GLenum bufferUsage = gl_buffer_usage_cast(usage);

		glGenBuffers(1, &_bufferId);
		assert(_bufferId != 0xffff);
		glBindBuffer(GL_ARRAY_BUFFER, _bufferId);
		glBufferData(GL_ARRAY_BUFFER, GLsizei(size), data, bufferUsage);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		_size = size;
		_offset = 0;
		_usage = usage;
	}
	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& moved) noexcept
	{
		move(moved);
		return *this;
	}
	VertexBuffer::~VertexBuffer()
	{
		if (_bufferId != 0xFFFF && !_reference)
		{
			if (_mapped) unmap();
			glDeleteBuffers(1, &_bufferId);
		}
	}
	void VertexBuffer::bind() const
	{
		assert(_bufferId != 0xFFFF);
		glBindBuffer(GL_ARRAY_BUFFER, bufferId());
	}
	void VertexBuffer::update_data(uint32_t offset, uint32_t size, const void* bytes, bool forceBind)
	{
		assert((offset + _offset + size) < _size);

		if (_mapped)
		{
			memcpy(_mappedPtr + offset + _offset, bytes, size);
			return;
		}

		if (g_renderSystem.glVersion() >= 450)
		{
			glNamedBufferSubData(bufferId(), GLintptr(offset+_offset), GLsizeiptr(size), bytes);
		}
		else
		{
			if (forceBind) bind();
			glBufferSubData(GL_ARRAY_BUFFER, GLintptr(offset+_offset), GLsizeiptr(size), bytes);
		}
	}
	uint8_t* VertexBuffer::map_write()
	{
		assert(_bufferId != 0xFFFF && !_reference);

		if (_mapped) return _mappedPtr;

		glBindBuffer(GL_ARRAY_BUFFER, bufferId());
		if ((_mappedPtr = (uint8_t*)glMapBufferRange(GL_ARRAY_BUFFER, 0, _size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT)))
		{
			_mapped = true;
		}

		return _mappedPtr;
	}
	void VertexBuffer::unmap()
	{
		assert(_bufferId != 0xFFFF);

		if (_mapped)
		{
			glBindBuffer(GL_ARRAY_BUFFER, bufferId());
			glUnmapBuffer(GL_ARRAY_BUFFER);
			_mapped = false;
			_mappedPtr = nullptr;
		}
	}
	void VertexBuffer::create_reference(uint32_t offset, uint32_t size, VertexBuffer& ref)
	{
		ref._reference = true;
		ref._mapped = _mapped;
		ref._bufferId = _bufferId;
		ref._size = size;
		ref._offset = _offset;
		ref._mappedPtr = _mappedPtr + _offset;
	}
	void VertexBuffer::move(VertexBuffer& moved)
	{
		_bufferId = moved.bufferId();
		_mapped = moved._mapped;
		_mappedPtr = moved._mappedPtr;
		_size = moved._size;
		_offset = moved._offset;
		_usage = moved._usage;
		moved._bufferId = 0xFFFF;
		moved._mapped = false;
		moved._mappedPtr = nullptr;
		moved._size = 0;
	}
	IndexBuffer::IndexBuffer(eBufferUsage usage, uint32_t size, const void* data)
	{
		init(usage, size, data);
	}
	IndexBuffer::IndexBuffer(IndexBuffer&& moved) noexcept
	{
		_bufferId = moved.bufferId();
		_mapped = moved._mapped;
		_mappedPtr = moved._mappedPtr;
		_size = moved._size;
		_offset = moved._offset;
		_usage = moved._usage;
		moved._bufferId = 0xFFFF;
		moved._mapped = false;
		moved._mappedPtr = nullptr;
		moved._size = 0;
	}
	void IndexBuffer::init(eBufferUsage usage, uint32_t size, const void* data)
	{
		GLenum bufferUsage = gl_buffer_usage_cast(usage);

		glGenBuffers(1, &_bufferId);
		assert(_bufferId != 0xffff);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _bufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizei(size), data, bufferUsage);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		_size = size;
		_offset = 0;
		_usage = usage;
	}
	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& moved) noexcept
	{
		_bufferId = moved.bufferId();
		_mapped = moved._mapped;
		_mappedPtr = moved._mappedPtr;
		_size = moved._size;
		_offset = moved._offset;
		_usage = moved._usage;
		moved._bufferId = 0xFFFF;
		moved._mapped = false;
		moved._mappedPtr = nullptr;
		moved._size = 0;

		return *this;
	}
	uint8_t* IndexBuffer::map_write()
	{
		assert(_bufferId != 0xFFFF && !_reference);

		if (_mapped) return _mappedPtr;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId());
		if ((_mappedPtr = (uint8_t*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, _size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT)))
		{
			_mapped = true;
		}

		return _mappedPtr;
	}
	void IndexBuffer::unmap()
	{
		assert(_bufferId != 0xFFFF);

		if (_mapped)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId());
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			_mapped = false;
			_mappedPtr = nullptr;
		}
	}

	void IndexBuffer::create_reference(uint32_t offset, uint32_t size, IndexBuffer& ref)
	{
		ref._reference = true;
		ref._mapped = _mapped;
		ref._bufferId = _bufferId;
		ref._size = size;
		ref._offset = offset;
		ref._mappedPtr = _mappedPtr + offset;
	}

	IndexBuffer::~IndexBuffer()
	{
		if (_bufferId != 0xFFFF && !_reference)
		{
			if (_mapped) unmap();
			glDeleteBuffers(1, &_bufferId);
		}
	}
	void IndexBuffer::bind() const
	{
		assert(_bufferId != 0xFFFF);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId());
	}
	void IndexBuffer::update_data(uint32_t offset, uint32_t size, const void* bytes, bool forceBind)
	{
		assert((offset + _offset + size) < _size);

		if (_mapped)
		{
			memcpy(_mappedPtr + offset + _offset, bytes, size);
			return;
		}

		if (g_renderSystem.glVersion() >= 450)
		{
			glNamedBufferSubData(bufferId(), GLintptr(offset+_offset), GLsizeiptr(size), bytes);
		}
		else
		{
			if (forceBind) bind();
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GLintptr(offset+_offset), GLsizeiptr(size), bytes);
		}
	}
	UniformBuffer::UniformBuffer(eBufferUsage usage, uint32_t size, const void* data)
	{
		init(usage, size, data);
	}
	UniformBuffer::UniformBuffer(UniformBuffer&& moved)
	{
		_bufferId = moved.bufferId();
		moved._bufferId = 0xFFFF;
	}
	UniformBuffer& UniformBuffer::operator=(UniformBuffer&& moved) noexcept
	{
		_bufferId = moved.bufferId();
		moved._bufferId = 0xFFFF;
		
		return *this;
	}
	UniformBuffer::~UniformBuffer() noexcept
	{
		if(_bufferId != 0xFFFF)
			glDeleteBuffers(1, &_bufferId);
	}
	void UniformBuffer::init(eBufferUsage usage, uint32_t size, const void* data)
	{
		GLenum bufferUsage = gl_buffer_usage_cast(usage);

		glGenBuffers(1, &_bufferId);
		assert(_bufferId != 0xffff);

		glBindBuffer(GL_UNIFORM_BUFFER, _bufferId);
		glBufferData(GL_UNIFORM_BUFFER, GLsizei(size), data, bufferUsage);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		_size = size;
	}
	void UniformBuffer::bind(int index) const
	{
		assert(_bufferId != 0xFFFF);
		glBindBufferBase(GL_UNIFORM_BUFFER, GLuint(index), bufferId());
	}
	void UniformBuffer::bind() const
	{
		assert(_bufferId != 0xffff);
		glBindBuffer(GL_UNIFORM_BUFFER, bufferId());
	}
	void UniformBuffer::bind_range(int index, uint32_t offset, uint32_t size)
	{
		assert(_bufferId != 0xffff);
		glBindBufferRange(GL_UNIFORM_BUFFER, GLuint(index), bufferId(), offset, size);
	}
	void UniformBuffer::update_data(uint32_t offset, uint32_t size, const void* bytes, bool forceBind)
	{
		if (g_renderSystem.glVersion() >= 450)
		{
			glNamedBufferSubData(bufferId(), GLintptr(offset), GLsizeiptr(size), bytes);
		}
		else
		{
			if (forceBind) bind();
			glBufferSubData(GL_UNIFORM_BUFFER, GLintptr(offset), GLsizeiptr(size), bytes);
		}
	}
}