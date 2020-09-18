#include "ActionGameProcess.h"
#include "../Scene.h"

void ActionGameProcess::Update()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (IsSceneChange)
		{
			Scene::Getinstance().RequestChangeScene("Data/Scene/ShootingGame.json");
		}
		IsSceneChange = false;
	}
	else
	{
		IsSceneChange = true;
	}
}
