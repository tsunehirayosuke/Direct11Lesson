#pragma once

#include"../GameObject.h"

class Missile : public GameObject
{
public:

	void Deserialize(const json11::Json& jsonObj)override; //初期化:オブジェクト作成用外部データの解釈
	void Update();		//更新
	
	inline void SetTarget(const std::shared_ptr<GameObject>& spTarget) { m_wpTarget = spTarget; }

	//当たり判定の更新
	void UpdateCollision();

	//撃ったキャラクターを登録
	inline void SetOwner(const std::shared_ptr<GameObject>& spOwner) { m_wpOwner = spOwner; }
private:
	KdVec3 m_prevPos;

	std::weak_ptr<GameObject> m_wpOwner;

	float		m_speed  = 0.5f;
	int         m_lifeSpan = 0;
	std::weak_ptr<GameObject> m_wpTarget;
};