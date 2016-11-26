#include "drawable_object.h"
#include "gl_check_error.h"

DrawableObject::DrawableObject()
{
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);
}

DrawableObject::DrawableObject(DrawableObject&& rhs)
    :
    _VAO(rhs._VAO),
    _VBO(rhs._VBO),
    _EBO(rhs._EBO)
{
    rhs._VAO = 0;
    rhs._VBO = 0;
    rhs._EBO = 0;
}

DrawableObject::~DrawableObject()
{
    if (_EBO)
        glDeleteBuffers(1, &_EBO);
    if (_VBO)
        glDeleteBuffers(1, &_VBO);
    if (_VAO)
        glDeleteVertexArrays(1, &_VAO);
}

void DrawableObject::bind() const
{
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    GL_CHECK_ERRORS;
}

void DrawableObject::unbind() const
{
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    GL_CHECK_ERRORS;
}

void DrawableObject::loadData(const GLvoid* data, GLsizeiptr size)
{
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_CHECK_ERRORS;
}

void DrawableObject::loadElements(const GLvoid* data, GLsizeiptr size)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    GL_CHECK_ERRORS;
}

void DrawableObject::setAttrib(GLuint index, GLint size, GLenum type, GLsizei stride, GLsizei offset)
{
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

    glVertexAttribPointer(index, size, type, GL_FALSE, stride, static_cast<char*>(0) + offset);
    glEnableVertexAttribArray(index);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GL_CHECK_ERRORS;
}

void DrawElements::setParam(GLenum mode, GLsizei count, GLenum type, GLsizei offset)
{
    _mode = mode;
    _count = count;
    _type = type;
    _offset = offset;
}

void DrawElements::operator() () const
{
    bind();
    glDrawElements(_mode, _count, _type, static_cast<char*>(0) + _offset);
    GL_CHECK_ERRORS;
}