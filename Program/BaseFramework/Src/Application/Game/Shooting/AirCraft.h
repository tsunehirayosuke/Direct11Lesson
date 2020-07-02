#pragma once

class Missile;

#include"../GameObject.h"

class AirCraft : public GameObject
{
public:

	void Deserialize(const json11::Json& jsonObj)override; //初期化:オブジェクト作成用外部データの解釈
	void Update();		//更新

	void ImGuiUpdate(); //AirCraftクラス専用のImGui更新

	void UpdateMove();	//移動更新処理
	void UpdateShoot(); //発射関数
	void UpdateCollision(); //当たり判定処理
	
private:

	float		m_Speed = 0.2f;			//移動速度
	bool		mcanShoot  = true;		//発射可能かどうか

	KdVec3		m_prevPos = {};			//1フレーム前の座標
};