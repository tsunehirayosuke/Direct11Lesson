#pragma once

class Missile;

#include"../GameObject.h"

class AirCraft : public GameObject
{
public:

	void Deserialize(const json11::Json& jsonObj)override; //初期化:オブジェクト作成用外部データの解釈
	void Update();		//更新


	void Draw() override;//描画

	void OnNotify_Damage(int damage);

	void DrawEffect() override;

private:

	void UpdateMove();	//移動更新処理
	void UpdateShoot(); //発射関数
	void UpdateCollision(); //当たり判定処理
	void UpdatePropeller();//プロペラ更新

	float m_propRotSpeed = 0.0f;//プロペラの回転速度

	float		m_Speed = 0.2f;			//移動速度
	bool		mcanShoot  = true;		//発射可能かどうか

	KdVec3		m_prevPos = {};			//1フレーム前の座標

	bool m_laser = false;		  //レーザー発射
	float m_laserRange = 1000.0f; //レーザーの射程

	int m_hp = 10;//０になるとリストから抜く
	int m_attackPow = 1;

	KdTrailPolygon m_propTrail;


	//基底アクションステート
	class BaseAction
	{
	public:
		virtual void Update(AirCraft& owner) = 0;
	};

	//飛行中
	class ActionFly : public BaseAction
	{
	public:
		virtual void Update(AirCraft& owner)override;
	};

	//墜落中
	class ActionCrash : public BaseAction
	{
	public:
		virtual void Update(AirCraft& owner)override;
		
		int m_timer = 180;
	};

	
	std::shared_ptr<BaseAction> m_spActionState;
};