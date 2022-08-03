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
	VertexBuffer::VertexBuffer(VertexBuffer&& moved)
	{
		_bufferId = moved.bufferId();
		moved._bufferId = 0xFFFF;
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
	}
	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& moved)
	{
		_bufferId = moved.bufferId();
		moved._bufferId = 0xFFFF;

		return *this;
	}
	VertexBuffer::~VertexBuffer()
	{
		if(_bufferId != 0xFFFF)
			glDeleteBuffers(1, &_bufferId);
	}
	void VertexBuffer::bind() const
	{
		assert(_bufferId != 0xFFFF);
		glBindBuffer(GL_ARRAY_BUFFER, bufferId());
	}
	void VertexBuffer::update_data(uint32_t offset, uint32_t size, const void* bytes, bool forceBind)
	{
		if (g_renderSystem.glVersion() >= 450)
		{
			glNamedBufferSubData(bufferId(), GLintptr(offset), GLsizeiptr(size), bytes);
		}
		else
		{
			if (forceBind) bind();
			glBufferSubData(GL_ARRAY_BUFFER, GLintptr(offset), GLsizeiptr(size), bytes);
		}
	}
	IndexBuffer::IndexBuffer(eBufferUsage usage, uint32_t size, const void* data)
	{
		init(usage, size, data);
	}
	IndexBuffer::IndexBuffer(IndexBuffer&& moved)
	{
		_bufferId = moved.bufferId();
		moved._bufferId = 0xFFFF;
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
	}
	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& moved)
	{
		_bufferId = moved.bufferId();
		moved._bufferId = 0xFFFF;

		return *this;
	}
	IndexBuffer::~IndexBuffer()
	{
		if(_bufferId != 0xFFFF)
			glDeleteBuffers(1, &_bufferId);
	}
	void IndexBuffer::bind() const
	{
		assert(_bufferId != 0xFFFF);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId());
	}
	void IndexBuffer::update_data(uint32_t offset, uint32_t size, const void* bytes, bool forceBind)
	{
		if (g_renderSystem.glVersion() >= 450)
		{
			glNamedBufferSubData(bufferId(), GLintptr(offset), GLsizeiptr(size), bytes);
		}
		else
		{
			if (forceBind) bind();
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GLintptr(offset), GLsizeiptr(size), bytes);
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
	UniformBuffer& UniformBuffer::operator=(UniformBuffer&& moved)
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