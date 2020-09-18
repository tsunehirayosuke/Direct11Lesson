#pragma once

#include "../GameObject.h"

class Human : public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj) override;
	virtual void Update() override;

	bool IsGround() { return m_isGround; }

private:
	void UpdateMove();//操作・キャラの行動による移動
	
	float m_moveSpeed = 0.1f; //キャラの移動速度
	KdVec3 m_pos; //ワールド行列上の座標

	void UpdateCamera(); //操作によるカメラの回転と移動
	float m_camRotSpeeed = 0.2f; //カメラの回転速度

	void UpdateRotate(const KdVec3& rMoveDir);
	float m_rotateAngle = 10.0f;
	KdVec3 m_rot;

	float m_gravity = 0.01f;//重力の強さ
	KdVec3 m_force;//キャラクターにかかる移動させる力

	//static const float s_allowToStepHeight;//歩いて乗り越えられる段差の高さ
	//static const float s_landingHeight;//地面から足が離れていても着地していると判定する高さ

	void UpdateCollision();					//当たり判定全般
	bool CheckGround(float& rDstDistance);	//地面との当たり判定

	KdVec3 m_prevPos;//１フレーム前の座標
	bool m_isGround; //着地しているかどうか
};