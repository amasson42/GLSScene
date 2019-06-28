
#include "SceneController.hpp"

ISceneController::ISceneController(AppEnv *e) : env(e) {
    mustUpdate = true;
}

ISceneController::~ISceneController() {

}

std::shared_ptr<GLS::Scene> ISceneController::scene() {
    return env->scene;
}

void ISceneController::update() {
    GLFWwindow *window = env->window;
    GLS::Scene *scene = env->scene.get();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (!scene->cameraNode().expired()) {
        GLS::Node& cam(*scene->cameraNode().lock());
        static float cameraAngleX = cam.transform().eulerAngles().x;
        static float cameraAngleY = cam.transform().eulerAngles().y;

        float cameraSpeed = 5.0 * env->deltaTime;
        glm::mat4 cameraMat = cam.getWorldTransformMatrix();
        glm::vec3 cameraFront = glm::vec3(cameraMat * glm::vec4(0, 0, -1, 0));
        glm::vec3 cameraRight = glm::vec3(cameraMat * glm::vec4(1, 0, 0, 0));
        glm::vec3 cameraUp = glm::vec3(cameraMat * glm::vec4(0, 1, 0, 0));

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cam.transform().moveBy(cameraSpeed * cameraFront);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cam.transform().moveBy(-cameraSpeed * cameraFront);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cam.transform().moveBy(-cameraSpeed * cameraRight);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cam.transform().moveBy(cameraSpeed * cameraRight);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            cam.transform().moveBy(cameraSpeed * cameraUp);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            cam.transform().moveBy(-cameraSpeed * cameraUp);

        float cameraRotateSpeed = 3.0 * env->deltaTime;
        bool changeCamera = true;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            cameraAngleY += cameraRotateSpeed;
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            cameraAngleY -= cameraRotateSpeed;
        else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            cameraAngleX -= cameraRotateSpeed;
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            cameraAngleX += cameraRotateSpeed;
        else
            changeCamera = false;

        if (changeCamera)
            cam.transform().setEulerAngles(cameraAngleX, cameraAngleY, 0);
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        mustUpdate = false;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        mustUpdate = true;
    }
}

void ISceneController::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)window;
    (void)key;
    (void)scancode;
    (void)action;
    (void)mods;
    std::cout << "{key: " << key << ", scancode: " << scancode
        << ", action: " << action << ", mods: " << mods << "}" << std::endl;
    if (key == GLFW_KEY_G && action == 1) {
        if (glIsEnabled(GL_FRAMEBUFFER_SRGB))
            glDisable(GL_FRAMEBUFFER_SRGB);
        else
            glEnable(GL_FRAMEBUFFER_SRGB);
    }
}
