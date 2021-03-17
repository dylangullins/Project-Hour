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
	std::vector<int> activebullets;

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

	std::string enemy1 = "enemy1.png";//cowboy
	std::string enemy2 = "enemy2.png";//knight
	std::string enemy3 = "enemy3.png";//male
	std::string enemy4 = "enemy4.png";//female
};
