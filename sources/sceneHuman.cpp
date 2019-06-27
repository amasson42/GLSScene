
#include "sceneTest.hpp"

using namespace GLS;

std::shared_ptr<GLS::Node> newNode() {
    return std::make_shared<GLS::Node>();
}

typedef std::shared_ptr<GLS::Node> T_Node;

float floatLinearValue(float a, float b, double t) {
    return ((b - a) * t + a);
}


glm::vec3 vec3LinearValue(glm::vec3 a, glm::vec3 b, double t) {
    return glm::vec3(floatLinearValue(a.x, b.x, t),
                    floatLinearValue(a.y, b.y, t),
                    floatLinearValue(a.z, b.z, t));
}

struct HumanAnimationState {
    glm::vec3 position;
    glm::vec3 torse;
    glm::vec3 tete;
    glm::vec3 bras_gauche;
    glm::vec3 avant_gauche;
    glm::vec3 bras_droit;
    glm::vec3 avant_droit;
    glm::vec3 cuisse_gauche;
    glm::vec3 mollet_gauche;
    glm::vec3 cuisse_droit;
    glm::vec3 mollet_droit;

    HumanAnimationState() :
        position(0),
        torse(0), tete(0),
        bras_gauche(0), avant_gauche(0),
        bras_droit(0), avant_droit(0),
        cuisse_gauche(0), mollet_gauche(0),
        cuisse_droit(0), mollet_droit(0)
        {}
    
    static HumanAnimationState linearValue(
    const HumanAnimationState& a,
    const HumanAnimationState& b,
    double timePercent) {
        HumanAnimationState r;
        r.position = vec3LinearValue(a.position, b.position, timePercent);
        r.torse = vec3LinearValue(a.torse, b.torse, timePercent);
        r.tete = vec3LinearValue(a.tete, b.tete, timePercent);
        r.bras_gauche = vec3LinearValue(a.bras_gauche, b.bras_gauche, timePercent);
        r.avant_gauche = vec3LinearValue(a.avant_gauche, b.avant_gauche, timePercent);
        r.bras_droit = vec3LinearValue(a.bras_droit, b.bras_droit, timePercent);
        r.avant_droit = vec3LinearValue(a.avant_droit, b.avant_droit, timePercent);
        r.cuisse_gauche = vec3LinearValue(a.cuisse_gauche, b.cuisse_gauche, timePercent);
        r.mollet_gauche = vec3LinearValue(a.mollet_gauche, b.mollet_gauche, timePercent);
        r.cuisse_droit = vec3LinearValue(a.cuisse_droit, b.cuisse_droit, timePercent);
        r.mollet_droit = vec3LinearValue(a.mollet_droit, b.mollet_droit, timePercent);
        return r;
    }

    bool readValuesFromWords(const std::vector<std::string>& words) {
        if (words.size() < 3)
            return false;
        
        const std::map<std::string, glm::vec3*> vecs = {
            {"position", &position},
            {"torse", &torse},
            {"cou", &tete},
            {"epaulegauche", &bras_gauche},
            {"coudegauche", &avant_gauche},
            {"epauledroite", &bras_droit},
            {"coudedroit", &avant_droit},
            {"jambegauche", &cuisse_gauche},
            {"genougauche", &mollet_gauche},
            {"jambedroite", &cuisse_droit},
            {"genoudroit", &mollet_droit}
        };
        
        auto it = vecs.find(words[0]);
        if (it == vecs.end())
            return false;
        
        float value = std::atof(words[2].c_str());

        if (words[1] == "x") {
            it->second->x = value;
        } else if (words[1] == "y") {
            it->second->y = value;
        } else if (words[1] == "z") {
            it->second->z = value;
        } else {
            return false;
        }

        return true;

    }

};

std::ostream& operator<<(std::ostream& out, const HumanAnimationState& has) {
    out << has.torse;
    return out;
}

class Human {

    T_Node _node;

    T_Node _torse;
    T_Node _tete;

    T_Node _bras_gauche;
    T_Node _avant_gauche;

    T_Node _bras_droit;
    T_Node _avant_droit;

    T_Node _cuisse_gauche;
    T_Node _mollet_gauche;

    T_Node _cuisse_droit;
    T_Node _mollet_droit;

    std::shared_ptr<GLS::Mesh> _cubeMesh;

    T_Node _createPivotWithMesh(glm::vec3 pivot_position,
                            glm::vec3 mesh_position,
                            glm::vec3 mesh_scale) {
        T_Node pivot = newNode();
        pivot->transform().setPosition(pivot_position);
        T_Node mesh = newNode();
        mesh->transform().setPosition(mesh_position);
        mesh->transform().setScale(mesh_scale);
        mesh->addRenderable(_cubeMesh);
        pivot->addChildNode(mesh);
        return pivot;
    }

    public:

    Human() {

        using glm::vec3;
        _cubeMesh = GLS::Mesh::cube(1, 1, 1);

        _node = newNode();

        _torse = _createPivotWithMesh(
            vec3(0, 2, 0), vec3(0, 0.65, 0), vec3(1, 1.3, 0.6));
        _node->addChildNode(_torse);

        _tete = _createPivotWithMesh(
            vec3(0, 1.3, 0), vec3(0, 0.4, 0), vec3(0.5, 0.5, 0.5));
        _torse->addChildNode(_tete);
        
        _bras_gauche = _createPivotWithMesh(
            vec3(-0.5, 1, 0), vec3(-0.5, 0, 0), vec3(1, 0.4, 0.4));
        _torse->addChildNode(_bras_gauche);

        _avant_gauche = _createPivotWithMesh(
            vec3(-1, 0, 0), vec3(-0.4, 0, 0), vec3(0.8, 0.3, 0.3));
        _bras_gauche->addChildNode(_avant_gauche);

        _bras_droit = _createPivotWithMesh(
            vec3(0.5, 1, 0), vec3(0.5, 0, 0), vec3(1, 0.4, 0.4));
        _torse->addChildNode(_bras_droit);

        _avant_droit = _createPivotWithMesh(
            vec3(1, 0, 0), vec3(0.4, 0, 0), vec3(0.8, 0.3, 0.3));
        _bras_droit->addChildNode(_avant_droit);

        _cuisse_droit = _createPivotWithMesh(
            vec3(0.25, 0, 0), vec3(0, -0.5, 0), vec3(0.4, 1, 0.5));
        _torse->addChildNode(_cuisse_droit);

        _mollet_droit = _createPivotWithMesh(
            vec3(0, -1, 0), vec3(0, -0.5, 0), vec3(0.4, 1, 0.4));
        _cuisse_droit->addChildNode(_mollet_droit);

        _cuisse_gauche = _createPivotWithMesh(
            vec3(-0.25, 0, 0), vec3(0, -0.5, 0), vec3(0.4, 1, 0.5));
        _torse->addChildNode(_cuisse_gauche);

        _mollet_gauche = _createPivotWithMesh(
            vec3(0, -1, 0), vec3(0, -0.5, 0), vec3(0.4, 1, 0.4));
        _cuisse_gauche->addChildNode(_mollet_gauche);

    }

    T_Node node() {
        return _node;
    }

    void applyFrame(HumanAnimationState frame) {
        _node->transform().setPosition(frame.position);
        _torse->transform().setEulerAngles(frame.torse);
        _tete->transform().setEulerAngles(frame.tete);
        _bras_gauche->transform().setEulerAngles(frame.bras_gauche);
        _avant_gauche->transform().setEulerAngles(frame.avant_gauche);
        _bras_droit->transform().setEulerAngles(frame.bras_droit);
        _avant_droit->transform().setEulerAngles(frame.avant_droit);
        _cuisse_gauche->transform().setEulerAngles(frame.cuisse_gauche);
        _mollet_gauche->transform().setEulerAngles(frame.mollet_gauche);
        _cuisse_droit->transform().setEulerAngles(frame.cuisse_droit);
        _mollet_droit->transform().setEulerAngles(frame.mollet_droit);
    }

};

HumanSceneController::HumanSceneController(AppEnv *e) :
ISceneController(e) {
    hooman = nullptr;
    hoomanAnimator = nullptr;
}

HumanSceneController::~HumanSceneController() {
    
}

void HumanSceneController::makeScene() {
    GLS::Scene& scene(*env->scene);

    // camera and light
    T_Node cameraNode = newNode();
    {
        std::shared_ptr<GLS::Camera> camera = std::make_shared<GLS::Camera>();
        camera->aspect = (scene.getAspect());
        camera->farZ = (25.0);
        cameraNode->setCamera(camera);
    }
    cameraNode->transform().moveBy(0, 3, 5);
    scene.setCameraNode(cameraNode);
    scene.rootNode()->addChildNode(cameraNode);

    std::shared_ptr<GLS::Light> light = std::make_shared<GLS::Light>();
    T_Node lightNode = newNode();
    lightNode->transform().setPosition(glm::vec3(3, 5, 3));
    light->type = (GLS::light_point);
    lightNode->setLight(light);
    scene.rootNode()->addChildNode(lightNode);

    // just a nanosuit
    hooman = std::make_shared<Human>();
    T_Node humanNode = newNode();
    humanNode->addChildNode(hooman->node());
    scene.rootNode()->addChildNode(humanNode);

    std::shared_ptr<std::string> animationFilename = env->getArgument("-animation");
    if (animationFilename != nullptr) {
        std::ifstream animationFile(*animationFilename);
        hoomanAnimator = std::make_shared<GLS::Animator<HumanAnimationState> >(animationFile);
        std::cout << *hoomanAnimator << std::endl;
    }


    // create ground plane

    // create grass on it

    // create beautiful car

    // create some trees

    // create random shit mesh

}

void HumanSceneController::update() {

    if (hoomanAnimator != nullptr && hooman != nullptr) {
        float loopTime = fmod(env->currentTime, hoomanAnimator->time());
        hooman->applyFrame(hoomanAnimator->frameAt(loopTime));
    }
}
