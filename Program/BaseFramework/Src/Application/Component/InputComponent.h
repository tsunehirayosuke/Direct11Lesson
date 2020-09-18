#pragma once

#include "../Game/GameObject.h"
namespace Input
{
	enum Axes //axisの複数形
	{
		L,
		R,
		AXS_MAX
	};

	enum Buttons
	{
		A,
		B,
		X,
		Y,
		L1,
		R1,
		BTN_MAX
	};
}

class InputComponent
{
public:

	//ボタン状態
	enum
	{
		Free  =0x00000000,
		Enter =0x00000001,
		Stay  =0x00000002,
		Exit  =0x00000004,
	};

	//コンストラクタ：オーナーの設定、ボタンの初期化
	InputComponent(GameObject& owner);
	
	virtual ~InputComponent() {};

	//更新
	virtual void Update() {};

	//操作軸取得
	virtual const Math::Vector2& GetAxis(Input::Axes no) const
	{
		assert(no != Input::Axes::AXS_MAX);
		return m_axes[no];
	}

	//ボタン軸取得
	inline int GetButton(Input::Buttons no)const
	{
		assert(no != Input::Buttons::BTN_MAX);
		return m_buttons[no];
	}

	//ボタンを押す
	void PushButton(Input::Buttons no);
	//ボタンを離す
	void ReleaseButton(Input::Buttons no);

protected:

	//操作軸
	std::array<Math::Vector2, Input::Axes::AXS_MAX> m_axes;
	//ボタン軸
	std::array<int, Input::Buttons::BTN_MAX> m_buttons;
	//持ち主
	GameObject& m_owner;
};

///////////////////////////////////////
//キーボード用入力コンポーネント
///////////////////////////////////////
class PlayerInputComponent :public InputComponent
{
public:
	PlayerInputComponent(GameObject& owner) :InputComponent(owner) {}

	virtual void Update() override;
};
///////////////////////////////////////
//敵用入力コンポーネント
///////////////////////////////////////
class EnemyInputComponent :public InputComponent
{
public:
	EnemyInputComponent(GameObject& owner) :InputComponent(owner) {}

	virtual void Update() override;

};

//地面を動き回る用のInputComponent
class ActionPlayerInputComponent : public InputComponent
{
public:
	ActionPlayerInputComponent(GameObject& rOwner) : InputComponent(rOwner){}

	virtual void Update()override;

	POINT m_prevMousePos;
};