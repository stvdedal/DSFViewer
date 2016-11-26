#ifndef DRAWABLE_OBJECT_H_
#define DRAWABLE_OBJECT_H_

#include <GL/glew.h>
#include <vector>

class DrawableObject
{
    GLuint _VAO, _VBO, _EBO;
public:
    DrawableObject(const DrawableObject&) = delete;
    DrawableObject& operator= (const DrawableObject&) = delete;

    DrawableObject();
    DrawableObject(DrawableObject&& rhs);
    virtual ~DrawableObject();

    void bind() const;
    void unbind() const;
    void loadData(const GLvoid* data, GLsizeiptr size);
    void loadElements(const GLvoid* data, GLsizeiptr size);
    void setAttrib(GLuint index, GLint size, GLenum type, GLsizei stride, GLsizei offset);
};

//
// glDrawElements wrapper
//
class DrawElements : public DrawableObject
{
    GLenum _mode;
    GLsizei _count;
    GLenum _type;
    GLsizei _offset;
public:
    void setParam(GLenum mode, GLsizei count, GLenum type, GLsizei offset);
    void operator() () const;
};

#endif
