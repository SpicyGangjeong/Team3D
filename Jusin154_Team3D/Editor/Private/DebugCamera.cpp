#include "pch.h"
#include "DebugCamera.h"
#include "Shader.h"

CDebugCamera::CDebugCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CDebugCamera::CDebugCamera(const CDebugCamera& rhs)
	: CCamera(rhs)
{
}

void CDebugCamera::Priority_Update(_float fTimeDelta)
{
}

void CDebugCamera::Update(_float fTimeDelta)
{
	_float3 vCamPos = {};
	XMStoreFloat3(&vCamPos, m_pTransformCom->Get_State(STATE::POSITION));
	GUI::Text("Cam Coord %.2f, %.2f, %.2f", vCamPos.x, vCamPos.y, vCamPos.z);
	if (m_pGameInstance->Key_Up(DIK_GRAVE)) {
		m_bMovable = !m_bMovable;
	}
	if (m_pGameInstance->Mouse_Down(DIM_RBUTTON)) {
		m_pGameInstance->Toggle_MouseCenter();
	}
	if (m_bMovable) {
#pragma region Position
		_float fSpeed = 5.f;
		if (m_pGameInstance->Key_Pressing(DIK_W)) {
			m_pTransformCom->Go_Straight(fTimeDelta * fSpeed);
		}
		if (m_pGameInstance->Key_Pressing(DIK_A)) {
			m_pTransformCom->Go_Left(fTimeDelta * fSpeed);
		}
		if (m_pGameInstance->Key_Pressing(DIK_S)) {
			m_pTransformCom->Go_Backward(fTimeDelta * fSpeed);
		}
		if (m_pGameInstance->Key_Pressing(DIK_D)) {
			m_pTransformCom->Go_Right(fTimeDelta * fSpeed);
		}
		if (m_pGameInstance->Key_Pressing(DIK_SPACE)) {
			m_pTransformCom->Go_Up(fTimeDelta * fSpeed);
		}
		if (m_pGameInstance->Key_Pressing(DIK_C)) {
			m_pTransformCom->Go_Down(fTimeDelta * fSpeed);
		}
		if (m_pGameInstance->Key_Pressing(DIK_TAB)) {
			Set_InitialPos();
		}

#pragma endregion
#pragma region Angle

		_float3	fMove = m_pGameInstance->Get_MouseMove();
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * fMove.x * m_fMouseSensor);
		m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * fMove.y * m_fMouseSensor);

#pragma endregion

	}
	__super::Bind_Matrices();
}

void CDebugCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CDebugCamera::Render()
{
	return S_OK;
}

HRESULT CDebugCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDebugCamera::Initialize(void* pArg)
{
	CAMERA_DEBUG_DESC* pDesc = static_cast<CAMERA_DEBUG_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vEye), 1.f));
	m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&pDesc->vAt), 1.f));
	m_matInitial = *m_pTransformCom->Get_WorldMatrixPtr();
	m_bActive = true;

	m_pGameInstance->Add_Camera(NEXT_LEVEL, this, m_pCameraKey);
	m_pGameInstance->Bind_Camera(NEXT_LEVEL, m_pCameraKey, false);
	return S_OK;
}

HRESULT CDebugCamera::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDebugCamera::Bind_ShaderResources()
{
	return S_OK;
}

void CDebugCamera::Set_InitialPos()
{
	m_pTransformCom->Set_WorldMatrix(m_matInitial);
}

CDebugCamera* CDebugCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebugCamera* pInstance = new CDebugCamera(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDebugCamera");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDebugCamera::Clone(void* pArg, CGameObject* pOwner)
{
	CDebugCamera* pInstance = new CDebugCamera(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDebugCamera");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDebugCamera::Free()
{
	__super::Free();
}

void CDebugCamera::Describe_Entity()
{
}
