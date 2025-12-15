#include "pch.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Transform.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
}

CGameObject::CGameObject(const CGameObject& rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_pGameInstance(rhs.m_pGameInstance)
	, m_bDead(rhs.m_bDead)
	, m_bCloned(true)
{
	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	return S_OK;
}

void CGameObject::Compute_Depth()
{
	_vector		vCamPosition = XMLoadFloat4(m_pGameInstance->Get_CamPosition());

	m_fCamDepth = XMVectorGetX(XMVector3LengthSq(vCamPosition - m_pTransformCom->Get_State(STATE::POSITION)));
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{
}

void CGameObject::Late_Update(_float fTimeDelta)
{
}

void CGameObject::OnCollision(CGameObject* pOther , void* pDesc)
{
}
void CGameObject::OnHit(CGameObject* pOther, CGameObject* pCaller)
{
}
_vector CGameObject::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}


HRESULT CGameObject::Add_Asset_Component(_uint iTargetLevel, const _wstring strPrototypeTag, CComponent** ppOut, void* pArg)
{
	CComponent* pComponent = m_pGameInstance->Clone_Asset_Prototype(iTargetLevel, strPrototypeTag, pArg, this);

	if (nullptr == pComponent) {
		return E_FAIL;
	}

	m_Components.push_back(pComponent);

	if (ppOut != nullptr)
	{
		*ppOut = pComponent;
		SAFE_ADDREF(pComponent);
	}


	return S_OK;
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

_float CGameObject::Get_Depth()
{
	return m_fCamDepth;
}

void CGameObject::Set_Shadow(pair<_bool, _ubyte> shadowResult)
{
	if (true == shadowResult.first) {
		m_iShadow = shadowResult.second;

		if (0 < (m_iShadow & (_ubyte)SHADOW::SHADOW_NEAR)) {
			m_pGameInstance->Add_RenderGroup(RENDER::SHADOW_NEAR, this);
		}
		if (0 < (m_iShadow & (_ubyte)SHADOW::SHADOW_MIDDLE)) {
			m_pGameInstance->Add_RenderGroup(RENDER::SHADOW_MIDDLE, this);
		}
		if (0 < (m_iShadow & (_ubyte)SHADOW::SHADOW_FAR)) {
			m_pGameInstance->Add_RenderGroup(RENDER::SHADOW_FAR, this);
		}
	}
	else {
		m_iShadow = 0;
	}
}

HRESULT CGameObject::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CTransform>(g_iStaticLevel, &m_pTransformCom, pArg))) {
		return E_FAIL;
	}
	return S_OK;
}

void CGameObject::Free()
{
	__super::Free();

	for (auto& pComponent : m_Components) {
		SAFE_RELEASE(pComponent);
	} m_Components.clear();

	SAFE_RELEASE(m_pTransformCom);
	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
