#include "ModelComponent.h"
#include "../Game/GameObject.h"

void ModelComponent::SetModel(const std::shared_ptr<kdModel>& rModel)
{
	//使用しているモデルをセット
	m_spModel = rModel;

	//念のため、こーぴー用の配列クリア
	m_coppieNodes.clear();

	//ノードのコピー
	if (rModel)
	{
		m_coppieNodes = rModel->GetOriginalNodes();
	}
}

void ModelComponent::LoadModel(const std::string& filename)
{
	m_spModel = std::make_shared<kdModel>();

	if (m_spModel->Load(filename) == false)
	{
		m_spModel = nullptr;
	}
}

void ModelComponent::Draw()
{
	//有効じゃないときはスキップ
	if (m_enable == false) { return; }

	if (m_spModel == nullptr) { return; }

	//全てのノードを一つ一つ描画
	for (UINT i = 0; i < m_coppieNodes.size(); i++)
	{
		auto& rNode = m_coppieNodes[i];
		if (rNode.m_spMesh == nullptr) { continue; }
		SHADER.m_standardShader.SetWorldMatrix(rNode.m_localTransform * m_rOwner.GetMatrix());
		SHADER.m_standardShader.DrawMesh(rNode.m_spMesh.get(), m_spModel->GetMaterials());
	}

}