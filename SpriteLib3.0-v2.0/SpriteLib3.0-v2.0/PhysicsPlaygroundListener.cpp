#include "PhysicsPlaygroundListener.h"

#include "ECS.h"
#include "HP.h"
#include "BulletCollide.h"

PhysicsPlaygroundListener::PhysicsPlaygroundListener()
	: b2ContactListener()
{

}

void PhysicsPlaygroundListener::BeginContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	bool sensorA = fixtureA->IsSensor();
	bool sensorB = fixtureB->IsSensor();

	//if neither or both are sensors, will be false
	if ((sensorA ^ sensorB))
	{
		if (sensorA)
		{
			TriggerEnter(fixtureA);
		}
		else if (sensorB)
		{
			TriggerEnter(fixtureB);
		}
	}

	b2Filter filterA = fixtureA->GetFilterData();
	b2Filter filterB = fixtureB->GetFilterData();

	if ((filterA.categoryBits == PLAYER && filterB.categoryBits == GROUND) || (filterB.categoryBits == PLAYER && filterA.categoryBits == GROUND))
	{
		if (filterA.categoryBits == PLAYER)
		{
			ECS::GetComponent<CanJump>((int)fixtureA->GetBody()->GetUserData()).m_canJump = true;
		}
		else if (filterB.categoryBits == PLAYER)
		{
			ECS::GetComponent<CanJump>((int)fixtureB->GetBody()->GetUserData()).m_canJump = true;
		}
	}

	//Bullet and enemy contact
	if (filterA.categoryBits == FRIENDLY || filterB.categoryBits == FRIENDLY)
	{
		if (filterA.categoryBits == ENEMY)
		{
			ECS::GetComponent<HP>((int)fixtureA->GetBody()->GetUserData()).hp -= 10;
			ECS::GetComponent<BulletCollide>((int)fixtureB->GetBody()->GetUserData()).collided = true;

		}
		else if (filterB.categoryBits == ENEMY)
		{
			ECS::GetComponent<HP>((int)fixtureB->GetBody()->GetUserData()).hp -= 10;
			ECS::GetComponent<BulletCollide>((int)fixtureA->GetBody()->GetUserData()).collided = true;
		}
		else if (filterA.categoryBits == GROUND)
		{
			ECS::GetComponent<BulletCollide>((int)fixtureB->GetBody()->GetUserData()).collided = true;
		}
		else if (filterB.categoryBits == GROUND)
		{
			ECS::GetComponent<BulletCollide>((int)fixtureA->GetBody()->GetUserData()).collided = true;
		}
	}

}

void PhysicsPlaygroundListener::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	bool sensorA = fixtureA->IsSensor();
	bool sensorB = fixtureB->IsSensor();

	//if neither or both are sensors, will be false
	if ((sensorA ^ sensorB))
	{
		if (sensorA)
		{
			TriggerExit(fixtureA);
		}
		else if (sensorB)
		{
			TriggerExit(fixtureB);
		}
	}
}

void PhysicsPlaygroundListener::TriggerEnter(b2Fixture* sensor)
{
	int entity = (int)sensor->GetBody()->GetUserData();

	ECS::GetComponent<Trigger*>(entity)->OnEnter();
}

void PhysicsPlaygroundListener::TriggerExit(b2Fixture* sensor)
{
	int entity = (int)sensor->GetBody()->GetUserData();

	ECS::GetComponent<Trigger*>(entity)->OnExit();
}
