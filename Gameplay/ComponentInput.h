#pragma once
#include "Component.h"
#include "../Common/UserInterface.h"
#include <functional>
class ComponentInput: public Component {
public:
	ComponentInput() { type = ComponentType::Input; }
	void Init() override;
	void Update(float dt) override;
	UserInterface* userInterface;
	std::function<void()> Callback[max_input];
private:
};