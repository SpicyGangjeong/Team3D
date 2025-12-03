#include "pch.h"
#include "Lumos.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"

CLumos::CLumos(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CLumos::CLumos(const CLumos& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CLumos::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CLumos::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Lumos")))
		return E_FAIL;

	m_wstrEffectName = L"Lumos";

	m_pLumos_Light = Get_PartObject<CEffectParts>("Lumos");

	SAFE_ADDREF(m_pLumos_Light);

	m_fDuration = 2.5f;

	return S_OK;
}

void CLumos::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CLumos::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return;

	m_pLumos_Light->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

}

void CLumos::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CLumos::Pre_Setting(CGameObject* pObject)
{
	if (FAILED(__super::Pre_Setting(pObject)))
		return E_FAIL;

	CWand* pWand = static_cast<CWand*>(m_pOwner);

	if (pWand == nullptr)
		return E_FAIL;

	m_pLumos_Light->Set_Visible(true);
	m_pLumos_Light->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

	m_isLoop = true;

	return S_OK;
}

HRESULT CLumos::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLumos::Ready_Child()
{
	return S_OK;
}

CLumos* CLumos::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLumos* pInstance = new CLumos(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLumos");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CLumos::Clone(void* pArg, CGameObject* pOwner)
{
	CLumos* pInstance = new CLumos(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLumos");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLumos::OnCollision(CGameObject* pOther, void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CLumos::Free()
{
	__super::Free();

	Safe_Release(m_pLumos_Light);

}
#ifdef _DEBUG

void CLumos::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CLumos::Bind_ShaderResources()
{
	return S_OK;
}


