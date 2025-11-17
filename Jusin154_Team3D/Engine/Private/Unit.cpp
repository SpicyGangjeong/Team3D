#include "pch.h"
#include "Unit.h"

#include "GameInstance.h"

CUnit::CUnit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUnit::CUnit(const CUnit& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CUnit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUnit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_Animation.resize(FSMSTATE::END);

	return S_OK;
}

void CUnit::Priority_Update(_float fTimeDelta)
{

}

void CUnit::Update(_float fTimeDelta)
{
}

void CUnit::Late_Update(_float fTimeDelta)
{
}

HRESULT CUnit::Render()
{
	return S_OK;
}

HRESULT CUnit::Ready_Components(void *pArg)
{
	__super::Ready_Components(pArg);

	/* Com_FSM */
	if (FAILED(Add_Component<CFSM>(g_iStaticLevel, &m_pFSM)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUnit::Bind_ShaderResources()
{
	return S_OK;
}

void CUnit::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
	SAFE_RELEASE(m_pFSM);
}

void CUnit::Describe_Entity()
{
}
