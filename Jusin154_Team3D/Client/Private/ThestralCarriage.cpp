#include "pch.h"
#include "ThestralCarriage.h"

#include "GameInstance.h"
#include "InfoInstance.h"

CThestralCarriage::CThestralCarriage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CThestralCarriage::CThestralCarriage(const CThestralCarriage& Prototype)
	: CGameObject(Prototype),
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

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-21.f, -5.759f, -14.f, 1.f));

	//vector<pair<_uint, _bool>> chain =
	//{
	//	{ 0, false },
	//	{ 1, false },
	//	{ 2, false }
	//};

	//for (auto& anim : chain)
	//	m_pModelCom->Set_ChainAnimation(anim);
	m_pModelCom->Set_AnimationIndex(0);
	m_pModelCom->IsRootBone(false);
	//
	//m_pModelCom->Start_ChainAnimation();

	return S_OK;
}

void CThestralCarriage::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CThestralCarriage::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pGameInstance->Key_Up(DIK_U))
	{
		m_pModelCom->Set_AnimationIndex(1);
		m_pModelCom->IsRootBone(false);
	}

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

	//m_pModelCom->Update_ChainAnimation();

	_float4 RootMomentum = m_pModelCom->Get_RootBoneMomentum();
	_vector RootDelta = XMLoadFloat4(&RootMomentum);

	_vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);
	_vector vUp = m_pTransformCom->Get_State(STATE::UP);
	_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);


	_float dx = XMVectorGetX(RootDelta);
	_float dy = XMVectorGetY(RootDelta);
	_float dz = XMVectorGetZ(RootDelta);

	_vector vWorldDelta =
		vRight * dx +
		vUp * dy +
		vLook * dz;

	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	m_pTransformCom->Set_State(STATE::POSITION, vPos + vWorldDelta);


#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

}

void CThestralCarriage::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

}

HRESULT CThestralCarriage::Render()
{
	if (!m_pModelCom) {
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
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

		m_pModelCom->Bind_OutPut_SRV_VS(26, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
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

	SAFE_RELEASE(m_pModelCom);
	SAFE_RELEASE(m_pShaderCom);
}
#ifdef _DEBUG

void CThestralCarriage::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	if (GUI::CollapsingHeader("Carriage")) {
		string AnimList = m_pModelCom->Get_AnimList(m_pModelCom->Get_AnimIndex());
		GUI::Text(AnimList.c_str());
		GUI::Text("AnimIndex %d", m_pModelCom->Get_AnimIndex());

		GUI::Text("AnimTrack %.2f", m_pModelCom->Get_CurrentTrackPosition());
		GUI::Text("AnimRatio %.2f", m_pModelCom->Get_CurrentTrackProgressRatio());
		_float3 Pos;
		XMStoreFloat3(&Pos, Get_WorldPostion());

		float Pos3[3] = { Pos.x, Pos.y, Pos.z };
		GUI::DragFloat3("Pos", Pos3);


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
