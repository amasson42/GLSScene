
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
    nanogui::FloatBox<float>* speedBox = new nanogui::FloatBox<float>(speedWidget, 1.0f);

    speedBox->setCallback([this](float x) {
        playSpeed = x;
        std::cout << "set speed to " << x << std::endl;
    });
    speedBox->setEditable(true);


    // Size Widget

    nanogui::Widget* sizeWidget = new nanogui::Widget(scrollWidget);
    sizeWidget->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Minimum, 10, 0));

    nanogui::Label* sizeLabel = new nanogui::Label(sizeWidget, "size");
    (void)sizeLabel;
    nanogui::FloatBox<float>* sizeBox = new nanogui::FloatBox<float>(sizeWidget, 1.0f);

    sizeBox->setCallback([this](float x) {
        if (_mainNode != nullptr) {
            _mainNode->transform().setScale(glm::vec3(x, x, x));
        }
        std::cout << "set size to " << x << std::endl;
    });
    sizeBox->setEditable(true);

    nanoguiScreen()->performLayout();
}
