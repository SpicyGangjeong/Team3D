#include "pch.h"
#include "Dummy_PhysXEffectHitBox.h"

#include "GameInstance.h"


CDummy_PhysXEffectHitBox::CDummy_PhysXEffectHitBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDummy_PhysXEffectHitBox::CDummy_PhysXEffectHitBox(const CDummy_PhysXEffectHitBox& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummy_PhysXEffectHitBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummy_PhysXEffectHitBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

void CDummy_PhysXEffectHitBox::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->RewindMomentum();
}

void CDummy_PhysXEffectHitBox::Update(_float fTimeDelta)
{
	m_pTransformCom->AccumulateMomentum(XMLoadFloat3(&m_vDeltaPos));
	m_vLifeTime.x += fTimeDelta;
	if (m_vLifeTime.x > m_vLifeTime.y) {
		m_bDead = true;
	}
}

void CDummy_PhysXEffectHitBox::Late_Update(_float fTimeDelta)
{
	if (true == m_pCharacter_Controller->IsActive()) {
		m_pCharacter_Controller->Move(fTimeDelta);
	}
	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	}
}

HRESULT CDummy_PhysXEffectHitBox::Render()
{
	//if (FAILED(Bind_ShaderResources())) {
	//	return E_FAIL;
	//}

	//if (FAILED(m_pModelCom->Bind_Material(0, m_pShaderCom))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
	//	return E_FAIL;
	//}

	//if (FAILED(m_pModelCom->Render(0))) {
	//	return E_FAIL;
	//}
#ifdef _DEBUG
	if (FAILED(m_pCharacter_Controller->Render())) {
		return E_FAIL;
	}
#endif // _DEBUG


	return S_OK;
}

HRESULT CDummy_PhysXEffectHitBox::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}
	PHYSXDUMMY_DESC* pDesc = static_cast<PHYSXDUMMY_DESC*>(pArg);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	m_pTransformCom->Rotation(pDesc->vRotRPY.x, pDesc->vRotRPY.y, pDesc->vRotRPY.z);
	m_vDeltaPos = pDesc->vDeltaPos;
	m_vLifeTime = pDesc->vLifeTime;
	{ // CCT
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.iSubKind = pDesc->iSubKind;
		Desc.pTransform = m_pTransformCom;
		Desc.eBodyType = ACTOR::BOX;
		Desc.fContactOffset = 0.1f;
		Desc.fMaterial = { 0.5f, 0.5f, 0.6f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.05f };
		Desc.fRadius = 0.5f;
		Desc.fHeight = 1.0f;
		Desc.pCallback_HitReport = m_pCallBack_HitReport = CCallBack_EffectHitBox_HitReport::Create();
		Desc.pCallback_Behavior = m_pCallBack_Behavior = CCallBack_EffectHitBox_Behavior::Create();
		Desc.eClimbingMode = PSX::PxCapsuleClimbingMode::eEASY;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), (CComponent**)&m_pCharacter_Controller, &Desc))) {
			return E_FAIL;
		}
	}
	m_pCharacter_Controller->Set_Position(m_pTransformCom->Get_State(STATE::POSITION));
	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Box"), (CComponent**)&m_pModelCom))) {
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CDummy_PhysXEffectHitBox::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
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

CDummy_PhysXEffectHitBox* CDummy_PhysXEffectHitBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_PhysXEffectHitBox* pInstance = new CDummy_PhysXEffectHitBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummy_PhysXEffectHitBox");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_PhysXEffectHitBox::Clone(void* pArg, CGameObject* pOwner)
{
	CDummy_PhysXEffectHitBox* pInstance = new CDummy_PhysXEffectHitBox(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy_PhysXEffectHitBox");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummy_PhysXEffectHitBox::Free()
{
	if (nullptr == m_pGameInstance) {
		m_pCharacter_Controller->SetActive(false);
		PSX::PxExtendedVec3 pxControlllerPos = m_pCharacter_Controller->Get_Controller()->getPosition();
		PSX::PxTransform pxTransform((_float)pxControlllerPos.x, (_float)pxControlllerPos.y + 100.f, (_float)pxControlllerPos.z);
		m_pCharacter_Controller->Set_Position(XMLoadFloat3((_float3*)&pxTransform.p));
		m_pGameInstance->Detach_Actor(*m_pCharacter_Controller->Get_Actor());


	}
	__super::Free();
	if (nullptr != m_pCallBack_Behavior) {
		m_pCallBack_Behavior->Finalize();
	}
	if (nullptr != m_pCallBack_HitReport) {
		m_pCallBack_HitReport->Finalize();
	}
	SAFE_RELEASE(m_pCharacter_Controller);
	Safe_Delete(m_pCallBack_Behavior);
	Safe_Delete(m_pCallBack_HitReport);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
}

void CDummy_PhysXEffectHitBox::Describe_Entity()
{
}
