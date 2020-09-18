#pragma once

#include "../GameProcess.h"

//ゲームシーンの管理者をカテゴリ分けするために、GameProcessにGameObjectを継承
class ActionGameProcess : public GameProcess
{
public:
	ActionGameProcess() {}
	virtual ~ActionGameProcess() {}

	void Update() override;

	bool IsSceneChange = false;
};