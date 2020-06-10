#include"CameraComponent.h"

CameraComponent::CameraComponent(GameObject& owner) : m_owner(owner)
{
	m_mProj.CreateProjectionPerspectiveFov(60 * KdToRadians,
		D3D.GetBackBuffer()->GetAspectRatio(), 0.01f, 5000.0f);

}

CameraComponent::~CameraComponent()
{

}

void CameraComponent::SetCameraMatrix(const KdMatrix& m)
{
	//カメラ行列セット
	m_mCam = m_mOffset * m;

	//カメラ行列からビュー行列を行列
	m_mView = m_mCam;
	m_mView.Inverse();
}


void CameraComponent::SetToShader()
{
	//追従カメラ座標をシェーダーにセット
	SHADER.m_cb7_Camera.Work().CamPos = m_mCam.GetTranslation();

	//追従カメラのビュー行列をシェーダーにセット
	SHADER.m_cb7_Camera.Work().mV = m_mView;

	//追従カメラの射影行列をシェーダーにセット
	SHADER.m_cb7_Camera.Work().mP = m_mProj;

	//カメラ情報(ビュー行列、射影行列)をシェーダーの定数バッファにセット
	SHADER.m_cb7_Camera.Write();
}