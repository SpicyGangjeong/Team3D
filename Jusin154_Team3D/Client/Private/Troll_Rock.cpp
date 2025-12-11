#include "pch.h"
#include "Troll_Rock.h"

#include "GameInstance.h"
#include "Troll.h"

CTroll_Rock::CTroll_Rock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CTroll_Rock::CTroll_Rock(const CTroll_Rock& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CTroll_Rock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTroll_Rock::Initialize(void* pArg)
{
	TROLLROCK_DESC* pDesc = static_cast<TROLLROCK_DESC*> (pArg);
	m_pSocketMatrices = pDesc->pSocketMatrices;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	return S_OK;
}

void CTroll_Rock::Priority_Update(_float fTimeDelta)
{
	m_pModelCom->Combined_BoneMatrix();
	if (m_bAttach)
	{
		_matrix socketMatrix = {};

		socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

		for (int i = 0; i < 3; ++i) {
			socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
		}

		m_pTransformCom->Set_WorldMatrix(socketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
		_float4 vTargetPos = static_cast<CTroll*>(m_pOwner)->Get_TargetPos();
		m_pTransformCom->LookAt(XMLoadFloat4(&vTargetPos));
	}
	else
	{
		_matrix world = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
		m_pTransformCom->Set_WorldMatrix(world);

		m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::LOOK), fTimeDelta * 50.f);
		m_pTransformCom->Go_Straight(fTimeDelta);

	}
	XMStoreFloat4(&m_vStartPos, m_pTransformCom->Get_State(STATE::POSITION));


#ifdef _DEBUG
	Describe_Entity();

#endif // _DEBUG
}

void CTroll_Rock::Update(_float fTimeDelta)
{
}

void CTroll_Rock::Late_Update(_float fTimeDelta)
{
	if (true == m_bVisible) {
		RockHit();
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

void CTroll_Rock::RockHit()
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
				PhsXUserData* pUserData = static_cast<PhsXUserData*>(pActor->userData);
				switch (PXOBJECT(pUserData->iSubKind))
				{
				case Engine::PXOBJECT::PLAYER:
				{
					if (false == m_bVisible) {
						break;
					}
					m_bVisible = false;
					CStat* pStat = pUserData->pCharacter->Get_Owner()->Get_Component<CStat>();
					pStat->Get_Damage(10.f);
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

void CTroll_Rock::Set_Attach(_bool Attach)
{
	m_bAttach = Attach;
	m_bVisible = true;
}

HRESULT CTroll_Rock::Render()
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

	return S_OK;
}

HRESULT CTroll_Rock::Ready_Components()
{

	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 18.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);


	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Troll_Rock_Big_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;                                                                                                                                                                     

	return S_OK;
}

HRESULT CTroll_Rock::Bind_ShaderResources()
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


CTroll_Rock* CTroll_Rock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTroll_Rock* pInstance = new CTroll_Rock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTroll_Rock");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CTroll_Rock::Clone(void* pArg, CGameObject* pOwner)
{
	CTroll_Rock* pInstance = new CTroll_Rock(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTroll_Rock");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTroll_Rock::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);

}
#ifdef _DEBUG

void CTroll_Rock::Describe_Entity()
{
	GUI::Begin("TrollRock");
	GUI::Checkbox("Visible", &m_bVisible);
	GUI::End();
}

#endif // _DEBUG
