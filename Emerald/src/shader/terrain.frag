#pragma once
#include "menu/submenu.h"

class ExampleMenu : public Submenu {
public:
	static ExampleMenu* GetInstance();
	void Init() override;
	void UpdateOnce() override;
	void Update() override;
	void FeatureUpdate() override;

	ExampleMenu() : Submenu() {}
	~ExampleMenu();
};

namespace ExampleMenuVars
{
	
}