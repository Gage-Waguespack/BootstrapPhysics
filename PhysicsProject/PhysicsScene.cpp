#include "PhysicsScene.h"
#include "PhysicsObject.h"
#include "Sphere.h"
#include "Plane.h"
#include "glm/ext.hpp"

PhysicsScene::PhysicsScene() : m_timeStep(0.01f), m_gravity(glm::vec2(0, 0))
{
}

PhysicsScene::~PhysicsScene()
{
}

void PhysicsScene::addActor(PhysicsObject* actor)
{
	m_actors.insert(actor);
}

void PhysicsScene::removeActor(PhysicsObject* actor)
{
	m_actors.erase(actor);
}

void PhysicsScene::update(float deltaTime)
{
	static float accumulatedTime = 0.0f;
	accumulatedTime += deltaTime;

	//As long as there is enough accumulated time for a fixedUpdate...
	while (accumulatedTime >= m_timeStep)
	{
		//...call each actor's fixedUpdate
		for (PhysicsObject* actor : m_actors)
		{
			actor->fixedUpdate(m_gravity, m_timeStep);
		}
		accumulatedTime -= m_timeStep;

		auto outerEnd = m_actors.end();
		outerEnd--;
		for (auto outer = m_actors.begin(); outer != outerEnd; outer++)
		{
			auto innerBegin = outer;
			innerBegin++;
			for (auto inner = innerBegin; inner != m_actors.end(); inner++)
			{
				PhysicsObject* object1 = *outer;
				PhysicsObject* object2 = *inner;

				//Collision checks
				sphereToSphere(object1, object2);
				sphereToPlane(object1, object2);
				planeToSphere(object1, object2);
			}
		}
	}
}

void PhysicsScene::draw()
{
	for (PhysicsObject* actor : m_actors)
	{
		actor->draw();
	}
}

bool PhysicsScene::planeToPlane(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}

bool PhysicsScene::planeToSphere(PhysicsObject* object1, PhysicsObject* object2)
{
	return sphereToPlane(object1, object2);
}

bool PhysicsScene::planeToBox(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}

bool PhysicsScene::sphereToPlane(PhysicsObject* object1, PhysicsObject* object2)
{
	Sphere* sphere = dynamic_cast<Sphere*>(object1);
	Plane* plane = dynamic_cast<Plane*>(object2);

	if (sphere && plane) 
	{
		//D1 = (C dot N) - D - R
		//D1 is the distance from the sphere surface to the plane surface
		//C is the center of the sphere
		//N is the normal of the plane
		//D is the distance from the plane to the origin
		//R is the radius of the sphere
		glm::vec2 sphereCenter = sphere->getPosition();
		glm::vec2 planeNormal = plane->getNormal();
		float planeDistance = plane->getDistance();
		float sphereRadius = sphere->getRadius();
		float sphereToPlaneDistance = glm::dot(sphereCenter, planeNormal) - planeDistance - sphereRadius;

		if (sphereToPlaneDistance <= 0)
		{
			sphere->applyForce(-sphere->getVelocity() * sphere->getMass());
			return true;
		}
	}

	return false;
}

bool PhysicsScene::sphereToSphere(PhysicsObject* object1, PhysicsObject* object2)
{
	Sphere* sphere1 = dynamic_cast<Sphere*>(object1);
	Sphere* sphere2 = dynamic_cast<Sphere*>(object2);

	if (sphere1 && sphere2)
	{
		//Find the distance
		glm::vec2 position1 = sphere1->getPosition();
		glm::vec2 position2 = sphere2->getPosition();
		glm::vec2 distanceVec = position1 - position2;
		float distance = glm::sqrt(distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y);

		//If the distance is less than the combined radii, there is a collision
		if (glm::abs(distance) < sphere1->getRadius() + sphere2->getRadius())
		{
			sphere1->applyForce(-(sphere1->getVelocity()) * sphere1->getMass());
			sphere2->applyForce(-(sphere2->getVelocity()) * sphere2->getMass());
			return true;
		}
	}

	return false;
}

bool PhysicsScene::sphereToBox(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}

bool PhysicsScene::boxToPlane(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}

bool PhysicsScene::boxToSphere(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}

bool PhysicsScene::boxToBox(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}
