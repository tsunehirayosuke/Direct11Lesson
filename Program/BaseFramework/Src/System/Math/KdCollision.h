#pragma once
struct KdRayResult
{
	float m_distance = -1;//“–‚½‚Á‚½‚Æ‚±‚ë‚Ü‚Å‚Ì‹——£
	bool  m_hit = false;//“–‚½‚Á‚½‚©‚Ç‚¤‚©
};

//’Ç‰Á
//ƒŒƒC‚É‚æ‚é“–‚½‚è”»’è
bool KdRayToMesh(const DirectX::XMVECTOR& rRayPos, const DirectX::XMVECTOR& rRayDir, float maxDistance,
	const KdMesh& rMesh, const KdMatrix& rMatrix, KdRayResult& rResult);
