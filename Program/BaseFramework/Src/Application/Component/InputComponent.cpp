#include"InputComponent.h"

InputComponent::InputComponent(GameObject& owner):m_owner(owner)
{
	//操作軸初期化
	for (auto& axis : m_axes)
	{
		axis = { 0.0f,0.0f };
	}

	m_buttons.fill(Free);
}

void InputComponent::PushButton(Input::Buttons no)
{
	assert(no != Input::Buttons::BTN_MAX);
	if (m_buttons[no] & Stay)
	{
		m_buttons[no] &= ~Enter;  //Enter反転とAND = EnterだけをOFF
		m_buttons[no] &= ~Stay;   //Stay反転とAND  = StayだけをOFF
	}

	else
	{
		m_buttons[no] |= Enter;   //EnterをOR = EnterをON
		m_buttons[no] |= Stay;    //StayをOR = StayをON
	}
}

void InputComponent::ReleaseButton(Input::Buttons no)
{
	assert(no != Input::Buttons::BTN_MAX);
	//押している
	if (m_buttons[no] & Stay)
	{
		m_buttons[no] &= ~Enter; //Enter反転とAND = EnterだけをOFF
		m_buttons[no] &= ~Stay;  //Stay反転とAND  = StayだけをOFF
		m_buttons[no] |= Exit;   //ExitをOR = ExitをON

	}

	else
	{
		m_buttons[no] &= ~Exit;  //Exit反転とAND = ExitだけをOFF
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
	if (GetAsyncKeyState(VK_UP) & 0x8000)    { m_axes[Input::Axes::L].y = 1.0f; }
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)  { m_axes[Input::Axes::L].y = -1.0f; }
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { m_axes[Input::Axes::L].x = 1.0f; }
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)  { m_axes[Input::Axes::L].x = -1.0f; }

	//右の値
	if (GetAsyncKeyState('W') & 0x8000) { m_axes[Input::Axes::R].y = 1.0f; }
	if (GetAsyncKeyState('A') & 0x8000) { m_axes[Input::Axes::R].y = -1.0f; }
	if (GetAsyncKeyState('S') & 0x8000) { m_axes[Input::Axes::R].x = -1.0f; }
	if (GetAsyncKeyState('D') & 0x8000) { m_axes[Input::Axes::R].x = 1.0f; }


	//ボタン入力処理
	if (GetAsyncKeyState('Z')) { PushButton(Input::Buttons::A); }
	else { ReleaseButton(Input::Buttons::A); }

	if (GetAsyncKeyState('X')) { PushButton(Input::Buttons::B); }
	else { ReleaseButton(Input::Buttons::B); }

	if (GetAsyncKeyState('C')) { PushButton(Input::Buttons::Y); }
	else { ReleaseButton(Input::Buttons::Y); }

	if (GetAsyncKeyState('V')) { PushButton(Input::Buttons::X); }
	else { ReleaseButton(Input::Buttons::X); }

	if (GetAsyncKeyState('Q')) { PushButton(Input::Buttons::L1); }
	else { ReleaseButton(Input::Buttons::L1); }

	if (GetAsyncKeyState('E')) { PushButton(Input::Buttons::R1); }
	else { ReleaseButton(Input::Buttons::R1); }
}