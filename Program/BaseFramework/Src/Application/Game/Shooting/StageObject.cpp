#include "StageObject.h"

#include "../../Component/ModelComponent.h"

void::StageObject::Deserialize()
{
	if (m_spModelComponent)
	{
		m_spModelComponent->SetModel(KdResFac.GetModel("Data/StageMap/StageMap.gltf"));
	}
}