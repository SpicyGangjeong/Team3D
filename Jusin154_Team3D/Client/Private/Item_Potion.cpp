#include "pch.h"
#include "Item_Potion.h"

#include "GameInstance.h"
#include "EffectPool.h"
#include "Layer.h"
CItem_Potion::CItem_Potion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CItem_Potion::CItem_Potion(const CItem_Potion& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CItem_Potion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem_Potion::Initialize(void* pArg)
{
	POTION_DESC* pDesc = static_cast<POTION_DESC*> (pArg);
	m_pSocketMatrices = pDesc->pSocketMatrices;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}


	m_pEffectPool = m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_EffectPool"))->Get_Object<CEffectPool>();
	SAFE_ADDREF(m_pEffectPool);

	return S_OK;
}

void CItem_Potion::Priority_Update(_float fTimeDelta)
{
	XMStoreFloat4(&m_vStartPos, m_pTransformCom->Get_State(STATE::POSITION));
	
#ifdef _DEBUG
	Describe_Entity();

#endif // _DEBUG
}

void CItem_Potion::Update(_float fTimeDelta)
{
	m_pModelCom->Combined_BoneMatrix();

}

void CItem_Potion::Late_Update(_float fTimeDelta)
{
	if (m_bVisible)
	{
		m_fTimer += fTimeDelta;
		if (m_fTimer >= 1.f && m_bOnce == false) {
			m_pEffectPool->Use_Skill(SKILL_TYPE::SCREEN_POTION , this);
			m_bOnce = true;
		}
		if (m_fTimer >= 1.6f) {
			m_bAttach = false;
		}

		if (m_fTimer >= 3.f) {
			m_fTimer = 0.f;
			m_bVisible = false;
			m_bAttach = true;
			m_bOnce = false;
		}
	}

	if (m_bAttach)
	{
		m_bHit = false;
		_matrix socketMatrix = {};

		socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

		for (int i = 0; i < 3; ++i) {
			socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
		}

		m_pTransformCom->Set_WorldMatrix(socketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
	}
	else
	{
		m_pGameInstance->Attach_Actor(*m_pRigidBody->Get_Actor(), NEXT_LEVEL);
		_matrix world = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
		m_pTransformCom->Set_WorldMatrix(world);

		m_pTransformCom->Go_Right(fTimeDelta);

	}
	if (false == m_bHit) {
		Check_GroundCollision();
	}


	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}
	

HRESULT CItem_Potion::Render()
{
	if (!m_pModelCom)
		return S_OK;

	if (!m_bVisible)
		return S_OK;

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

#ifdef _DEBUG
	m_pRigidBody->Render();
#endif // _DEBUG


	return S_OK;
}

void CItem_Potion::Check_GroundCollision()
{
	_vector vStartPos = XMLoadFloat4(&m_vStartPos);
	_vector vEndPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vDir = vEndPos - vStartPos;
	_float fLength = XMVectorGetX(XMVector4Length(vDir));
	vDir = XMVector4Normalize(vDir);
	m_SweepBuffer = {};
	_bool bHit = m_pGameInstance->SphereCast(0.02f, vStartPos, vDir, fLength, PSX::PxHitFlag::eDEFAULT, PSX::PxQueryFlag::eSTATIC, m_SweepBuffer);
	if (true == bHit) {
		vector<PSX::PxSweepHit*> Hits;
		_uint iHitCount = m_SweepBuffer.nbTouches;
		if (true == m_SweepBuffer.hasBlock) {
			iHitCount += 1;
			Hits.reserve(iHitCount);
			Hits.emplace_back(&m_SweepBuffer.block);
		}
		else {
			Hits.reserve(iHitCount);
		}
		for (_uint i = 0; i < m_SweepBuffer.nbTouches; ++i) {
			Hits.emplace_back(&m_SweepBuffer.touches[i]);
		}

		for (_uint i = 0; i < Hits.size(); ++i) {
			PSX::PxSweepHit* pHit = Hits[i];
			PSX::PxActor* pActor = pHit->actor;
			if (nullptr != pActor && nullptr != pActor->userData) {
				const PSX::PxSweepHit& hit = m_SweepBuffer.block;
				PSX::PxShape* pShape = hit.shape;

				PHYSX_USERDATA* pUserData = static_cast<PHYSX_USERDATA*>(pActor->userData);


				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::TERRAIN:
				{
					OnCollision(this);
					m_bHit = true;
					break;
				}
				}
			}
		}
	}
}


void CItem_Potion::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bVisible == false)
		return;

	m_bHit = false;
	m_pEffectPool->Use_Skill(SKILL_TYPE::POTION_BROKEN, this);
	m_fTimer = 0.f;
	m_bVisible = false;
	m_bAttach = true;
	m_bOnce = false;
}

void CItem_Potion::Set_Attach(_bool Attach)
{
	m_bAttach = Attach;

}


HRESULT CItem_Potion::Ready_Components()
{

	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);


	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Potion_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	{ // DO
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::POTION);
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_POTION"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}

		m_pGameInstance->Detach_Actor(*m_pRigidBody->Get_Actor(), NEXT_LEVEL);
	}

	return S_OK;
}

HRESULT CItem_Potion::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
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


CItem_Potion* CItem_Potion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Potion* pInstance = new CItem_Potion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem_Potion");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CItem_Potion::Clone(void* pArg, CGameObject* pOwner)
{
	CItem_Potion* pInstance = new CItem_Potion(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem_Potion");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CItem_Potion::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pEffectPool);

}
#ifdef _DEBUG

void CItem_Potion::Describe_Entity()
{

}

#endif // _DEBUG
