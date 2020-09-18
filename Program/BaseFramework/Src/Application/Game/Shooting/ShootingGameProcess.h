#pragma once

#include "../GameProcess.h"

//ゲームシーンの管理者をカテゴリ分けするために、GameProcessにGameObjectを継承
class ShootingGameProcess : public GameProcess
{
public:
	ShootingGameProcess() {}
	virtual ~ShootingGameProcess() {}

	void Update() override;

	bool IsSceneChange = false;
};