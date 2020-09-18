#pragma once

class GameObject;

//モデルコンポーネント

class ModelComponent
{
public:
	ModelComponent(GameObject& rOwner) : m_rOwner(rOwner){}

	//有効フラグ
	bool IsEnable()const { return m_enable; }
	void SetEnable(bool enable) { m_enable = enable; }

	//モデル取得
	const std::vector<kdModel::Node>& GetNodes() const { return m_coppieNodes; }

	//メッシュ取得
	inline const std::shared_ptr<KdMesh>GetMesh(UINT index)const
	{
		if (index >= m_coppieNodes.size()) { return nullptr; }
		return m_coppieNodes[index].m_spMesh;
	}

	inline kdModel::Node* FindNode(const std::string& name)
	{
		for (auto&& node : m_coppieNodes)
		{
			if (node.m_name == name)
			{
				return &node;
			}
		}
		return nullptr;
	}

	//モデルセット
	void SetModel(const std::shared_ptr<kdModel>& rModel);

	void LoadModel(const std::string& filename);

	//StandardShaderで描画
	void Draw();

private:

	std::vector<kdModel::Node> m_coppieNodes;

	//有効
	bool m_enable = true;

	//モデルデータの参照
	std::shared_ptr<kdModel> m_spModel;

	GameObject& m_rOwner;
};