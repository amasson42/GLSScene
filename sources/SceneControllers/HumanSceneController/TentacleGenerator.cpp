
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

    // create node positions
    t1->transform().moveBy(0, 1, 0);
    t2->transform().moveBy(0, 1, 0);
    t3->transform().moveBy(0, 1, 0);

    // create skeleton
    std::shared_ptr<GLS::Skeleton> skeleton = std::make_shared<GLS::Skeleton>();
    root->addAnimatable(skeleton);
    skeleton->addBone(t0);
    skeleton->addBone(t1);
    skeleton->addBone(t2);
    skeleton->addBone(t3);

    // create animation
    GLS::SkeletonAnimation *animation = skeleton->createAnimation("waving", 2.0, true);
    int it0 = skeleton->indexOfBoneNamed("tentacle_t0");
    int it1 = skeleton->indexOfBoneNamed("tentacle_t1");
    int it2 = skeleton->indexOfBoneNamed("tentacle_t2");
    int it3 = skeleton->indexOfBoneNamed("tentacle_t3");

    animation->interpolatorAt(it0).addScaleAt(0, glm::vec3(1, 1, 1));
    animation->interpolatorAt(it0).addPositionAt(0, glm::vec3(0, 0, 0));
    animation->interpolatorAt(it0).addPositionAt(1, glm::vec3(2, 0, 0), GLS::CurveFunction<GLS::timefloat>::easeOut());
    animation->interpolatorAt(it0).addPositionAt(2, glm::vec3(0, 0, 0), GLS::CurveFunction<GLS::timefloat>::easeOut());

    animation->interpolatorAt(it1).addPositionAt(0, glm::vec3(0, 1, 0));
    animation->interpolatorAt(it1).addRotationAt(0, glm::quat());
    animation->interpolatorAt(it1).addRotationAt(0.5, glm::quat(glm::vec3(0, 0, 0.6)), GLS::CurveFunction<GLS::timefloat>::bounce());
    animation->interpolatorAt(it1).addRotationAt(1.5, glm::quat(glm::vec3(0, 0, -0.6)), GLS::CurveFunction<GLS::timefloat>::bounce());
    animation->interpolatorAt(it1).addRotationAt(2.0, glm::quat());

    animation->interpolatorAt(it2).addPositionAt(0, glm::vec3(0, 1, 0));
    animation->interpolatorAt(it2).addRotationAt(0, glm::quat());
    animation->interpolatorAt(it2).addRotationAt(0.5, glm::quat(glm::vec3(0, 0, 0.9)));
    animation->interpolatorAt(it2).addRotationAt(1.5, glm::quat(glm::vec3(0, 0, -0.9)));
    animation->interpolatorAt(it2).addRotationAt(2.0, glm::quat());

    animation->interpolatorAt(it3).addPositionAt(0, glm::vec3(0, 1, 0));
    animation->interpolatorAt(it3).addRotationAt(0, glm::quat());
    animation->interpolatorAt(it3).addRotationAt(0.5, glm::quat(glm::vec3(0, 0, 1.5)));
    animation->interpolatorAt(it3).addRotationAt(1.5, glm::quat(glm::vec3(0, 0, -1.5)));
    animation->interpolatorAt(it3).addRotationAt(2.0, glm::quat());

    skeleton->initAnimationNamed("waving");

    std::shared_ptr<GLS::SkinnedMesh> mesh = std::make_shared<GLS::SkinnedMesh>();
    mesh->setSkeleton(skeleton, root);
    root->addRenderable(mesh);
    std::vector<GLS::SkinnedVertex>& vertices(mesh->verticesRef());
    std::vector<GLuint>& indices(mesh->indicesRef());

    const float r33 = sqrt(3.0f) / 3.0f;
    const float o33 = 1.0f - r33;
    const float r22 = sqrt(2.0f) / 2.0f;

    const float height = 4.0f;
    const int heightsCount = 4 * 2; // number of lines of vertices
    const int columnsCount = 4; // number of columns of vertices
    const float wx = 1.0f / (columnsCount - 1);
    const float hy = height / heightsCount;
    const float uy = 1.0f / heightsCount;
    auto rf = [height](float x) { return 1 - powf(x / height, 0.33); };
    float r = rf(0);

    // T0 layer
    r = rf(0 * hy);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 0 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(0 * wx, 0 * uy)));
    vertices[0 * columnsCount + 0].addWeight(0, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.5, 0 * hy, r *  o33), glm::vec3( r22, 0.0, r22), glm::vec2(1 * wx, 0 * uy)));
    vertices[0 * columnsCount + 1].addWeight(0, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.0, 0 * hy, r * -r33), glm::vec3( 0.0, 0.0, 1.0), glm::vec2(2 * wx, 0 * uy)));
    vertices[0 * columnsCount + 2].addWeight(0, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 0 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(3 * wx, 0 * uy)));
    vertices[0 * columnsCount + 3].addWeight(0, 1);

    // T0.5 layer
    r = rf(1 * hy);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 1 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(0 * wx, 1 * uy)));
    vertices[1 * columnsCount + 0].addWeight(0, 0.5);
    vertices[1 * columnsCount + 0].addWeight(1, 0.5);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.5, 1 * hy, r *  o33), glm::vec3( r22, 0.0, r22), glm::vec2(1 * wx, 1 * uy)));
    vertices[1 * columnsCount + 1].addWeight(0, 0.5);
    vertices[1 * columnsCount + 1].addWeight(1, 0.5);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.0, 1 * hy, r * -r33), glm::vec3( 0.0, 0.0, 1.0), glm::vec2(2 * wx, 1 * uy)));
    vertices[1 * columnsCount + 2].addWeight(0, 0.5);
    vertices[1 * columnsCount + 2].addWeight(1, 0.5);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 1 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(3 * wx, 1 * uy)));
    vertices[1 * columnsCount + 3].addWeight(0, 0.5);
    vertices[1 * columnsCount + 3].addWeight(1, 0.5);

    // T1 layer
    r = rf(2 * hy);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 2 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(0 * wx, 2 * uy)));
    vertices[2 * columnsCount + 0].addWeight(1, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.5, 2 * hy, r *  o33), glm::vec3( r22, 0.0, r22), glm::vec2(1 * wx, 2 * uy)));
    vertices[2 * columnsCount + 1].addWeight(1, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.0, 2 * hy, r * -r33), glm::vec3( 0.0, 0.0, 1.0), glm::vec2(2 * wx, 2 * uy)));
    vertices[2 * columnsCount + 2].addWeight(1, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 2 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(3 * wx, 2 * uy)));
    vertices[2 * columnsCount + 3].addWeight(1, 1);

    // T1.5 layer
    r = rf(3 * hy);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 3 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(0 * wx, 3 * uy)));
    vertices[3 * columnsCount + 0].addWeight(1, 0.5);
    vertices[3 * columnsCount + 0].addWeight(2, 0.5);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.5, 3 * hy, r *  o33), glm::vec3( r22, 0.0, r22), glm::vec2(1 * wx, 3 * uy)));
    vertices[3 * columnsCount + 1].addWeight(1, 0.5);
    vertices[3 * columnsCount + 1].addWeight(2, 0.5);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.0, 3 * hy, r * -r33), glm::vec3( 0.0, 0.0, 1.0), glm::vec2(2 * wx, 3 * uy)));
    vertices[3 * columnsCount + 2].addWeight(1, 0.5);
    vertices[3 * columnsCount + 2].addWeight(2, 0.5);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 3 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(3 * wx, 3 * uy)));
    vertices[3 * columnsCount + 3].addWeight(1, 0.5);
    vertices[3 * columnsCount + 3].addWeight(2, 0.5);

    // T2 layer
    r = rf(4 * hy);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 4 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(0 * wx, 4 * uy)));
    vertices[4 * columnsCount + 0].addWeight(2, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.5, 4 * hy, r *  o33), glm::vec3( r22, 0.0, r22), glm::vec2(1 * wx, 4 * uy)));
    vertices[4 * columnsCount + 1].addWeight(2, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.0, 4 * hy, r * -r33), glm::vec3( 0.0, 0.0, 1.0), glm::vec2(2 * wx, 4 * uy)));
    vertices[4 * columnsCount + 2].addWeight(2, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 4 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(3 * wx, 4 * uy)));
    vertices[4 * columnsCount + 3].addWeight(2, 1);

    // T2.5 layer
    r = rf(5 * hy);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 5 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(0 * wx, 5 * uy)));
    vertices[5 * columnsCount + 0].addWeight(2, 0.5);
    vertices[5 * columnsCount + 0].addWeight(3, 0.5);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.5, 5 * hy, r *  o33), glm::vec3( r22, 0.0, r22), glm::vec2(1 * wx, 5 * uy)));
    vertices[5 * columnsCount + 1].addWeight(2, 0.5);
    vertices[5 * columnsCount + 1].addWeight(3, 0.5);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.0, 5 * hy, r * -r33), glm::vec3( 0.0, 0.0, 1.0), glm::vec2(2 * wx, 5 * uy)));
    vertices[5 * columnsCount + 2].addWeight(2, 0.5);
    vertices[5 * columnsCount + 2].addWeight(3, 0.5);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 5 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(3 * wx, 5 * uy)));
    vertices[5 * columnsCount + 3].addWeight(2, 0.5);
    vertices[5 * columnsCount + 3].addWeight(3, 0.5);

    // T3 layer
    r = rf(6 * hy);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 6 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(0 * wx, 6 * uy)));
    vertices[6 * columnsCount + 0].addWeight(3, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.5, 6 * hy, r *  o33), glm::vec3( r22, 0.0, r22), glm::vec2(1 * wx, 6 * uy)));
    vertices[6 * columnsCount + 1].addWeight(3, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.0, 6 * hy, r * -r33), glm::vec3( 0.0, 0.0, 1.0), glm::vec2(2 * wx, 6 * uy)));
    vertices[6 * columnsCount + 2].addWeight(3, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 6 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(3 * wx, 6 * uy)));
    vertices[6 * columnsCount + 3].addWeight(3, 1);

    // T3.5 layer
    r = rf(7 * hy);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 7 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(0 * wx, 7 * uy)));
    vertices[7 * columnsCount + 0].addWeight(3, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.5, 7 * hy, r *  o33), glm::vec3( r22, 0.0, r22), glm::vec2(1 * wx, 7 * uy)));
    vertices[7 * columnsCount + 1].addWeight(3, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.0, 7 * hy, r * -r33), glm::vec3( 0.0, 0.0, 1.0), glm::vec2(2 * wx, 7 * uy)));
    vertices[7 * columnsCount + 2].addWeight(3, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 7 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(3 * wx, 7 * uy)));
    vertices[7 * columnsCount + 3].addWeight(3, 1);

    // T4 layer
    r = rf(8 * hy);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 8 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(0 * wx, 8 * uy)));
    vertices[8 * columnsCount + 0].addWeight(3, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.5, 8 * hy, r *  o33), glm::vec3( r22, 0.0, r22), glm::vec2(1 * wx, 8 * uy)));
    vertices[8 * columnsCount + 1].addWeight(3, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r *  0.0, 8 * hy, r * -r33), glm::vec3( 0.0, 0.0, 1.0), glm::vec2(2 * wx, 8 * uy)));
    vertices[8 * columnsCount + 2].addWeight(3, 1);
    vertices.push_back(GLS::SkinnedVertex(glm::vec3(r * -0.5, 8 * hy, r *  o33), glm::vec3(-r22, 0.0, r22), glm::vec2(3 * wx, 8 * uy)));
    vertices[8 * columnsCount + 3].addWeight(3, 1);


    // T0-T0.5 Faces
    for (int i = 0; i < heightsCount; i++) {
        for (int j = 0; j < columnsCount - 1; j++) {
            indices.push_back(i * columnsCount + j + 0);
            indices.push_back(i * columnsCount + j + 1);
            indices.push_back((i + 1) * columnsCount + j + 1);
            
            indices.push_back(i * columnsCount + j + 0);
            indices.push_back((i + 1) * columnsCount + j + 1);
            indices.push_back((i + 1) * columnsCount + j + 0);
        }
    }

    mesh->generateBuffers();
    return root;
}
