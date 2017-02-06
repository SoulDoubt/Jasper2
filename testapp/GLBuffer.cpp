#include "GLBuffer.h"
#include <cassert>
#include <GL/glew.h>

namespace Jasper
{

GLBuffer::GLBuffer() : GLBuffer(BufferType::VERTEX)
{
}

GLBuffer::GLBuffer(GLBuffer::BufferType type) : m_type(type), m_usage(Usage::StaticDraw)
{
}


GLBuffer::~GLBuffer()
{
    if (IsCreated()) {
        Destroy();
    }
}

bool GLBuffer::Create()
{
    glGenBuffers(1, &m_bufferID);
    if (m_bufferID > 0) {
        return true;
    }
    return false;
}

void GLBuffer::Destroy()
{
    printf("Destroying buffer with id: %d\n", m_bufferID);
    if (m_bufferID < 1){
        int x = 0;
    }
    assert(m_bufferID > 0);
    if (m_bufferID > 0) {
        glDeleteBuffers(1, &m_bufferID);
    }
}

bool GLBuffer::IsCreated() const
{
    return m_bufferID != 0;
}

bool GLBuffer::Bind()
{
    assert(m_bufferID != 0);
    if (m_bufferID > 0) {
        glBindBuffer(m_type, m_bufferID);
        return true;
    }
    return false;
}

void GLBuffer::Release()
{
    assert(m_bufferID != 0);
    if (m_bufferID > 0) {
        glBindBuffer(m_type, 0);
    }
}

GLuint GLBuffer::BufferID() const
{
    return m_bufferID;
}

void GLBuffer::SetUsage(Usage usage)
{
    m_usage = usage;
}

void GLBuffer::Allocate(const void * data, uint count)
{
    assert(m_bufferID != 0);
    if (m_bufferID > 0) {
        glBufferData(m_type, count, data, m_usage);
    }

}

void GLBuffer::UpdateContents(uint offset, uint size, const void* data)
{
    assert(m_bufferID != 0);
    if (m_bufferID > 0) {
        glBufferSubData(m_type, offset, size, data);
    }

}

int GLBuffer::Size()
{
    assert(m_bufferID > 0);
    int sz = -1;
    glGetBufferParameteriv(m_type, GL_BUFFER_SIZE, &sz);
    return sz;
}

void* GLBuffer::MapForReading(){
    return glMapBuffer(m_type, GL_WRITE_ONLY);
}

void GLBuffer::Unmap(){
    glUnmapBuffer(m_type);
}

}
