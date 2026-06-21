#include "pch.h"
#include "Goblin_Dagger.h"

#include "GameInstance.h"
#include "Goblin.h"

CGoblin_Dagger::CGoblin_Dagger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CGoblin_Dagger::CGoblin_Dagger(const CGoblin_Dagger& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CGoblin_Dagger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGoblin_Dagger::Initialize(void* pArg)
{
	GOBLINDAGGER_DESC* pDesc = static_cast<GOBLINDAGGER_DESC*> (pArg);
	m_pSocketMatrices = pDesc->pSocketMatrices;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	return S_OK;
}

void CGoblin_Dagger::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->ResetVelocityVector();
	XMStoreFloat4(&m_vStartPos, m_pTransformCom->Get_State(STATE::POSITION));
	m_pModelCom->Combined_BoneMatrix();
	if (m_bAttach)
	{
		_matrix socketMatrix = {};

		socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

		for (int i = 0; i < 3; ++i) {
			socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
		}

		m_pTransformCom->Set_WorldMatrix(socketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
		_float4 vTargetPos = static_cast<CGoblin*>(m_pOwner)->Get_TargetPos();
		m_pTransformCom->LookAt(XMLoadFloat4(&vTargetPos));
	}
	else
	{
		_matrix world = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
		m_pTransformCom->Set_WorldMatrix(world);
		m_pTransformCom->Go_Straight(fTimeDelta);
	}


#ifdef _DEBUG
	Describe_Entity();

#endif // _DEBUG
}

void CGoblin_Dagger::Update(_float fTimeDelta)
{
}

void CGoblin_Dagger::Late_Update(_float fTimeDelta)
{
	Dagger_Hit();
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CGoblin_Dagger::Render()
{
	if (!m_pModelCom)
		return S_OK;

	if (!m_bVisible)
		return S_OK;

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	_float Intensity = 1.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMBIntensity", &Intensity, sizeof(_float)))) {
		return E_FAIL;
	}
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}
	Intensity = 0.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMBIntensity", &Intensity, sizeof(_float)))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CGoblin_Dagger::Ready_Components()
{

	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 25.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);


	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Goblin_Dagger_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoblin_Dagger::Bind_ShaderResources()
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

void CGoblin_Dagger::Dagger_Hit()
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
				
				PHYSX_USERDATA* pUserData = static_cast<PHYSX_USERDATA*>(pActor->userData);
				tagCollInfo.pObject = pUserData->pOwner;
				tagCollInfo.eHitType = ENUM_CLASS(HIT_TYPE::HIT_PROJECTILE);
				tagCollInfo.fDamage = 10.f;
				
				switch (pUserData->eKind)
				{
				case PHYSX_KIND::BODY_DYNAMIC:
					switch (PXOBJECT(pUserData->iSubKind))
					{
					case PXOBJECT::SKILL_PROTEGO:
					{
						if (false == m_bVisible) {
							break;
						}
						m_bVisible = false;
						pUserData->pOwner->OnCollision(this, &tagCollInfo);
					}
					break;
					}
					break;
				case PHYSX_KIND::CCTActor:
				{
					switch (PXOBJECT(pUserData->iSubKind))
					{
					case Engine::PXOBJECT::PLAYER:
					{
						if (false == m_bVisible) {
							break;
						}
						m_bVisible = false;
						pUserData->pOwner->OnCollision(this, &tagCollInfo);
					}
					break;

					}
				}
				}
				
				
			}
		}
	}
}


CGoblin_Dagger* CGoblin_Dagger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin_Dagger* pInstance = new CGoblin_Dagger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGoblin_Dagger");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CGoblin_Dagger::Clone(void* pArg, CGameObject* pOwner)
{
	CGoblin_Dagger* pInstance = new CGoblin_Dagger(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGoblin_Dagger");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CGoblin_Dagger::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);

}
#ifdef _DEBUG

void CGoblin_Dagger::Describe_Entity()
{
}

#endif // _DEBUG
