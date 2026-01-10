#include "pch.h"

#include "CamPosition_Target.h"

CCamPosition_Target::CCamPosition_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamPosition(pDevice, pContext)
{
}
CCamPosition_Target::CCamPosition_Target(const CCamPosition_Target& rhs)
	:CCamPosition(rhs)
{
}

void CCamPosition_Target::Priority_Update(_float fTimeDelta)
{
	if (nullptr != m_pStalkingTarget) {
		Set_WorldPostion(m_pStalkingTarget->Get_WorldPostion());
	}
}

void CCamPosition_Target::Update(_float fTimeDelta)
{
}

void CCamPosition_Target::Late_Update(_float fTimeDelta)
{
}

_vector CCamPosition_Target::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CCamPosition_Target::Set_WorldPostion(_vector vPos)
{
	m_pTransformCom->Set_State(STATE::POSITION, vPos);
}

void CCamPosition_Target::Stalking_Target(CGameObject* pStalkingTarget)
{
	SAFE_RELEASE(m_pStalkingTarget);
	m_pStalkingTarget = pStalkingTarget;
	SAFE_ADDREF(m_pStalkingTarget);
}

void CCamPosition_Target::Stop_Stalking()
{
	SAFE_RELEASE(m_pStalkingTarget);
}

HRESULT CCamPosition_Target::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CCamPosition_Target::Initialize(void* pArg)
{
	CAMERAPOSITION_TARGET_DESC* pDesc = static_cast<CAMERAPOSITION_TARGET_DESC*> (pArg);
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCamPosition_Target::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCamPosition_Target::Bind_ShaderResources()
{
	return S_OK;
}

CCamPosition_Target* CCamPosition_Target::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamPosition_Target* pInstance = new CCamPosition_Target(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamPosition_Target");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
CCamPosition_Target* CCamPosition_Target::Clone(void* pArg, class CGameObject* pOWner)
{
	CCamPosition_Target* pInstance = new CCamPosition_Target(*this);
	pInstance->m_pOwner = pOWner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamPosition_Target");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CCamPosition_Target::Free()
{
	__super::Free();
	SAFE_RELEASE(m_pStalkingTarget);
}
#ifdef _DEBUG

void CCamPosition_Target::Describe_Entity()
{
}

#endif // _DEBUG
