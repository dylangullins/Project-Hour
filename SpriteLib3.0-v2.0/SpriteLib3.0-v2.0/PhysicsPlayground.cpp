#include "PhysicsPlayground.h"
#include "Utilities.h"

#include <random>
#include "Ammo.h"
#include "BulletCollide.h"
#include "HP.h"

PhysicsPlayground::PhysicsPlayground(std::string name)
	: Scene(name)
{
	m_gravity = b2Vec2(0.f, 0.f);
	m_physicsWorld->SetGravity(m_gravity);

	m_physicsWorld->SetContactListener(&listener);
}

void PhysicsPlayground::InitScene(float windowWidth, float windowHeight)
{
	Scene::SetClearColor(vec4(0.f, 0.f, 0.f, 1.f));
	m_sceneReg = new entt::registry;
	ECS::AttachRegister(m_sceneReg);
	float aspectRatio = windowWidth / windowHeight;

	//Setup MainCamera Entity
	{

		//Creates Camera entity
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainCamera(entity, true);

		//Creates new orthographic camera
		ECS::AttachComponent<Camera>(entity);
		ECS::AttachComponent<HorizontalScroll>(entity);
		ECS::AttachComponent<VerticalScroll>(entity);

		vec4 temp = vec4(-75.f, 75.f, -75.f, 75.f);
		ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

		//Attaches the camera to vert and horiz scrolls
		ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
	}

	//Player entity
	{
		auto entity = ECS::CreateEntity();

		ECS::SetIsMainPlayer(entity, true);

		//Add components
		ECS::AttachComponent<Player>(entity);
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Ammo>(entity);
		ECS::AttachComponent<HP>(entity);

		//set components
		std::string fileName = "Front.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 20, 20);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 30.f, 30.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 35.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(10.f), float32(10.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), vec2(0.f, 0.f), false,
			PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER | GROUND | ENVIRONMENT, 0.35f, 1.2f); //circle body

		tempBody->SetFixedRotation(true);
		tempPhsBody.SetRotationAngleDeg(0.f);
		tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
	}

	{//Create Weapon
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainUI(entity, true);

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//set components
		std::string fileName = "Gat.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 10, 10);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(100.f, 100.f, 1.f));
		ECS::GetComponent<Transform>(entity).SetRotationAngleZ(0);
	}

	{//Create background
		auto entity = ECS::CreateEntity();
		ECS::SetIsBackgroundUI(entity, true);

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//set components
		std::string fileName = "Labwall.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 90, 30);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1000.f, 1000.f, 1.f));
	}

	{//Create healthbar
		auto entity = ECS::CreateEntity();
		ECS::SetIsHealthUI(entity, true);

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//set components
		std::string fileName = "hexagon.gif";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 90, 30);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(100.f, 100.f, 1.f));
	}

	/*{//Create Ammo bar
		auto entity = ECS::CreateEntity();
		ECS::SetIsHealthUI(entity, true);

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//set components
		std::string fileName = "hexagon.gif";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 90, 30);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(100.f, 100.f, 1.f));
	}*/

	{//Create tutorial
		auto entity = ECS::CreateEntity();
		ECS::SetIsTutorial(entity, true);

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//set components
		std::string fileName = "LikeButton.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 25, 25);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(20.f, 20.f, 1.f));
	}

	/*
	The Lab
	*/
	std::string fileName = "LabWall.png";
	MakePlatform(fileName, 0.f, 0.f, 100, 10, 0.f);
	MakePlatform(fileName, -50.f, 50, 10, 100, 0.f);
	MakePlatform(fileName, 50.f, 50.f, 10, 100, 0.f);
	MakePlatform(fileName, -30.f, 100.f, 35, 10, 0.f);
	MakePlatform(fileName, 30.f, 100.f, 35, 10, 0.f);
	MakePlatform(fileName, -50.f, 140.f, 10, 85, 0.f);
	MakePlatform(fileName, 95.f, 180.f, 300, 10, 0.f);
	MakePlatform(fileName, 75.f, 230.f, 10, 95, 0.f);
	MakePlatform(fileName, 270.f, 275.f, 400, 10, 0.f);
	MakePlatform(fileName, 305.f, 190.f, 10, 25, 0.f);
	MakePlatform(fileName, 390.f, 205, 175, 10, 0.f);
	MakePlatform(fileName, 650.f, 275.f, 375, 10, 0.f);
	MakePlatform(fileName, 670.f, -150.f, 900, 10, 70.f);
	MakePlatform(fileName, 270.f, -570.f, 500, 10, 0.f);
	MakePlatform(fileName, 50.f, -270.f, 600, 10, 270.f);
	MakePlatform(fileName, 150.f, -160.f, 200, 10, 0.f);
	MakePlatform(fileName, 250.f, -220.f, 10, 130, 0.f);
	MakePlatform(fileName, 200.f, -280.f, 100, 10, 0.f);
	MakePlatform(fileName, 180.f, -340.f, 80, 10, 0.f);
	MakePlatform(fileName, 170.f, -370.f, 60, 10, 90.f);
	MakePlatform(fileName, 80.f, -460.f, 60, 10, 0.f);
	MakePlatform(fileName, 210.f, -400.f, 90, 10, 0.f);
	MakePlatform(fileName, 250.f, -370.f, 10, 70, 0.f);
	MakePlatform(fileName, 250.f, -390.f, 10, 60, 0.f);
	MakePlatform(fileName, 250.f, -520.f, 10, 90, 0.f);
	MakePlatform(fileName, 450.f, -440.f, 225, 10, 0.f);
	MakePlatform(fileName, 360.f, -60.f, 10, 400, 0.f);
	MakePlatform(fileName, 405.f, 140.f, 100, 10, 0.f);
	MakePlatform(fileName, 455.f, 100.f, 10, 90, 0.f);
	MakePlatform(fileName, 505.f, 100.f, 100, 10, 0.f);
	MakePlatform(fileName, 605.f, 140.f, 100, 10, 0.f);
	MakePlatform(fileName, 555.f, 100.f, 10, 90, 0.f);
	MakePlatform(fileName, 655.f, 75.f, 10, 140, 0.f);
	MakePlatform(fileName, 576.2f, -80.f, 250, 10, 50.f);
	MakePlatform(fileName, 497.f, -218.f, 10, 90, 0.f);
	MakePlatform(fileName, 430.f, -40.f, 150, 10, 0.f);
	MakePlatform(fileName, 700.f, 180.f, 185, 10, 0.f);
	MakePlatform(fileName, 594.f, 201.f, 60, 10, 130.f);

	{// enemy spawner
		std::string enemy1 = "hexagon.gif";//cowboy
		std::string enemy2 = "enemy2.png";//knight
		std::string enemy3 = "ScientistEnemy.gif";//male
		std::string enemy4 = "ScientistEnemy2.gif";//female
		activeEnemies.push_back(Scene::CreateEnemy(enemy3, 20.f, 20.f, 50.f, 50.f, 0.f));
		activeEnemies.push_back(Scene::CreateEnemy(enemy4, 20.f, 20.f, 60.f, 60.f, 0.f));
		activeEnemies.push_back(Scene::CreateEnemy(enemy3, 20.f, 20.f, 100.f, 100.f, 0.f));
		activeEnemies.push_back(Scene::CreateEnemy(enemy4, 20.f, 20.f, 120.f, 120.f, 0.f));
		activeEnemies.push_back(Scene::CreateEnemy(enemy3, 20.f, 20.f, 500.f, 500.f, 0.f));
		activeEnemies.push_back(Scene::CreateEnemy(enemy4, 20.f, 20.f, 200.f, -300.f, 0.f));

	}

	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
}

void PhysicsPlayground::Update()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	Scene::AdjustScrollOffset();


	std::cout << player.GetPosition().x << ", " << player.GetPosition().y << "\n ";

	player.GetBody()->SetLinearVelocity(b2Vec2(player.GetBody()->GetLinearVelocity().x * 0.888f, player.GetBody()->GetLinearVelocity().y * 0.888f));

	for (int x = 0; x < activeEnemies.size(); x++)
	{
		if (ECS::GetComponent<HP>(activeEnemies[x]).hp <= 0)
		{
			PhysicsBody::m_bodiesToDelete.push_back(activeEnemies[x]);
			activeEnemies.erase(activeEnemies.begin() + x);
		}
	}

	for (int i = 0; i < bulletStorage.size(); i++)
	{
		if (ECS::GetComponent<BulletCollide>(bulletStorage[i]).collided == true)
		{
			PhysicsBody::m_bodiesToDelete.push_back(bulletStorage[i]);
			bulletStorage.erase(bulletStorage.begin() + i);
		}
	}

	
	{
		//auto& Gun = ECS::GetComponent<PhysicsBody>(MainEntities::MainUI());
		std::string Revolver = "Revolver.png";//cowboy
		std::string Gat = "Gat.png";//knight
		std::string LikeButton = "LikeButton.png";//knight
		std::string BeachBall = "BeachBall.png";//knight

		ECS::GetComponent<Transform>(MainEntities::MainUI()).SetPosition(vec3(player.GetPosition().x + 10, player.GetPosition().y + 2, 2.f));

		if (weapon == 1)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainUI()).LoadSprite(Gat, 20, 20, false);

			//Gun.SetRotationAngleDeg(player.GetRotationAngleDeg());
		}

		if (weapon == 2)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainUI()).LoadSprite(Revolver, 20, 20, false);
		}

		if (weapon == 3)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainUI()).LoadSprite(LikeButton, 20, 20, false);
		}

		if (weapon == 4)
		{
			ECS::GetComponent<Sprite>(MainEntities::MainUI()).LoadSprite(BeachBall, 20, 20, false);
		}

	}

	{//background UI change

		std::string background1 = "Labwall.png";//cowboy
		std::string background2 = "Gat.png";//knight
		std::string background3 = "LikeButton.png";//knight
		std::string background4 = "BeachBall.png";//knight

		ECS::GetComponent<Transform>(MainEntities::BackgroundUI()).SetPosition(vec3(player.GetPosition().x, player.GetPosition().y, 0.f));

		if (player.GetPosition().x <= 500 && player.GetPosition().x >= 0)
		{
			ECS::GetComponent<Sprite>(MainEntities::BackgroundUI()).LoadSprite(background1, 1000, 1000, false);
		}

		if (player.GetPosition().x <= 1000 && player.GetPosition().x >= 500)
		{
			ECS::GetComponent<Sprite>(MainEntities::BackgroundUI()).LoadSprite(background2, 1000, 1000, false);
		}

		if (player.GetPosition().x <= 1500 && player.GetPosition().x >= 1000)
		{
			ECS::GetComponent<Sprite>(MainEntities::BackgroundUI()).LoadSprite(background3, 1000, 1000, false);
		}

		if (player.GetPosition().x <= 2000 && player.GetPosition().x >= 1500)
		{
			ECS::GetComponent<Sprite>(MainEntities::BackgroundUI()).LoadSprite(background4, 1000, 1000, false);
		}

	}

	{//Health bar UI 

		std::string Hp5 = "LikeButton.png";//Max hp
		std::string Hp4 = "Gat.png";//Near Max hp
		std::string Hp3 = "LikeButton.png";//Half hp
		std::string Hp2 = "BeachBall.png";//near low hp
		std::string Hp1 = "ScientistEnemy.gif";//Low hp
		std::string Hp0 = "hexagon.gif";//Dead

		ECS::GetComponent<Transform>(MainEntities::HealthUI()).SetPosition(vec3(player.GetPosition().x - 80.f, player.GetPosition().y + 75.f, 50.f));

		if (ECS::GetComponent<HP>(MainEntities::MainPlayer()).hp = 50)
		{
			ECS::GetComponent<Sprite>(MainEntities::HealthUI()).LoadSprite(Hp5, 90, 30, false);
		}

		if (ECS::GetComponent<HP>(MainEntities::MainPlayer()).hp < 50 && ECS::GetComponent<HP>(MainEntities::MainPlayer()).hp >= 40)
		{
			ECS::GetComponent<Sprite>(MainEntities::HealthUI()).LoadSprite(Hp4, 90, 30, false);
		}

		if (ECS::GetComponent<HP>(MainEntities::MainPlayer()).hp < 40 && ECS::GetComponent<HP>(MainEntities::MainPlayer()).hp >= 40)
		{
			ECS::GetComponent<Sprite>(MainEntities::HealthUI()).LoadSprite(Hp3, 90, 30, false);
		}

		if (ECS::GetComponent<HP>(MainEntities::MainPlayer()).hp < 30 && ECS::GetComponent<HP>(MainEntities::MainPlayer()).hp >= 20)
		{
			ECS::GetComponent<Sprite>(MainEntities::HealthUI()).LoadSprite(Hp2, 90, 30, false);
		}

		if (ECS::GetComponent<HP>(MainEntities::MainPlayer()).hp < 20 && ECS::GetComponent<HP>(MainEntities::MainPlayer()).hp >= 10)
		{
			ECS::GetComponent<Sprite>(MainEntities::HealthUI()).LoadSprite(Hp1, 90, 30, false);
		}

		if (ECS::GetComponent<HP>(MainEntities::MainPlayer()).hp < 10 && ECS::GetComponent<HP>(MainEntities::MainPlayer()).hp >= 0)
		{
			ECS::GetComponent<Sprite>(MainEntities::HealthUI()).LoadSprite(Hp0, 90, 30, false);
		}
	}

	{//weapon UI

		std::string Ammo0 = "LikeButton.png";//Empty
		std::string Ammo1 = "hexagon.gif";//Near empty
		std::string Ammo2 = "Gat.png";//Half ammo
		std::string Ammo3 = "LikeButton.png";//Full ammo

		//ECS::GetComponent<Transform>(MainEntities::AmmoUI()).SetPosition(vec3(player.GetPosition().x + 50, player.GetPosition().y - 50, 50.f));

		/*if (weapon == 1)
		{
			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo1 = 30)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo3, 90, 30, false);
			}

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo1 < 30 && ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo1 >= 15)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo2, 90, 30, false);
			}

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo1 < 15 && ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo1 >= 0)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo1, 90, 30, false);
			}

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo1 = 0)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo0, 90, 30, false);
			}
		}

		if (weapon == 2)
		{
			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo2 = 10)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo3, 90, 30, false);
			}

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo2 < 10 && ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo2 >= 5)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo2, 90, 30, false);
			}

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo2 < 5 && ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo2 >= 0)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo1, 90, 30, false);
			}

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo2 = 0)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo0, 90, 30, false);
			}
		}

		if (weapon == 3)
		{
			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo3 = 20)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo3, 90, 30, false);
			}

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo3 < 20 && ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo3 >= 10)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo2, 90, 30, false);
			}

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo3 < 10 && ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo3 >= 0)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo1, 90, 30, false);
			}

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo3 < 0)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo0, 90, 30, false);
			}
		}

		if (weapon == 4)
		{
			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo4 = 5)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo3, 90, 30, false);
			}

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo4 < 5 && ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo4 >= 2)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo2, 90, 30, false);
			}

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo4 < 2 && ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo4 >= 0)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo1, 90, 30, false);
			}

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo4 = 0)
			{
				ECS::GetComponent<Sprite>(MainEntities::AmmoUI()).LoadSprite(Ammo0, 90, 30, false);
			}
		}*/
	}

		{//tutorial change 

		std::string tut1 = "LikeButton.png";//weapon 1 tutorial
		std::string tut2 = "Gat.png";//weapon 2 tutorial
		std::string tut3 = "LikeButton.png";//weapon 3 tutorial
		std::string tut4 = "BeachBall.png";//weapon 4 tutorial
		std::string tut5 = "LikeButton.png";//weapon 4 tutorial
		std::string tut6 = "Gat.png";//weapon 4 tutorial


		ECS::GetComponent<Transform>(MainEntities::Tutorial()).SetPosition(vec3(20.f, 20.f, 1.f));

		if (Input::GetKey(Key::U))
		{
			ECS::GetComponent<Sprite>(MainEntities::Tutorial()).LoadSprite(tut1, 25, 25, false);
		
		}

		if (Input::GetKey(Key::I))
		{
			ECS::GetComponent<Sprite>(MainEntities::Tutorial()).LoadSprite(tut2, 25, 25, false);
		
		}

		if (Input::GetKey(Key::O))
		{
			ECS::GetComponent<Sprite>(MainEntities::Tutorial()).LoadSprite(tut3, 25, 25, false);
			
		}

		if (Input::GetKey(Key::J))
		{
			ECS::GetComponent<Sprite>(MainEntities::Tutorial()).LoadSprite(tut4, 25, 25, false);
			
		}
	}

	Track();
}


void PhysicsPlayground::KeyboardDown()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());

	if (Input::GetKeyDown(Key::F))
	{
		switch (weapon)
		{
		case 1:

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo1 > 0)
			{
				bulletEntity = Scene::CreateBullet(player.GetBody()->GetPosition().x, player.GetBody()->GetPosition().y, weapon, rotation);
				bulletStorage.push_back(bulletEntity);
				ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo1 -= 1;
			}

			break;

		case 2:

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo2 > 0)
			{
				bulletEntity = Scene::CreateBullet(player.GetBody()->GetPosition().x, player.GetBody()->GetPosition().y, weapon, rotation);
				bulletStorage.push_back(bulletEntity);
				ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo2 -= 1;
			}

			break;

		case 3:

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo3 > 0)
			{
				bulletEntity = Scene::CreateBullet(player.GetBody()->GetPosition().x, player.GetBody()->GetPosition().y, weapon, rotation);
				bulletStorage.push_back(bulletEntity);
				ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo3 -= 1;
			}

			break;

		case 4:

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo4 > 0)
			{
				bulletEntity = Scene::CreateBullet(player.GetBody()->GetPosition().x, player.GetBody()->GetPosition().y, weapon, rotation);
				bulletStorage.push_back(bulletEntity);
				ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo4 -= 1;
			}

			break;
		}
	}

	if (Input::GetKeyDown(Key::R))
	{
		switch (weapon)
		{
		case 1:

			ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo1 = 30;

			break;

		case 2:

			ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo2 = 10;

			break;

		case 3:

			ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo3 = 20;

			break;

		case 4:

			ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo4 = 5;

			break;
		}
	}

	if (Input::GetKeyDown(Key::One))
	{
		weapon = 1;
	}

	if (Input::GetKeyDown(Key::Two))
	{
		weapon = 2;
	}

	if (Input::GetKeyDown(Key::Three))
	{
		weapon = 3;
	}

	if (Input::GetKeyDown(Key::Four))
	{
		weapon = 4;
	}
}

void PhysicsPlayground::KeyboardHold()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());


	if (Input::GetKey(Key::W))
	{
		std::string enemy1 = "MichaelWalkUp.gif";//cowboy
		ECS::GetComponent<Sprite>(MainEntities::MainPlayer()).LoadSprite(enemy1, 20, 20, false);

		player.GetBody()->ApplyForceToCenter(b2Vec2(0.f, 300000.f), true);
		//player.SetRotationAngleDeg(90.f);
		rotation = 90;
	}

	if (Input::GetKey(Key::A))
	{
		std::string enemy1 = "MichaelWalkLeft.gif";//cowboy
		ECS::GetComponent<Sprite>(MainEntities::MainPlayer()).LoadSprite(enemy1, 20, 20, false);
		player.GetBody()->ApplyForceToCenter(b2Vec2(-300000.f, 0.f), true);
		//player.SetRotationAngleDeg(180.f);
		rotation = 180;
	}

	if (Input::GetKey(Key::S))
	{
		std::string enemy1 = "MichaelWalkDown.gif";//cowboy
		ECS::GetComponent<Sprite>(MainEntities::MainPlayer()).LoadSprite(enemy1, 20, 20, false);
		player.GetBody()->ApplyForceToCenter(b2Vec2(0.f, -300000.f), true);
		//player.SetRotationAngleDeg(270.f);
		rotation = 270;
	}

	if (Input::GetKey(Key::D))
	{
		std::string enemy1 = "MichaelWalkRight.gif";//cowboy
		ECS::GetComponent<Sprite>(MainEntities::MainPlayer()).LoadSprite(enemy1, 20, 20, false);
		player.GetBody()->ApplyForceToCenter(b2Vec2(300000.f, 0.f), true);
		//player.SetRotationAngleDeg(0.f);
		rotation = 0;
	}
}



void PhysicsPlayground::KeyboardUp()
{


}

void PhysicsPlayground::Track()
{
	for (int i = 0; i < activeEnemies.size(); i++)
	{
		auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
		auto& enemy = ECS::GetComponent<PhysicsBody>(activeEnemies[i]);

		vec2 total = vec2(player.GetPosition().x - enemy.GetPosition().x, player.GetPosition().y - enemy.GetPosition().y);
		float length = (sqrt((total.x * total.x) + (total.y * total.y)));
		vec2 normal = vec2(total.x / length, total.y / length);

		for (int x = 0; x < 50; x++)
		{
			if (player.GetPosition().x + x == enemy.GetPosition().x || player.GetPosition().y + x == enemy.GetPosition().y)
			{
				enemy.GetBody()->SetLinearVelocity(b2Vec2(normal.x * 100.f, normal.y * 100.f));
			}

			if (player.GetPosition().x - x == enemy.GetPosition().x || player.GetPosition().y - x == enemy.GetPosition().y)
			{
				enemy.GetBody()->SetLinearVelocity(b2Vec2(normal.x * 100.f, normal.y * 100.f));
			}
		}

		b2Vec2 defaultVector = b2Vec2(1.f, 0.f);//0 degrees

		b2Vec2 playerPos = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer()).GetPosition();
		b2Vec2 enemyPos = ECS::GetComponent<PhysicsBody>(activeEnemies[i]).GetPosition();
		b2Vec2 vectorToPlayer = playerPos - enemyPos;

		float dot = (defaultVector.x * vectorToPlayer.x + defaultVector.y * vectorToPlayer.y);
		float angle = acos(dot / (vectorToPlayer.Length() * defaultVector.Length()));

		if (vectorToPlayer.y >= 0)
		{
			ECS::GetComponent<PhysicsBody>(activeEnemies[i]).SetRotationAngleDeg(angle * (180 / PI));
		}

		else if (vectorToPlayer.y < 0)
		{
			ECS::GetComponent<PhysicsBody>(activeEnemies[i]).SetRotationAngleDeg(-angle * (180 / PI));
		}
	}
}

void PhysicsPlayground::MakePlatform(std::string fileName, float32 x, float32 y, int fx, int fy, float rotation)
{
	auto entity = ECS::CreateEntity();

	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, fx, fy);
	ECS::GetComponent<Transform>(entity).SetPosition(vec3(30.f, -20.f, 2.f));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	float shrinkX = 0.f;
	float shrinkY = 0.f;
	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(x), float32(y));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
		float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY | OBJECTS);
	tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
	tempPhsBody.SetRotationAngleDeg(rotation);
}



