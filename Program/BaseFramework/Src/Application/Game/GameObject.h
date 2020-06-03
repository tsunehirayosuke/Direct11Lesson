#pragma once

class GameObject
{
public:
	GameObject();
	~GameObject();

	virtual void Deserialise();
	virtual void Update();
	virtual void Draw();
protected:

	void Release();

	kdModel   * m_pModel = nullptr;
	KdMatrix  m_mWorld;
};