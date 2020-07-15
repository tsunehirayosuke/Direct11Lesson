#pragma once
struct KdRayResult
{
	float m_distance = -1;//���������Ƃ���܂ł̋���
	bool  m_hit = false;//�����������ǂ���
};

//�ǉ�
//���C�ɂ�铖���蔻��
bool KdRayToMesh(const DirectX::XMVECTOR& rRayPos, const DirectX::XMVECTOR& rRayDir, float maxDistance,
	const KdMesh& rMesh, const KdMatrix& rMatrix, KdRayResult& rResult);
