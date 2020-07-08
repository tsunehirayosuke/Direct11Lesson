﻿#pragma once

#include"../GameObject.h"

class Missile : public GameObject
{
public:

	void Deserialize(const json11::Json& jsonObj)override; //初期化:オブジェクト作成用外部データの解釈
	void Update();		//更新
	
	inline void SetTarget(const std::shared_ptr<GameObject>& spTarget) { m_wpTarget = spTarget; }
private:

	float		m_speed  = 0.5f;
	int         m_lifeSpan = 0;
	std::weak_ptr<GameObject> m_wpTarget;
};