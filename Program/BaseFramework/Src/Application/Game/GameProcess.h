#pragma once

#include "GameObject.h"

//ゲームシーンの管理者をカテゴリ分けするために、GameProcessにGameObjectを継承
class GameProcess : public GameObject
{
public:
	GameProcess() {}
	virtual ~GameProcess(){}
};