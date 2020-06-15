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
}

class InputComponent
{
public:
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
protected:

	//操作軸
	std::array<Math::Vector2, Input::Axes::AXS_MAX> m_axes;
	//持ち主
	GameObject& m_owner;
};


//キーボード用入力コンポーネント

class PlayerInputComponent :public InputComponent
{
public:
	PlayerInputComponent(GameObject& owner) :InputComponent(owner) {}

	virtual void Update() override;
};