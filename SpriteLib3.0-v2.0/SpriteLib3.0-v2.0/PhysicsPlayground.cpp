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
		tempDef.position.Set(float32(0.f), float32(0.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), vec2(0.f, 0.f), false,
			PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER | GROUND | ENVIRONMENT, 0.35f, 1.2f); //circle body

		tempBody->SetFixedRotation(true);
		tempPhsBody.SetRotationAngleDeg(0.f);
		tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));
	}

	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
}

void PhysicsPlayground::Update()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	Scene::AdjustScrollOffset();

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

	if (weapon == 1)
	{
		ECS::GetComponent<Sprite>(MainEntities::MainPlayer()).LoadSprite(loadout1, 20, 20, false);
	}
	
	if (weapon == 2)
	{
		ECS::GetComponent<Sprite>(MainEntities::MainPlayer()).LoadSprite(loadout2, 20, 20, false);
	}

	if (weapon == 3)
	{
		ECS::GetComponent<Sprite>(MainEntities::MainPlayer()).LoadSprite(loadout3, 20, 20, false);
	}

	if (weapon == 4)
	{
		ECS::GetComponent<Sprite>(MainEntities::MainPlayer()).LoadSprite(loadout4, 20, 20, false);
	}
}


void PhysicsPlayground::KeyboardHold()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());


	if (Input::GetKey(Key::W))
	{
		player.GetBody()->ApplyForceToCenter(b2Vec2(0.f, 300000.f), true);
		player.SetRotationAngleDeg(90.f);
		dashDirection = 1;
	}

	if (Input::GetKey(Key::A))
	{
		player.GetBody()->ApplyForceToCenter(b2Vec2(-300000.f, 0.f), true);
		player.SetRotationAngleDeg(180.f);
		dashDirection = 2;
	}

	if (Input::GetKey(Key::S))
	{
		player.GetBody()->ApplyForceToCenter(b2Vec2(0.f, -300000.f), true);
		player.SetRotationAngleDeg(270.f);
		dashDirection = 3;
	}

	if (Input::GetKey(Key::D))
	{
		player.GetBody()->ApplyForceToCenter(b2Vec2(300000.f, 0.f), true);
		player.SetRotationAngleDeg(0.f);
		dashDirection = 4;
	}
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
				bulletEntity = Scene::CreateBullet(player.GetBody()->GetPosition().x, player.GetBody()->GetPosition().y);
				bulletStorage.push_back(bulletEntity);
				ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo1 -= 1;
			}

			break;

		case 2:

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo2 > 0)
			{
				bulletEntity = Scene::CreateBullet(player.GetBody()->GetPosition().x, player.GetBody()->GetPosition().y);
				bulletStorage.push_back(bulletEntity);
				ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo2 -= 1;
			}

			break;

		case 3:

			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo3 > 0)
			{
				bulletEntity = Scene::CreateBullet(player.GetBody()->GetPosition().x, player.GetBody()->GetPosition().y);
				bulletStorage.push_back(bulletEntity);
				ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo3 -= 1;
			}

			break;

		case 4:
			if (ECS::GetComponent<Ammo>(MainEntities::MainPlayer()).ammo4 > 0)
			{
				bulletEntity = Scene::CreateBullet(player.GetBody()->GetPosition().x, player.GetBody()->GetPosition().y);
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

	if (Input::GetKeyDown(Key::U))
	{
		weapon = 1;
	}

	if (Input::GetKeyDown(Key::I))
	{
		weapon = 2;
	}

	if (Input::GetKeyDown(Key::O))
	{
		weapon = 3;
	}

	if (Input::GetKeyDown(Key::L))
	{
		weapon = 4;
	}

	if (Input::GetKeyDown(Key::Q)) 
	{
		switch (dashDirection)
		{
		case 1:
			player.GetBody()->ApplyLinearImpulseToCenter(b2Vec2(0.f, 300000.f), true);
			break;

		case 2:
			player.GetBody()->ApplyLinearImpulseToCenter(b2Vec2(-300000.f, 0.f), true);
			break;

		case 3:
			player.GetBody()->ApplyLinearImpulseToCenter(b2Vec2(0.f, -300000), true);
			break;

		case 4:
			player.GetBody()->ApplyLinearImpulseToCenter(b2Vec2(300000.f, 0.f), true);
			break;
		}
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

		for(int x = 0; x < 50; x++)
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

		b2Vec2 defaultVector = b2Vec2(1, 0);//0 degrees

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

void PhysicsPlayground::MakePlatform(std::string fileName, float32 x, float32 y, int fx, int fy) 
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
}
