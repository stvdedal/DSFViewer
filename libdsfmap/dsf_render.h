#ifndef DSF_RENDER_H_
#define DSF_RENDER_H_

#include <drawable_object.h>
#include <glm/glm.hpp>
#include <list>


class DsfRender
{
    DrawElements _rectangle;
    std::list<DrawElements> _dsf_objects;

    glm::mat4 _transform;
public:
    DsfRender(const DsfRender&) = delete;
    DsfRender& operator= (const DsfRender&) = delete;

    DsfRender();
    DsfRender(DsfRender&& rhs);
    virtual ~DsfRender();

    void load(int lon, int lat);
    void setTransform(const glm::mat4& m);
    void render(GLuint transformLoc) const;

    int age;
};

#endif
