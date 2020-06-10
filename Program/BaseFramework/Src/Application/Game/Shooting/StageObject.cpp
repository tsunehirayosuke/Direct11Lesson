#include "StageObject.h"


void::StageObject::Deserialize()
{
	m_spModel = std::make_shared<kdModel>();
	if (m_spModel == nullptr)
	{
		return;
	}

	if (m_spModel->Load("Data/StageMap/StageMap.gltf") == false)
	{
		m_spModel.reset();
	}
}