#pragma once

#include "Scene.h"
#include "PhysicsPlaygroundListener.h"
#include <string>

class PhysicsPlayground : public Scene
{
public:
	PhysicsPlayground(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;

	void Update() override;

	void MakePlatform(std::string fileName, float32 x, float32 y, int fx, int fy, float rotation);

	//Input overrides
	void KeyboardHold() override;
	void KeyboardDown() override;
	void KeyboardUp() override;
	void Track();

	std::vector<int> bulletStorage;
	std::vector<int> activeEnemies;

protected:
	bool m_firstWindow = false;
	bool m_secondWindow = false;

	std::string m_fileInput;

	PhysicsPlaygroundListener listener;

	int puzzleWall1;
	int puzzleWall2;
	int ammoEntity = -1;
	int bulletEntity = -1;
	int weapon = 1;
	int dashDirection;

	std::string loadout1 = "Front.png";
	std::string loadout2 = "Front.png";
	std::string loadout3 = "Front.png";
	std::string loadout4 = "Front.png";
};
