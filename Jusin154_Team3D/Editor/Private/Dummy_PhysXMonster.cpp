#include "pch.h"
#include "Dummy_PhysXMonster.h"
#include "CallBack_Monster_Behavior.h"
#include "CallBack_Monster_HitReport.h"
#include "GameInstance.h"

CDummy_PhysXMonster::CDummy_PhysXMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CGameObject(pDevice, pContext)
{
}

CDummy_PhysXMonster::CDummy_PhysXMonster(const CDummy_PhysXMonster& rhs) :
	CGameObject(rhs)
{
}

void CDummy_PhysXMonster::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->RewindMomentum();
	m_pTransformCom->Set_State(STATE::POSITION, m_pCharacter_Controller->Get_Position());
}

void CDummy_PhysXMonster::Update(_float fTimeDelta)
{
	GUI::Text("%d", m_pCharacter_Controller->IsActive());
	GUI::Text("%f %f", m_vStunTimer.x, m_vStunTimer.y);
	if (true == m_pCharacter_Controller->IsActive()) {
		if (m_pGameInstance->Key_Pressing(DIK_UPARROW)) {
			m_pTransformCom->AccumulateMomentum(XMVectorSet(0.f, 0.f, 1.f, 0.f) * m_pTransformCom->Get_Speed() * fTimeDelta);
		}
		if (m_pGameInstance->Key_Pressing(DIK_LEFT)) {
			m_pTransformCom->AccumulateMomentum(XMVectorSet(-1.f, 0.f, 0.f, 0.f) * m_pTransformCom->Get_Speed() * fTimeDelta);
		}
		if (m_pGameInstance->Key_Pressing(DIK_DOWN)) {
			m_pTransformCom->AccumulateMomentum(XMVectorSet(0.f, 0.f, -1.f, 0.f) * m_pTransformCom->Get_Speed() * fTimeDelta);
		}
		if (m_pGameInstance->Key_Pressing(DIK_RIGHT)) {
			m_pTransformCom->AccumulateMomentum(XMVectorSet(1.f, 0.f, 0.f, 0.f) * m_pTransformCom->Get_Speed() * fTimeDelta);
		}
		m_pCharacter_Controller->Move(fTimeDelta);
		m_vStunTimer.x = 0.f;
	}
	else {
		if (0.f == m_vStunTimer.x) {
			PSX::PxExtendedVec3 pxControlllerPos = m_pCharacter_Controller->Get_Controller()->getPosition();
			PSX::PxTransform pxTransform((_float)pxControlllerPos.x, (_float)pxControlllerPos.y + 100.f, (_float)pxControlllerPos.z);
			m_pCharacter_Controller->Set_Position(XMLoadFloat3((_float3*)&pxTransform.p));
			//m_pCharacter_Controller->Move(fTimeDelta);
		}
		m_vStunTimer.x += fTimeDelta;
		m_pTransformCom->Set_WorldMatrix(m_pRigidBody->Get_Actor()->getGlobalPose());
		if (m_vStunTimer.y < m_vStunTimer.x) {
			m_pRigidBody->ConvertToCCT(*m_pCharacter_Controller);
		}
	}
}

void CDummy_PhysXMonster::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	}
}

HRESULT CDummy_PhysXMonster::Render()
{
#ifdef _DEBUG
	if (true == m_pCharacter_Controller->IsActive()) {
		if (FAILED(m_pCharacter_Controller->Render())) {
			return E_FAIL;
		}
	}
	else {
		if (FAILED(m_pRigidBody->Render())) {
			return E_FAIL;
		}
	}
#endif // _DEBUG
	return S_OK;
}

HRESULT CDummy_PhysXMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummy_PhysXMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}
	m_pCallBack_Behavior->Initialize(m_pCharacter_Controller, m_pRigidBody);
	m_pCallBack_HitReport->Initialize(m_pCharacter_Controller, m_pRigidBody);

	return S_OK;
}

HRESULT CDummy_PhysXMonster::Ready_Components(void* pArg)
{
	CTransform::TRANSFORM_DESC TransformDesc{};
	TransformDesc.fRadius = 100.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(60.f);
	TransformDesc.fSpeedPerSec = 30.f;
	if (FAILED(__super::Ready_Components(&TransformDesc))) {
		return E_FAIL;
	}
	PHYSXDUMMY_DESC* pDesc = static_cast<PHYSXDUMMY_DESC*>(pArg);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	m_pTransformCom->Rotation(pDesc->vRotRPY.x, pDesc->vRotRPY.y, pDesc->vRotRPY.z);
	{ // CCT
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.iSubKind = pDesc->iSubKind;
		Desc.pTransform = m_pTransformCom;
		Desc.eBodyType = ACTOR::CAPSULE;
		Desc.fContactOffset = 0.1f;
		Desc.fMaterial = { 0.5f, 0.5f, 0.6f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.05f };
		Desc.fRadius = 0.5f;
		Desc.fHeight = 1.0f;
		Desc.pCallback_HitReport = m_pCallBack_HitReport = CCallBack_Monster_HitReport::Create();
		Desc.pCallback_Behavior = m_pCallBack_Behavior = CCallBack_Monster_Behavior::Create();
		Desc.eClimbingMode = PSX::PxCapsuleClimbingMode::eEASY;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), (CComponent**)&m_pCharacter_Controller, &Desc))) {
			return E_FAIL;
		}
	}
	m_pCharacter_Controller->Set_Position(m_pTransformCom->Get_State(STATE::POSITION));
	{ // DO
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = pDesc->iSubKind;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
		m_pGameInstance->Detach_Actor(*m_pRigidBody->Get_Actor());
	}

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))){
		return E_FAIL;
	}

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Box"), (CComponent**)&m_pModelCom))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDummy_PhysXMonster::Bind_ShaderResources()
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

CDummy_PhysXMonster* CDummy_PhysXMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_PhysXMonster* pInstance = new CDummy_PhysXMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummy_PhysXMonster");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_PhysXMonster::Clone(void* pArg, CGameObject* pOwner)
{
	CDummy_PhysXMonster* pInstance = new CDummy_PhysXMonster(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy_PhysXMonster");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummy_PhysXMonster::Free()
{
	__super::Free();

	if (nullptr != m_pCallBack_Behavior) {
		m_pCallBack_Behavior->Finalize();
		Safe_Delete(m_pCallBack_Behavior);
	}
	if (nullptr != m_pCallBack_HitReport) {
		m_pCallBack_HitReport->Finalize();
		Safe_Delete(m_pCallBack_HitReport);
	}
	SAFE_RELEASE(m_pCharacter_Controller);
	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
}

void CDummy_PhysXMonster::Describe_Entity()
{
	m_pCharacter_Controller->Describe_Entity();
}
