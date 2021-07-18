
#include "AppEnv.hpp"

std::shared_ptr<GLS::Node> HumanSceneController::generateTentacle() const {
    // Create node hierarchy
    T_Node root = newNode("tentacle_root");
    T_Node t0 = newNode("tentacle_t0");
    root->addChildNode(t0);
    T_Node t1 = newNode("tentacle_t1");
    t0->addChildNode(t1);
    T_Node t2 = newNode("tentacle_t2");
    t1->addChildNode(t2);
    T_Node t3 = newNode("tentacle_t3");
    t2->addChildNode(t3);
    std::cout << "hierarchy ok !" << std::endl;

    // create node positions
    t1->transform().moveBy(0, 1, 0);
    t2->transform().moveBy(0, 1, 0);
    t3->transform().moveBy(0, 1, 0);
    std::cout << "transform ok !" << std::endl;

    // create skeleton
    std::shared_ptr<GLS::Skeleton> skeleton = std::make_shared<GLS::Skeleton>();
    root->addAnimatable(skeleton);
    skeleton->addBone(t0);
    skeleton->addBone(t1);
    skeleton->addBone(t2);
    skeleton->addBone(t3);
    std::cout << "bones ok !" << std::endl;

    // create animation
    GLS::SkeletonAnimation *animation = skeleton->createAnimation("waving", 2.0, true);
    int it0 = skeleton->indexOfBoneNamed("tentacle_t0");
    int it1 = skeleton->indexOfBoneNamed("tentacle_t1");
    int it2 = skeleton->indexOfBoneNamed("tentacle_t2");
    int it3 = skeleton->indexOfBoneNamed("tentacle_t3");
    std::cout << "animation init ok ! " << animation << '-' << it0 << ';' << it1 << ';' << it2 << ';' << it3 << std::endl;

    animation->interpolatorAt(it0).addScaleAt(0, glm::vec3(1, 1, 1));

    animation->interpolatorAt(it1).addRotationAt(0, glm::quat());
    animation->interpolatorAt(it1).addRotationAt(0.5, glm::quat(glm::vec3(0, 0, 0.6)));
    animation->interpolatorAt(it1).addRotationAt(1.5, glm::quat(glm::vec3(0, 0, -0.6)));
    animation->interpolatorAt(it1).addRotationAt(2.0, glm::quat());

    animation->interpolatorAt(it2).addRotationAt(0, glm::quat());
    animation->interpolatorAt(it2).addRotationAt(0.5, glm::quat(glm::vec3(0, 0, 0.9)));
    animation->interpolatorAt(it2).addRotationAt(1.5, glm::quat(glm::vec3(0, 0, -0.9)));
    animation->interpolatorAt(it2).addRotationAt(2.0, glm::quat());

    animation->interpolatorAt(it3).addRotationAt(0, glm::quat());
    animation->interpolatorAt(it3).addRotationAt(0.5, glm::quat(glm::vec3(0, 0, 1.5)));
    animation->interpolatorAt(it3).addRotationAt(1.5, glm::quat(glm::vec3(0, 0, -1.5)));
    animation->interpolatorAt(it3).addRotationAt(2.0, glm::quat());

    std::cout << "interpolators ok !" << std::endl;
    skeleton->initAnimationNamed("waving");
    std::cout << "init animation with name ok !" << std::endl;

    return root;
}
