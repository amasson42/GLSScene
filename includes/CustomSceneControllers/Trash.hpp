
#pragma once

#include "AppEnv.hpp"

// namespace GLS {

//     class IAnimatable {

//         public:

//         virtual void initAnimation() = 0;
//         virtual void animate(float deltaTime) = 0;

//     };

// }

class ExplodingMesh: public GLS::IAnimatable {
	private:
	GLS::timefloat _lifeTime;
	std::shared_ptr<GLS::Node> _node; // contains multiples children node containing meshes

	public:

	ExplodingMesh(const std::shared_ptr<GLS::Mesh>& mesh);
	virtual ~ExplodingMesh();

	std::shared_ptr<GLS::Node> node() const;

	virtual void initAnimation();

	virtual void animate(GLS::timefloat deltaTime);

	virtual bool alive() const;

};

class TrashSceneController: public ISceneController {
    public:
    TrashSceneController(std::shared_ptr<GLSWindow> window);
    virtual ~TrashSceneController();

    virtual void makeScene();
    virtual void update();
};
