#include "pch.h"
#include "Camera_Debug.h"
#include "Shader.h"
#include "Layer.h"
#include "Player.h"


CCamera_Debug::CCamera_Debug(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Debug::CCamera_Debug(const CCamera_Debug& rhs)
	: CCamera(rhs)
{
}

void CCamera_Debug::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Bind_Camera(NEXT_LEVEL, CAMERA_DEBUG, false);
#endif // _DEBUG

	if (FAILED(m_pGameInstance->IsBinded_Camera(CAMERA_DEBUG))) {
		return;
	}
	Transition(fTimeDelta);
	_float3 vCamPos = {};
	_float3 vSetPos = {};
	XMStoreFloat3(&vCamPos, m_pTransformCom->Get_State(STATE::POSITION));

	if (m_pGameInstance->Key_Up(DIK_GRAVE)) {
		m_bMovable = !m_bMovable;
	}
	if (m_pGameInstance->Mouse_Down(DIM_MBUTTON)) {
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
		if (m_pGameInstance->Key_Pressing(DIK_LCONTROL)) {
			if (m_pGameInstance->Key_Up(DIK_MINUS)) {
				CLayer* pLayer = m_pGameInstance->Get_Layer(CURRENT_LEVEL, LAYER_PLAYER);
				if (nullptr != pLayer) {
					CPlayer* pPlayer = pLayer->Get_Object<CPlayer>();
					pPlayer->Get_Component<CCharacter_Controller>()->Set_Position(Get_WorldPostion());
				}
			}
		}

		_long LWheel = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::W);
		if (LWheel > 0)
		{
			m_pTransformCom->Add_SpeedPerSec(0.5f);
		}
		else if (LWheel < 0)
			m_pTransformCom->Add_SpeedPerSec(-0.5f);

#pragma endregion
#pragma region Angle

		_float3	fMove = m_pGameInstance->Get_MouseMove();
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * fMove.x * m_fMouseSensor);
		m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * fMove.y * m_fMouseSensor);

#pragma endregion

	}
	__super::Bind_Matrices();
}

void CCamera_Debug::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG
}

void CCamera_Debug::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->IsBinded_Camera(CAMERA_DEBUG))) {
		return;
	}
}

HRESULT CCamera_Debug::Render()
{
	return S_OK;
}

HRESULT CCamera_Debug::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Debug::Initialize(void* pArg)
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

	return S_OK;
}

HRESULT CCamera_Debug::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCamera_Debug::Bind_ShaderResources()
{
	return S_OK;
}

void CCamera_Debug::Set_InitialPos()
{
	m_pTransformCom->Set_WorldMatrix(m_matInitial);
}

CCamera_Debug* CCamera_Debug::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Debug* pInstance = new CCamera_Debug(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Debug");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Debug::Clone(void* pArg, CGameObject* pOwner)
{
	CCamera_Debug* pInstance = new CCamera_Debug(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Debug");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CCamera_Debug::Free()
{
	__super::Free();
}
#ifdef _DEBUG

void CCamera_Debug::Describe_Entity()
{
	_float3 vPosition = {};
	GUI::Begin("PickingPos");
	if (m_pGameInstance->Key_Pressing(DIK_Y))
	{
		if (m_pGameInstance->isPicking(&vPosition))
		{
			GUI::InputFloat3("Pos", (float*)&vPosition);
		}
	}
	GUI::End();
	GUI::Begin("CAMERA");
	if (GUI::CollapsingHeader("DebugCamera")){
		m_pTransformCom->Describe_Entity();
	}
	GUI::End();
}

#endif // _DEBUG
