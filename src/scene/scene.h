#ifndef SCENE_H
#define SCENE_H

#include <Frame.h>
#include <memory>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/constants.hpp> // glm::pi

class Renderer;
class Scene {
public:
    Scene();
    virtual ~Scene();

    virtual int init(Renderer*);
    virtual void draw(const  glm::mat4 &viewMat, const  glm::mat4 &projMat, const glm::mat4 modelMat = glm::mat4(1.0)) = 0;

    void glCheckError();

    Renderer *renderer() const;

private:
    virtual int initEgl() = 0;
    Renderer* _renderer;
};

typedef std::shared_ptr<Scene> ScenePtr;

#endif
