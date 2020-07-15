#include "KdCollision.h"

using namespace DirectX;

bool KdRayToMesh(const DirectX::XMVECTOR& rRayPos, const DirectX::XMVECTOR& rRayDir, float maxDistance, const KdMesh& rMesh, const KdMatrix& rMatrix, KdRayResult& rResult)
{
	//モデルの逆行列でレイを変換
	//KdMatrix invMat = rMatrix;
	//invMat.Inverse();

	XMMATRIX invMat = XMMatrixInverse(0, rMatrix);//高速化

	//レイの判定開始位置を逆変換

	XMVECTOR rayPos = XMVector3TransformCoord(rRayPos, invMat);//高速化

	//発射方向を正規化する

	XMVECTOR rayDir = XMVector3TransformCoord(rRayDir, invMat);//高速化

	//逆行列に拡縮が入っていると
	//レイが当たった距離にも拡縮が反映されてしまうので
	//判定用の最大距離にも拡縮を反映させておく
	//float rayCheckRange = maxDistance * XMVector3Length(rayDir).m128_f32[0];
	float dirLength = XMVector3Length(rayDir).m128_f32[0];
	float rayCheckRange = maxDistance * dirLength;

	rayDir = XMVector3Normalize(rayDir);//高速化

	//-------------------------------------------------------
	//ブロードフェイズ
	//　比較的軽量なAABB　VS レイな判定で
	//　明らかにヒットしない場合は、これ以降の判定を中止
	//-------------------------------------------------------

	float AABBdist = FLT_MAX;
	if (rMesh.GetBoundingBox().Intersects(rayPos, rayDir, AABBdist) == false) { return false; }

	//最大距離以降なら範囲外なので中止
	if (AABBdist > rayCheckRange) { return false; }

	bool ret = false;//当たったかどうか
	float closestDist = FLT_MAX;//当たった面との距離

	/*面情報の取得
	const std::shared_ptr<KdMesh>& mesh = m_spModelComponent->GetMesh();//モデル(メッシュ)情報の取得
	const KdMeshFace* pFace = &mesh->GetFaces()[0];//面情報の先頭を取得
	UINT faceNum = mesh->GetFaces().size();//面の総数の取得*/

	const KdMeshFace* pFace = &rMesh.GetFaces()[0];//面情報の先頭を取得
	UINT faceNum = rMesh.GetFaces().size();//面の総数の取得

	//すべての面(三角形)と当たり判定
	for (UINT faceIdx = 0; faceIdx < faceNum; faceIdx++)
	{
		//三角形を構成する３つの頂点のIndex
		const UINT* idx = pFace[faceIdx].Idx;

		//レイと三角形の当たり判定
		float triDist = FLT_MAX;
		bool bHit = DirectX::TriangleTests::Intersects(
			//rInfo.m_pos,	//発射場所
			rayPos,			//発射場所
			rayDir,			//発射方向

			//判定する三角形の頂点情報
			rMesh.GetVertexPositions()[idx[0]],
			rMesh.GetVertexPositions()[idx[1]],
			rMesh.GetVertexPositions()[idx[2]],

			triDist //当たった場合の距離
		);

		//ヒットしていなかったらスキップ
		if (bHit == false) { continue; }

		//最大距離以内か
		if (triDist <= rayCheckRange)
		{
			ret = true;
			//実際の長さを計算
			triDist /= dirLength;

			//近い方を残す
			if (triDist < closestDist)
			{
				closestDist = triDist;
			}
		}
	}
	rResult.m_distance = closestDist;
	rResult.m_hit = ret;
	return ret;
}
