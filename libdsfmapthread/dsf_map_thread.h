#ifndef DSF_MAP_THREAD_H_
#define DSF_MAP_THREAD_H_

#include <GL/glew.h>

#include <mutex>
#include <vector>

struct FutureTexture
{
    FutureTexture() : dirty(true), width(0), height(0) {}

    void load()
    {
        guard.lock();
        if (!dirty)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buf.data());
        guard.unlock();
    }

    std::mutex guard;
    bool dirty;
    GLuint width, height;
    std::vector<GLubyte> buf;
};

class DsfMap;

class MapTextureProvider
{
    GLuint _width, _height;

    std::mutex _mtx;
    GLfloat _lon, _lat;
    GLfloat _scale_x, _scale_y;
    FutureTexture* _result;

    void work();

    volatile bool _working;
    std::thread _th;
    std::condition_variable _cv;

public:
    MapTextureProvider();
    MapTextureProvider(const MapTextureProvider&) = delete;
    MapTextureProvider& operator=(const MapTextureProvider&) = delete;
    ~MapTextureProvider();

    void start(GLuint width, GLuint height);
    void stop();

    void get(
        GLfloat lon, GLfloat lat,
        GLfloat scale_x, GLfloat scale_y,
        FutureTexture* result);
};

#endif
