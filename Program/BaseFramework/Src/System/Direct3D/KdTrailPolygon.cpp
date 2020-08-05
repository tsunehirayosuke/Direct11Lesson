#include "KdTrailPolygon.h"

void KdTrailPolygon::Draw(float width)
{
	if (m_pointList.size() < 2) { return; }

	float sliceCount = (float)(m_pointList.size() - 1);

	std::vector<Vertex> vertex;

	vertex.resize(m_pointList.size() * 2);


	//頂点データ
	//頂点データ作成
	for (UINT i = 0; i < m_pointList.size(); i++)
	{
		//登録行列の参照
		Math::Matrix& mat = m_pointList[i];

		//2つの頂点の参照
		Vertex& v1 = vertex[i * 2];
		Vertex& v2 = vertex[i * 2 + 1];

		//X方向
		Math::Vector3 axisX = mat.Right();
		axisX.Normalize();

		//座標
		v1.Pos = mat.Translation() + axisX * width * 0.5f;
		v2.Pos = mat.Translation() - axisX * width * 0.5f;

		//UV
		float uvY = i / sliceCount;
		v1.UV = { 0,uvY };
		v2.UV = { 1,uvY };
	}

	//テクスチャセット
	if (m_texture)
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, m_texture->GetSRViewAddress());
	}
	else
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, D3D.GetWhiteTex()->GetSRViewAddress());
	}

	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, vertex.size(), &vertex[0], sizeof(Vertex));

}

void KdTrailPolygon::DrawBillboard(float width)
{
	if (m_pointList.size() < 2) { return; }

	Math::Matrix mCam = SHADER.m_cb7_Camera.GetWork().mV.Invert();

	float sliceCount = (float)(m_pointList.size() - 1);

	std::vector<Vertex> vertex;

	vertex.resize(m_pointList.size() * 2);


	//頂点データ作成
	Math::Vector3 prevPos;
	for (UINT i = 0; i < m_pointList.size(); i++)
	{
		//登録行列の参照
		Math::Matrix& mat = m_pointList[i];

		//2つの頂点の参照
		Vertex& v1 = vertex[i * 2];
		Vertex& v2 = vertex[i * 2 + 1];

		Math::Vector3 vDir;
		if (i == 0)
		{
			//初回時のみ、次のポイントを使用
			vDir = m_pointList[1].Translation();
		}
		else
		{
			//二回目以降は前回の座標から向きを決定する
			vDir = mat.Translation() - prevPos;
		}

		//カメラからポイントへの向き
		Math::Vector3 v = mat.Translation() - mCam.Translation();
		Math::Vector3 axisX = DirectX::XMVector3Cross(vDir, v);
		axisX.Normalize();

		//座標
		v1.Pos = mat.Translation() + axisX * width * 0.5f;
		v2.Pos = mat.Translation() - axisX * width * 0.5f;

		//UV
		float uvY = i / sliceCount;
		v1.UV = { 0,uvY };
		v2.UV = { 1,uvY };

		//座標を記録しておく
		prevPos = mat.Translation();
	}

	//テクスチャセット
	if (m_texture)
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, m_texture->GetSRViewAddress());
	}
	else
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, D3D.GetWhiteTex()->GetSRViewAddress());
	}

	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, vertex.size(), &vertex[0], sizeof(Vertex));

}
