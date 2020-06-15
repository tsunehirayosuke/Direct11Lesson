#include"InputComponent.h"

InputComponent::InputComponent(GameObject& owner):m_owner(owner)
{
	//操作軸初期化
	for (auto& axis : m_axes)
	{
		axis = { 0.0f,0.0f };
	}
}

void  PlayerInputComponent::Update()
{
	//操作軸初期化
	for (auto& axis : m_axes)
	{
		axis = { 0.0f,0.0f };
	}
	//左の軸の値
	if (GetAsyncKeyState(VK_UP) & 0x8000) { m_axes[Input::Axes::L].y = 1.0f; }
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) { m_axes[Input::Axes::L].y = -1.0f; }
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { m_axes[Input::Axes::L].x = 1.0f; }
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) { m_axes[Input::Axes::L].x = -1.0f; }
}