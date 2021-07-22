
#include "Human.hpp"

void HumanSceneController::_generateUI() {

    // Tools window
    _toolsWindow = new nanogui::Window(nanoguiScreen(), "Tools");
    _toolsWindow->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Fill, 0, 0));
    _toolsWindow->setPosition(nanogui::Vector2i(10, 40));

    nanogui::VScrollPanel* scrollPanel = new nanogui::VScrollPanel(_toolsWindow);
    scrollPanel->setFixedSize(nanogui::Vector2i(200, 300));

    nanogui::Widget* scrollWidget = new nanogui::Widget(scrollPanel);
    scrollWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Minimum, 10, 0));


    // Speed Widget

    nanogui::Widget* speedWidget = new nanogui::Widget(scrollWidget);
    speedWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Minimum, 10, 0));

    nanogui::Label* speedLabel = new nanogui::Label(speedWidget, "speed");
    (void)speedLabel;
    nanogui::FloatBox<float>* speedBox = new nanogui::FloatBox<float>(speedWidget, playSpeed);
    speedBox->setFixedWidth(80);

    speedBox->setCallback([this](float x) {
        playSpeed = x;
        std::cout << "set speed to " << x << std::endl;
    });
    speedBox->setEditable(true);


    // Size Widget

    if (_mainNode != nullptr) {
        nanogui::Widget* sizeWidget = new nanogui::Widget(scrollWidget);
        sizeWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Minimum, 10, 0));

        nanogui::Label* sizeLabel = new nanogui::Label(sizeWidget, "size");
        (void)sizeLabel;
        nanogui::FloatBox<float>* sizeBox = new nanogui::FloatBox<float>(sizeWidget, _mainNode->transform().scale().x);
        sizeBox->setFixedWidth(80);

        sizeBox->setCallback([this](float x) {
            _mainNode->transform().setScale(glm::vec3(x, x, x));
        });
        sizeBox->setEditable(true);
    }


    // Animations Widget

    std::shared_ptr<GLS::Skeleton> skeleton = nullptr;
    if (_mainNode != nullptr
        && _mainNode->childNodes().empty() == false
        && (skeleton = _mainNode->childNodeAt(0)->getAnimatable<GLS::Skeleton>()) != nullptr) {
        nanogui::Widget* animationsWidget = new nanogui::Widget(scrollWidget);
        animationsWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Minimum, 10, 0));
        
        new nanogui::Label(animationsWidget, "Animations");

        // TODO: Put things in vector and use it to change their values
        std::vector<nanogui::Slider*> sliders;
        std::vector<nanogui::Button*> buttons;
        std::vector<std::string> names = skeleton->animationNames();
        for (size_t i = 0; i < names.size(); i++) {
            nanogui::Widget* animationWidget = new nanogui::Widget(animationsWidget);
            animationWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Minimum, 10, 5));

            nanogui::Button* animationButton = new nanogui::Button(animationWidget, names[i]);
            buttons.push_back(animationButton);

            nanogui::Slider* animationBlender = new nanogui::Slider(animationWidget);
            sliders.push_back(animationBlender);

        }

        for (size_t i = 0; i < names.size(); i++) {
            std::string name(names[i]);
            buttons[i]->setCallback([skeleton, name, sliders, i]() {
                skeleton->stopAnimations();
                skeleton->setAnimationBlend(name, 0, 1);
                for (size_t j = 0; j < sliders.size(); j++) {
                    sliders[j]->setValue(i == j ? 1.0 : 0.0);
                }
            });
            sliders[i]->setCallback([name, skeleton](float x) {
                skeleton->setAnimationBlend(name, skeleton->getAnimationTime(name), x);
            });
        }

    } else {
        std::cout << "No skeleton" << std::endl;
    }

    nanoguiScreen()->performLayout();
}
