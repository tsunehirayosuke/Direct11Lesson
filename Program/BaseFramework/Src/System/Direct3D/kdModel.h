﻿#pragma once

class kdModel
{
public:
	kdModel();
	~kdModel();

	bool Load(const std::string& filename);

	//アクセサ

	const std::shared_ptr<KdMesh> GetMesh(UINT index) const
	{
		return index < m_originalNodes.size() ? m_originalNodes[index].m_spMesh : nullptr;
	}

	const std::vector<KdMaterial>& GetMaterials()const { return m_materials; }

	struct Node
	{
		std::string m_name;
		KdMatrix m_localTransform;
		std::shared_ptr<KdMesh> m_spMesh;
	};

	inline Node* FindNode(const std::string& name)
	{
		for (auto&& node : m_originalNodes)
		{
			if (node.m_name == name) { return &node; }
		}
		return nullptr;
	}

	const std::vector<Node>& GetOriginalNodes() const { return m_originalNodes; }
private:

	void Release();
	std::vector<Node> m_originalNodes;

	//マテリアル配列
	std::vector<KdMaterial> m_materials;

};