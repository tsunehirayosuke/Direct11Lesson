#include"GameObject.h"

#include"../Component/CameraComponent.h"
#include"../Component/InputComponent.h"
GameObject::GameObject()
{

}

GameObject::~GameObject()
{
	Release();
}

void GameObject::Deserialise(){}

void GameObject::Update(){}

void GameObject::Draw()
{
	if (m_spModel == nullptr)
	{
		return;
	}
	SHADER.m_standardShader.SetWorldMatrix(m_mWorld);
	SHADER.m_standardShader.DrawMesh(m_spModel->GetMesh().get(), m_spModel->GetMaterials());
}

void GameObject::Release() 
{
	if (m_spModel)
	{
		m_spModel = nullptr;
	}
}