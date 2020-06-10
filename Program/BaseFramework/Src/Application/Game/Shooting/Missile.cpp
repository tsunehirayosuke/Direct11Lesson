#include"Missile.h"

#include"Application//main.h"



void Missile::Deserialize()
{
	m_spModel = std::make_shared<kdModel>();
	if (m_spModel == nullptr) { return; }
	
	if (m_spModel->Load("Data/Missile/Missile.gltf") == false)
	{
		Release();
	}

	m_lifeSpan = APP.m_maxFps * 10;
}

void Missile::Update()
{
	if (m_alive == false) { return; }

	if (--m_lifeSpan <= 0)
	{
		Destroy();
	}

	KdVec3 move = m_mWorld.GetAxisZ();
	move.Normalize();

	move *= m_speed;

	m_mWorld._41 += move.x;
	m_mWorld._42 += move.y;
	m_mWorld._43 += move.z;
}