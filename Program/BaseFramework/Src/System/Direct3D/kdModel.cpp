#include"kdModel.h"
#include "KdGLTFLoader.h"

//コンストラクタ
kdModel::kdModel()
{

}

//デストラクタ
kdModel::~kdModel()
{
	if (m_spMesh)
	{
		m_spMesh.reset();
	}
}

//	ロード関数
bool kdModel::Load(const std::string& filename)
{
	std::string fileDir = KdGetDirFromPath(filename);
	
	auto spModel = KdLoadGLTFModel(filename);

	if (spModel==nullptr)
	{
		return false;
	}

	//メッシュの受け取り
	for (UINT i = 0; i < spModel->Nodes.size(); ++i)
	{
		const KdGLTFNode& rNode = spModel->Nodes[i];
		if (rNode.IsMesh)
		{
			m_spMesh = std::make_shared<KdMesh>();
			if (m_spMesh)
			{
				//頂点情報配列　面情報配列　サブセット情報配列
				m_spMesh->Create(rNode.Mesh.Vertices, rNode.Mesh.Faces, rNode.Mesh.Subsets);
				break;
			}
		}
	}

	//マテリアル配列を受け取れるサイズのメモリを確保
	m_materials.resize(spModel->Materials.size());

	for (UINT i = 0; i < m_materials.size(); ++i)
	{

		//src = Sroce
		//dst = Destination
		const KdGLTFMaterial& rSrcMaterial = spModel->Materials[i];
		KdMaterial& rDstMaterial = m_materials[i];

		//名前
		rDstMaterial.Name = rSrcMaterial.Name;

		//基本色
		rDstMaterial.BaseColor = rSrcMaterial.BaseColor;
		rDstMaterial.BaseColorTex = std::make_shared<KdTexture>();

		//テクスチャ読み込み
		/*if (rDstMaterial.BaseColorTex->Load(fileDir + rSrcMaterial.BaseColorTexture) == false)
		{
			rDstMaterial.BaseColorTex = D3D.GetWhiteTex();
		}*/
		rDstMaterial.BaseColorTex = KdResFac.GetTexture(fileDir + rSrcMaterial.BaseColorTexture);
		if (rDstMaterial.BaseColorTex == nullptr)
		{
			rDstMaterial.BaseColorTex = D3D.GetWhiteTex();
		}
	}
	return true;
}