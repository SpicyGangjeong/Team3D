#include "pch.h"
#include "Monster.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "Player.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnit(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster& Prototype)
	: CUnit(Prototype),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))){
		return E_FAIL;
	}


	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->RewindMomentum();
	__super::Priority_Update(fTimeDelta);
}

void CMonster::Update(_float fTimeDelta)
{
	_vector vCurrentPos = Get_WorldPostion();
	pair<CUnit*, CTransform*> pairTarget = m_pInfoInstance->Get_NearestPlayerAlly(vCurrentPos);
	if (nullptr != pairTarget.first) {
		Set_Target(*pairTarget.first, *pairTarget.second);
	}
	if (nullptr != m_pTarget && m_pTarget->isDead()) {
		SAFE_RELEASE(m_pTarget);
		m_fTargetDistance = { FLT_MAX };
	}
	__super::Update(fTimeDelta);
}

void CMonster::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CMonster::Render()
{
	return S_OK;
}

HRESULT CMonster::Ready_Components(void*pArg)
{
	__super::Ready_Components(pArg);

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))){
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster::Bind_ShaderResources()
{
	return S_OK;
}

void CMonster::Set_Target(CUnit& pTarget, CTransform& pTransform)
{
	SAFE_RELEASE(m_pTarget);
	m_pTarget = &pTarget;
	SAFE_ADDREF(m_pTarget);

	_vector vTargetPos = pTransform.Get_State(STATE::POSITION);
	_vector vToTargetDir = vTargetPos - Get_WorldPostion();
	XMStoreFloat4(&m_vTargetPos, vTargetPos);
	XMStoreFloat3(&m_vToTargetDir, XMVector3Normalize(vToTargetDir));
	m_fTargetDistance = XMVectorGetX(XMVector3Length(vToTargetDir));
}

void CMonster::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pTarget);
	SAFE_RELEASE(m_pInfoInstance);
}
#ifdef _DEBUG

void CMonster::Describe_Entity()
{
}

#endif // _DEBUG
