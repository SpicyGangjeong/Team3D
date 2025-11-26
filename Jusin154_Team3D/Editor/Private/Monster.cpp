#include "pch.h"
#include "Monster.h"

#include "GameInstance.h"
#include "Player.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnit(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster& Prototype)
	: CUnit(Prototype)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pPlayerTransform = static_cast<CPlayer*>(pArg)->Get_Component<CTransform>();
	SAFE_ADDREF(m_pPlayerTransform);

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{

}

void CMonster::Update(_float fTimeDelta)
{

}

void CMonster::Late_Update(_float fTimeDelta)
{
	m_fTargetDistance = m_pTransformCom->TargetDis(m_pPlayerTransform->Get_State(STATE::POSITION));
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
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Bind_ShaderResources()
{
	return S_OK;
}

void CMonster::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pPlayerTransform);
}

void CMonster::Describe_Entity()
{
}
