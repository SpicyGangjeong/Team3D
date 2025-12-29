#include "pch.h"
#include "Goblin_Sword.h"

#include "GameInstance.h"
#include "Goblin.h"

CGoblin_Sword::CGoblin_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CGoblin_Sword::CGoblin_Sword(const CGoblin_Sword& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CGoblin_Sword::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGoblin_Sword::Initialize(void* pArg)
{
	GOBLIN_SWORD_DESC* pDesc = static_cast<GOBLIN_SWORD_DESC*> (pArg);
	m_pSocketMatrices = pDesc->pSocketMatrices;
	m_iIndex = pDesc->iIndex;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	return S_OK;
}

void CGoblin_Sword::Priority_Update(_float fTimeDelta)
{



#ifdef _DEBUG
	Describe_Entity();

#endif // _DEBUG
}

void CGoblin_Sword::Update(_float fTimeDelta)
{
	if (m_bDisolve) {
		m_fDisolveTime += fTimeDelta;
		if (m_fDisolveTime >= 1.f)
		{
			m_fDisolveTime = 0.f;
			m_bDisolve = false;
			m_bVisible = false;
		}
	}
}

void CGoblin_Sword::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4(&m_vStartPos, m_pTransformCom->Get_State(STATE::POSITION));

	_matrix socketMatrix = {};

	socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

	for (int i = 0; i < 3; ++i) {
		socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
	}

	m_pTransformCom->Set_WorldMatrix(socketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
	XMStoreFloat4x4(&m_vSwordMat, XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Bone")) * m_pTransformCom->Get_XMWorldMatrix());
	if (m_bCanTakeDamage)
	{
		Sword_Hit();
	}
	else {
		m_bHit = false;
	}
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CGoblin_Sword::Render()
{
	if (!m_pModelCom)
		return S_OK;

	if (!m_bVisible)
		return S_OK;

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	if (FAILED(Render_Disolve())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::SPECTOR_WEAPON)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	{
		_bool bDisolve = false;
		_float zero = 0.f;
		m_pShaderCom->Bind_RawValue("g_bDisolve", &bDisolve, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_fDisolveRatio", &zero, sizeof(_float));
	}

	return S_OK;
}

HRESULT CGoblin_Sword::Render_Disolve()
{
	if (FLT_EPSILON3 * 10 < m_fDisolveTime)
	{
		_bool bDisolve = true;
		_float fDisolveAmount = 0.1f;
		_float fDisolveEdgeWidth = 0.1f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bDisolve", &bDisolve, sizeof(_bool)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveRatio", &m_fDisolveTime, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveAmount", &fDisolveAmount, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveEdgeWidth", &fDisolveEdgeWidth, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Bind_GlobalSRV(m_pShaderCom, TEXT("GLOBAL_DISOLVE_NOISE_05"), "g_DeadDisolveTexture"))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Bind_GlobalSRV(m_pShaderCom, TEXT("GLOBAL_DISOLVE_BURN_VERTICAL"), "g_DisolveTexture"))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CGoblin_Sword::Ready_Components()
{

	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);


	if (m_iIndex == 0)
	{
		m_strModelPrototypeTag = TEXT("Prototype_Component_Goblin_Sword_L_Model");
	}
	else {
		m_strModelPrototypeTag = TEXT("Prototype_Component_Goblin_Sword_R_Model");
	}

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoblin_Sword::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
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

void CGoblin_Sword::Sword_Hit()
{
	_vector vStartPos = XMLoadFloat4(&m_vStartPos);
	_vector vEndPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vDir = vEndPos - vStartPos;
	_float fLength = XMVectorGetX(XMVector4Length(vDir));
	vDir = XMVector4Normalize(vDir);
	m_SweepBuffer = {};
	_bool bHit = m_pGameInstance->SphereCast(0.5f, vStartPos, vDir, fLength, PSX::PxHitFlag::eDEFAULT, PSX::PxQueryFlag::eDYNAMIC, m_SweepBuffer);
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
				ON_COLLISION_INFO tagCollInfo = {};

				tagCollInfo.vWorldPos.w = 1.f;

				memcpy_s(&tagCollInfo.vWorldPos, sizeof(tagCollInfo.vWorldPos), &hit.position, sizeof(hit.position));

				memcpy_s(&tagCollInfo.vWorldNomal, sizeof(tagCollInfo.vWorldNomal), &hit.normal, sizeof(hit.normal));
				XMStoreFloat4(&tagCollInfo.vHitDir, vDir);
				tagCollInfo.fLength = fLength;
				tagCollInfo.fDamage = 10.f;
				tagCollInfo.eHitType = ENUM_CLASS(HIT_TYPE::HIT_MEDIUM);

				PHYSX_USERDATA* pUserData = static_cast<PHYSX_USERDATA*>(pActor->userData);
				tagCollInfo.pObject = pUserData->pOwner;
				switch (PXOBJECT(pUserData->iSubKind))
				{
				case Engine::PXOBJECT::PLAYER:
				{
					pUserData->pOwner->OnCollision(this, &tagCollInfo);
					m_bHit = true;
					m_bCanTakeDamage = false;
				}
				break;
				case Engine::PXOBJECT::ALLY_HITBOX:
					break;
				default:
					break;
				}


			}
		}
	}
}


CGoblin_Sword* CGoblin_Sword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin_Sword* pInstance = new CGoblin_Sword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGoblin_Sword");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CGoblin_Sword::Clone(void* pArg, CGameObject* pOwner)
{
	CGoblin_Sword* pInstance = new CGoblin_Sword(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGoblin_Sword");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CGoblin_Sword::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);

}
#ifdef _DEBUG

void CGoblin_Sword::Describe_Entity()
{
}

#endif // _DEBUG
