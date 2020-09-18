#include "ShootingGameProcess.h"
#include "../Scene.h"

void ShootingGameProcess::Update()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (IsSceneChange)
		{
			Scene::Getinstance().RequestChangeScene("Data/Scene/ActionGame.json");
		}
		IsSceneChange = false;
	}
	else
	{
		IsSceneChange = true;
	}
}
