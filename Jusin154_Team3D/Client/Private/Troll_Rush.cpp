#include "pch.h"
#include "Troll_Rush.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"

CTroll_Rush::CTroll_Rush(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CTroll_Rush::CTroll_Rush(const CTroll_Rush& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CTroll_Rush::Initialize_Prototype()
{
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/TrollRushBlur")))
		return E_FAIL;


	return S_OK;

}

HRESULT CTroll_Rush::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;



	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"TrollRushBlur";


	m_pRush_Blur = Get_PartObject<CEffectParts>("Rush_Blur");

	SAFE_ADDREF(m_pRush_Blur);

	m_fDuration = 2.5f;

	return S_OK;
}

void CTroll_Rush::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CTroll_Rush::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	CCharacter_Controller* pCCT = m_pOwner->Get_Component<CCharacter_Controller>();

	_vector vPos = pCCT->Get_Position();

	vPos += m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK) * 2.f; 

	m_pRush_Blur->Set_Visible(true);

	_vector vRight = XMVector3Normalize(m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK));
	_vector vLook = XMVector3Normalize(XMVector3Cross(vRight , XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook ,vRight));

	_matrix PJ_WorldMat = { vRight , vUp ,vLook , vPos };

	m_pRush_Blur->Get_Component<CTransform>()->Set_WorldMatrix(PJ_WorldMat);

}

void CTroll_Rush::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CTroll_Rush::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	m_pRush_Blur->Get_Component<CInstance_Model>()->Set_Loop(true);

	CCharacter_Controller* pCCT = m_pOwner->Get_Component<CCharacter_Controller>();

	_vector vPos = pCCT->Get_Position();

	vPos += m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);


	m_pRush_Blur->Set_Visible(true);

	_vector vRight = XMVector3Normalize( m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK));
	_vector vLook = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vRight));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));

	_matrix PJ_WorldMat = { vRight , vUp ,vLook , vPos };

	m_pRush_Blur->Get_Component<CTransform>()->Set_WorldMatrix(PJ_WorldMat);

	return S_OK;
}

HRESULT CTroll_Rush::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTroll_Rush::Ready_Child()
{
	return S_OK;
}

CTroll_Rush* CTroll_Rush::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTroll_Rush* pInstance = new CTroll_Rush(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTroll_Rush");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CTroll_Rush::Clone(void* pArg, CGameObject* pOwner)
{
	CTroll_Rush* pInstance = new CTroll_Rush(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTroll_Rush");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTroll_Rush::OnCollision(CGameObject* pOther, void* pDesc)
{


}

void CTroll_Rush::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pRush_Blur);
}
#ifdef _DEBUG
void CTroll_Rush::Describe_Entity()
{

}
#endif

HRESULT CTroll_Rush::Bind_ShaderResources()
{
	return S_OK;
}


