#include "pch.h"
#include "ThestralCarriage.h"

#include "GameInstance.h"
#include "InfoInstance.h"

CThestralCarriage::CThestralCarriage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnit(pDevice, pContext)
{
}

CThestralCarriage::CThestralCarriage(const CThestralCarriage& Prototype)
	: CUnit(Prototype),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CThestralCarriage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CThestralCarriage::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

#ifdef _DEBUG

	D3D11_RASTERIZER_DESC RSS_Desc = {};
	RSS_Desc.FillMode = D3D11_FILL_WIREFRAME;
	RSS_Desc.CullMode = D3D11_CULL_BACK;
	RSS_Desc.FrontCounterClockwise = FALSE;
	RSS_Desc.DepthClipEnable = TRUE;
	RSS_Desc.AntialiasedLineEnable = TRUE;

	m_pDevice->CreateRasterizerState(&RSS_Desc, &m_pRSS);


#endif // _DEBUG


	m_iAnimationIndex = 2;
	m_pModelCom->Set_AnimationIndex(m_iAnimationIndex);
	//vector<pair<_uint, _bool>> chain =
	//{
	//	{ 0, false },
	//	{ 1, false },
	//	{ 2, false }
	//};

	//for (auto& anim : chain)
	//	m_pModelCom->Set_ChainAnimation(anim);
	m_pModelCom->IsRootBone(false);
	//
	//m_pModelCom->Start_ChainAnimation();

	return S_OK;
}

void CThestralCarriage::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	Lerp_Transform(fTimeDelta);
}

void CThestralCarriage::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);
	
	m_pTransformCom->Go_Straight(fTimeDelta * m_iStraight);
	
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

}

void CThestralCarriage::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CThestralCarriage::Render()
{
	if (!m_pModelCom) {
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
#ifdef _DEBUG
	ID3D11RasterizerState* pOriginalRSS = { nullptr };
	if (m_bRender_WireFrame) {
		m_pContext->RSGetState(&pOriginalRSS);
	}
#endif // _DEBUG

	RENDER ePass = m_pGameInstance->Get_CurrentRenderPass();
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	if (RENDER::NONBLEND == ePass) {
		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (i == ENUM_CLASS(CARRIAGE_MESH_ORDER::WINDOWS)) {
				continue;
			}
			if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_Matrices(
				"g_OffsetMatrix",
				m_pModelCom->Get_OffsetMatrix(i).data(),
				(_int)m_pModelCom->Get_OffsetMatrix(i).size()
			)))
			{
				return E_FAIL;
			}
			if (FAILED(m_pModelCom->Begin(i, m_pShaderCom))) {
				return E_FAIL;
			}
#ifdef _DEBUG
			if (m_bRender_WireFrame) {
				m_pContext->RSSetState(m_pRSS);
			}
#endif // _DEBUG
			m_pModelCom->Bind_OutPut_SRV_VS(26, 0);

			if (FAILED(m_pModelCom->Render(i))) {
				return E_FAIL;
			}
		}
	}
	else if (RENDER::BLEND == ePass) {
		if (FAILED(m_pModelCom->Bind_Material(1, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_Matrices(
			"g_OffsetMatrix",
			m_pModelCom->Get_OffsetMatrix(1).data(),
			(_int)m_pModelCom->Get_OffsetMatrix(1).size()
		)))
		{
			return E_FAIL;
		}
		if (FAILED(m_pModelCom->Begin(1, m_pShaderCom))) {
			return E_FAIL;
		}
#ifdef _DEBUG
		if (m_bRender_WireFrame) {
			m_pContext->RSSetState(m_pRSS);
		}
#endif // _DEBUG
		m_pModelCom->Bind_OutPut_SRV_VS(26, 0);

		if (FAILED(m_pModelCom->Render(1))) {
			return E_FAIL;
		}
	}
#ifdef _DEBUG
	if (m_bRender_WireFrame) {
		m_pContext->RSSetState(pOriginalRSS);
		SAFE_RELEASE(pOriginalRSS);
	}
#endif // _DEBUG


	return S_OK;
}

void CThestralCarriage::Trigger(CTimeSocket& Socket)
{
	SOCKETCONTENTS* pContents = &Socket.m_Contents;
	switch (pContents->eTypeFunc)
	{
	case TIMESOCKET_FUNC::TELEPORTATION:
	{
		m_pTransformCom->Set_WorldMatrix(pContents->pxTransform);
	} break;
	case TIMESOCKET_FUNC::TRANSLATION:
	{
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3((_float3*)&pContents->pxTransform.p), 1.f));
	} break;
	case TIMESOCKET_FUNC::AFFINE_LERP:
	{
		Lerp_Start(pContents->pxTransform, pContents->vParam_11.x);
	} break;
	case TIMESOCKET_FUNC::SET_ANIMSTATE:
	{
		if (pContents->vFlags.b[0]) {
			m_iStraight = 1;
		}
		else if (pContents->vFlags.b[1]) {
			m_iStraight = 3;
		}
		else {
			m_iStraight = 0;
		}
		
	} break;
	case TIMESOCKET_FUNC::SET_FSMSTATE:
	{
			if (pContents->vFlags.b[0]) {
				m_iAnimationIndex = 0;
				m_pModelCom->Set_AnimationIndex(m_iAnimationIndex);
				m_pModelCom->IsRootBone(false);
			}
			else if (pContents->vFlags.b[1]) {
				m_iAnimationIndex = 1;
				m_pModelCom->Set_AnimationIndex(m_iAnimationIndex);
				m_pModelCom->IsRootBone(false);
			}
			else if (pContents->vFlags.b[2]) {
				m_iAnimationIndex = 2;
				m_pModelCom->Set_AnimationIndex(m_iAnimationIndex);
				m_pModelCom->IsRootBone(false);
			}
	} break;
	case TIMESOCKET_FUNC::BIND_SOCKET_MATRIX:
	{

	} break;
	case TIMESOCKET_FUNC::UNBIND_SOCKET_MATRIX:
	{

	} break;
	default:
		break;
	}
}

_vector CThestralCarriage::Get_WorldPostion()
{
	return (XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("seatSocket_front_left")) * m_pTransformCom->Get_XMWorldMatrix()).r[3];
}

_matrix CThestralCarriage::Get_SeatMatrix()
{
	return XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("seatSocket_front_left")) * m_pTransformCom->Get_XMWorldMatrix();
}

_matrix CThestralCarriage::Get_SocketWorldMatrix(CARRIAGE_SOCKET eSocket)
{
	switch (eSocket)
	{
	case CThestralCarriage::CARRIAGE_SOCKET::BACK:
		return XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("backSeat")) * m_pTransformCom->Get_XMWorldMatrix();
		break;
	case CThestralCarriage::CARRIAGE_SOCKET::BACK_LEFT:
		return XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("seatSocket_back_left")) * m_pTransformCom->Get_XMWorldMatrix();
		break;
	case CThestralCarriage::CARRIAGE_SOCKET::BACK_RIGHT:
		return XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("seatSocket_back_right")) * m_pTransformCom->Get_XMWorldMatrix();
		break;
	case CThestralCarriage::CARRIAGE_SOCKET::FRONT:
		return XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("frontSeat")) * m_pTransformCom->Get_XMWorldMatrix();
		break;
	case CThestralCarriage::CARRIAGE_SOCKET::FRONT_LEFT:
		return XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("seatSocket_front_left")) * m_pTransformCom->Get_XMWorldMatrix();
		break;
	case CThestralCarriage::CARRIAGE_SOCKET::FRONT_RIGHT:
		return XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("seatSocket_front_right")) * m_pTransformCom->Get_XMWorldMatrix();
		break;
	default:
		return XMMatrixIdentity();
		break;
	}
}

void CThestralCarriage::Lerp_Stop()
{
	m_LerpStartMatrix = { };
	m_LerpEndMatrix = { };
	m_vLerpTimer = { };
	m_bLerp = { false };
}

void CThestralCarriage::Lerp_Start(PSX::PxTransform pxTransform, _float fLerpTimer)
{
	XMStoreFloat4x4(&m_LerpStartMatrix, m_pTransformCom->Get_XMWorldMatrix());
	_float3 vScale = m_pTransformCom->Get_Scale();
	XMStoreFloat4x4(&m_LerpEndMatrix, XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorZero(),
		XMLoadFloat4((_float4*)&pxTransform.q), XMVectorSetW(XMLoadFloat3((_float3*)&pxTransform.p), 1.f)));
	m_vLerpTimer.x = 0;
	m_vLerpTimer.y = fLerpTimer;
	m_bLerp = true;
}

void CThestralCarriage::Lerp_Transform(_float fTimeDelta)
{
	if (true == m_bLerp) {
		m_vLerpTimer.x += fTimeDelta;
		_float4x4 vLerpResult = {};
		if (m_vLerpTimer.x >= m_vLerpTimer.y) {
			m_bLerp = false;
			m_vLerpTimer.x = 0.f;
			CMyTools::MatrixLerp(XMLoadFloat4x4(&m_LerpStartMatrix), XMLoadFloat4x4(&m_LerpEndMatrix), vLerpResult, 1.f);
		}
		else {
			CMyTools::MatrixLerp(XMLoadFloat4x4(&m_LerpStartMatrix), XMLoadFloat4x4(&m_LerpEndMatrix), vLerpResult, m_vLerpTimer.x / m_vLerpTimer.y);
		}
		m_pTransformCom->Set_WorldMatrix(vLerpResult);
	}
}

HRESULT CThestralCarriage::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_ThestralCarriage_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_NPC_PBR_ANIM,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}

	m_pModelCom->Set_DisableRootMotionScale(true);


	return S_OK;
}

HRESULT CThestralCarriage::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", m_pTransformCom->Get_PrevWorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevViewMatrix", D3DTS::VIEW))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevProjMatrix", D3DTS::PROJ))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}

	return S_OK;
}

CThestralCarriage* CThestralCarriage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CThestralCarriage* pInstance = new CThestralCarriage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CThestralCarriage");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CThestralCarriage::Clone(void* pArg, CGameObject* pOwner)
{
	CThestralCarriage* pInstance = new CThestralCarriage(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CThestralCarriage");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CThestralCarriage::Free()
{
	__super::Free();

#ifdef _DEBUG
	SAFE_RELEASE(m_pRSS);
#endif // _DEBUG
	SAFE_RELEASE(m_pModelCom);
	SAFE_RELEASE(m_pShaderCom);
}
#ifdef _DEBUG

void CThestralCarriage::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	if (GUI::CollapsingHeader("Carriage")) {
		m_pTransformCom->Describe_Entity();
		m_pShaderCom->Describe_Entity();
		string AnimList = m_pModelCom->Get_AnimList(m_pModelCom->Get_AnimIndex());
		GUI::Text(AnimList.c_str());
		GUI::Text("AnimIndex %d", m_pModelCom->Get_AnimIndex());

		GUI::Text("AnimTrack %.2f", m_pModelCom->Get_CurrentTrackPosition());
		GUI::Text("AnimRatio %.2f", m_pModelCom->Get_CurrentTrackProgressRatio());
		_float3 Pos;
		XMStoreFloat3(&Pos, m_pTransformCom->Get_State(STATE::POSITION));

		float Pos3[3] = { Pos.x, Pos.y, Pos.z };
		GUI::DragFloat3("Pos", Pos3);

		GUI::Checkbox("WireFrame", &m_bRender_WireFrame);

		_float RotR, RotU, RotL;
		RotR = XMVectorGetX(m_pTransformCom->Get_State(STATE::RIGHT));
		RotU = XMVectorGetY(m_pTransformCom->Get_State(STATE::UP));
		RotL = XMVectorGetZ(m_pTransformCom->Get_State(STATE::LOOK));

		float Rot3[3] = { RotR, RotU,RotL };
		GUI::DragFloat3("Rot", Rot3);

		if (GUI::Button("MovePos"))
		{
			m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_pGameInstance->Get_CamPosition()->x, m_pGameInstance->Get_CamPosition()->y, m_pGameInstance->Get_CamPosition()->z, 1.f));
		}
		m_pModelCom->Describe_Entity();
	}
	GUI::End();
}

#endif // _DEBUG
