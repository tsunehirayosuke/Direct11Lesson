#include"kdModel.h"
#include "KdGLTFLoader.h"

//コンストラクタ
kdModel::kdModel()
{

}

//デストラクタ
kdModel::~kdModel()
{
	Release();
}
void kdModel::Release()
{
	m_materials.clear();
	m_originalNodes.clear();
}

//	ロード関数
bool kdModel::Load(const std::string& filename)
{
	//ファイルの完全パスを取得
	std::string fileDir = KdGetDirFromPath(filename);

	//GLTFの読み込み
	std::shared_ptr<KdGLTFModel> spGltfModel = KdLoadGLTFModel(filename);
	if (spGltfModel == nullptr) { return false; }

	//ノード格納場所のメモリ確保
	m_originalNodes.resize(spGltfModel->Nodes.size());

	//メッシュの受け取り
	for (UINT i = 0; i < spGltfModel->Nodes.size(); i++)
	{
		//入力元ノード
		const KdGLTFNode& rSrcNode = spGltfModel->Nodes[i];

		//出力先ノード
		Node& rDstNode = m_originalNodes[i];

		//ノード情報のセット
		rDstNode.m_name = rSrcNode.Name;
		rDstNode.m_localTransform = rSrcNode.LocalTransform;

		//ノード内容がメッシュだったら
		if (rSrcNode.IsMesh)
		{
			rDstNode.m_spMesh = std::make_shared<KdMesh>();

			if (rDstNode.m_spMesh)
			{
				rDstNode.m_spMesh->Create
				(rSrcNode.Mesh.Vertices, rSrcNode.Mesh.Faces, rSrcNode.Mesh.Subsets);
			}
		}
	}

	m_materials.resize(spGltfModel->Materials.size());

	for (UINT i = 0; i < m_materials.size(); i++)
	{
		const KdGLTFMaterial& rSrcMaterial = spGltfModel->Materials[i];
		KdMaterial& rDstMaterial = m_materials[i];

		rDstMaterial.Name = rSrcMaterial.Name;

		rDstMaterial.BaseColor = rSrcMaterial.BaseColor;
		rDstMaterial.BaseColorTex = std::make_shared<KdTexture>();

		rDstMaterial.BaseColorTex = KdResFac.GetTexture(fileDir + rSrcMaterial.BaseColorTexture);
		if (rDstMaterial.BaseColorTex == nullptr)
		{
			rDstMaterial.BaseColorTex = D3D.GetWhiteTex();
		}
	}

	return true;
}