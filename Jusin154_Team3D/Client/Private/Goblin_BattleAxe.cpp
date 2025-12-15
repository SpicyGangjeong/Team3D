#include "pch.h"
#include "Goblin_BattleAxe.h"

#include "GameInstance.h"
#include "TrailObject.h"
#include "EffectParts.h"

CGoblin_BattleAxe::CGoblin_BattleAxe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CGoblin_BattleAxe::CGoblin_BattleAxe(const CGoblin_BattleAxe& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CGoblin_BattleAxe::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGoblin_BattleAxe::Initialize(void* pArg)
{
	GOBLIN_BATTLEAXE_DESC* pDesc = static_cast<GOBLIN_BATTLEAXE_DESC*> (pArg);
	m_pSocketMatrices = pDesc->pSocketMatrices;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

#ifdef _DEBUG
	m_pGripShape = (GeometricPrimitive::CreateSphere(m_pContext, 0.3f, 10, false, false));
	m_pSubShape = (GeometricPrimitive::CreateSphere(m_pContext, 0.6f, 10, false, false));
#endif // _DEBUG

	return S_OK;
}

void CGoblin_BattleAxe::Priority_Update(_float fTimeDelta)
{
	if (m_bVisible) {
		XMStoreFloat4(&m_vStartAxePos, Get_WorldPostion());
		XMStoreFloat4(&m_vStartGripPos, Get_WorldPostion());
	}
	m_pModelCom->Combined_BoneMatrix();
	_matrix socketMatrix = {};

	socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

	for (int i = 0; i < 3; ++i) {
		socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
	}

	m_pTransformCom->Set_WorldMatrix(socketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
	XMStoreFloat4x4(&m_vAxeMat, XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Bone")) * m_pTransformCom->Get_XMWorldMatrix());
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG
}

void CGoblin_BattleAxe::Update(_float fTimeDelta)
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

void CGoblin_BattleAxe::Late_Update(_float fTimeDelta)
{
	if (m_bVisible) {
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
		SwingHit(m_bPlayerHit);
	}
	else {
		m_bPlayerHit = false;
	}
}

HRESULT CGoblin_BattleAxe::Render()
{
	if (!m_pModelCom)
		return S_OK;

	if (!m_bVisible) {

		return S_OK;
	}

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (FAILED(Render_Disolve())) {
		return E_FAIL;
	}


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

#ifdef _DEBUG
	m_pGripShape->Draw(m_pTransformCom->Get_XMWorldMatrix(), m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW), m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ), DirectX::Colors::Green, nullptr, true);
	m_pSubShape->Draw(XMLoadFloat4x4(&m_vAxeMat), m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW), m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ), DirectX::Colors::Purple, nullptr, true);
#endif // _DEBUG
	return S_OK;
}

HRESULT CGoblin_BattleAxe::Render_Disolve()
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

HRESULT CGoblin_BattleAxe::Ready_Components()
{
	__super::Ready_Components(nullptr);

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Goblin_BattleAxe_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoblin_BattleAxe::Bind_ShaderResources()
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

void CGoblin_BattleAxe::CheckAxeHits(_uint& iHitCount, vector<PSX::PxSweepHit>& pxHits)
{
	{
		_vector vStartPos = XMLoadFloat4(&m_vStartAxePos);
		_vector vEndPos = XMVectorSet(m_vAxeMat._41, m_vAxeMat._42, m_vAxeMat._43,1.f);
		_vector vDir = vEndPos - vStartPos;
		_float fDistance = XMVectorGetX(XMVector4Length(vDir));
		vDir = XMVector4Normalize(vDir);

		_bool bCollision = false;
		m_SweepBufferAxe = {};
		bCollision = m_pGameInstance->SphereCast(0.5f, vStartPos, vDir, fDistance, PSX::PxHitFlag::eDEFAULT, PSX::PxQueryFlag::eDYNAMIC, m_SweepBufferAxe);
		if (true == bCollision) {
			iHitCount += m_SweepBufferAxe.getNbTouches() + m_SweepBufferAxe.hasBlock;
			auto touches = m_SweepBufferAxe.getTouches();
			for (_uint i = 0; i < m_SweepBufferAxe.nbTouches; ++i) {
				pxHits.push_back(m_SweepBufferAxe.touches[i]);
			}
			if (true == m_SweepBufferAxe.hasBlock) {
				pxHits.push_back(m_SweepBufferAxe.block);
			}
		}
	}
	{
		_vector vStartPos = XMLoadFloat4(&m_vStartGripPos);
		_vector vEndPos = m_pTransformCom->Get_State(STATE::POSITION);
		_vector vDir = vEndPos - vStartPos;
		_float fDistance = XMVectorGetX(XMVector4Length(vDir));
		vDir = XMVector4Normalize(vDir);

		_bool bCollision = false;
		m_SweepBufferGrip = {};
		bCollision = m_pGameInstance->SphereCast(0.5f, vStartPos, vDir, fDistance, PSX::PxHitFlag::eDEFAULT, PSX::PxQueryFlag::eDYNAMIC, m_SweepBufferGrip);
		if (true == bCollision) {
			iHitCount += m_SweepBufferGrip.getNbTouches() + m_SweepBufferGrip.hasBlock;
			auto touches = m_SweepBufferGrip.getTouches();
			for (_uint i = 0; i < m_SweepBufferGrip.nbTouches; ++i) {
				pxHits.push_back(m_SweepBufferGrip.touches[i]);
			}
			if (true == m_SweepBufferGrip.hasBlock) {
				pxHits.push_back(m_SweepBufferGrip.block);
			}
		}
	}
}


void CGoblin_BattleAxe::SwingHit(_bool& bPlayerHit)
{
	vector<PSX::PxSweepHit> pxHits;
	_uint iHitCount = 0;
	CheckAxeHits(iHitCount, pxHits);
	{
		for (_uint i = 0; i < pxHits.size(); ++i) {
			PSX::PxActor* pxHitActor = pxHits[i].actor;
			if (nullptr != pxHitActor && nullptr != pxHitActor->userData) {
				PhsXUserData* pUserData = (PhsXUserData*)pxHitActor->userData;
				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::PLAYER:
				{
					if (true == bPlayerHit) {
						continue;
					}
					CStat* pStat = pUserData->pCharacter->Get_Owner()->Get_Component<CStat>();
					pStat->Get_Damage(20.f);
					bPlayerHit = true;
					pUserData->pOwner->OnCollision(this);
				} break;
				case PXOBJECT::ALLY_HITBOX:
					break;
				case PXOBJECT::ENVIRIONMENT:
					break;
				case PXOBJECT::TERRAIN:
					break;
				case PXOBJECT::BOX:
					break;
				case PXOBJECT::NPC:
					break;
				default:
					break;
				}
			}
		}
	}
}


CGoblin_BattleAxe* CGoblin_BattleAxe::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin_BattleAxe* pInstance = new CGoblin_BattleAxe(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGoblin_BattleAxe");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CGoblin_BattleAxe::Clone(void* pArg, CGameObject* pOwner)
{
	CGoblin_BattleAxe* pInstance = new CGoblin_BattleAxe(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGoblin_BattleAxe");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CGoblin_BattleAxe::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);

}
#ifdef _DEBUG

void CGoblin_BattleAxe::Describe_Entity()
{
}

#endif // _DEBUG
