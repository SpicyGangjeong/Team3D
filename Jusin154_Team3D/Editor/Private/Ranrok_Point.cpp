#include "pch.h"
#include "Ranrok_Point.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CRanrok_Point::CRanrok_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CRanrok_Point::CRanrok_Point(const CRanrok_Point& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CRanrok_Point::Initialize_Prototype()
{


	return S_OK;

}

HRESULT CRanrok_Point::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/Ranrok/RanrokPoint")))
		return E_FAIL;

	m_wstrEffectName = L"RanrokPoint";


	m_pGooRod_0 = Get_PartObject<CEditEffect>("GooRod_0");
	m_pSmoke = Get_PartObject<CEditEffect>("Smoke");
	m_pSpread_PT = Get_PartObject<CEditEffect>("Spread_PT");

	SAFE_ADDREF(m_pGooRod_0);
	SAFE_ADDREF(m_pSmoke);
	SAFE_ADDREF(m_pSpread_PT);

	m_fDuration = 30.f;
	

	return S_OK;
}

void CRanrok_Point::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CRanrok_Point::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


	/* PJ 월드 구성하기  */
	if (m_isStop == false)
	{
		/* 초기 방향 */

		XMStoreFloat3(&m_vLook, m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK));

		_vector vRight = XMVector3Normalize(XMLoadFloat3(&m_vLook));
		_vector vLook = XMVector3Normalize(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
		_vector vPos = m_pOwner->Get_WorldPostion();

		_matrix PJ_WorldMat = { vRight , vUp ,vLook , vPos };

		m_pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
		m_pGooRod_0->Get_Component<CTransform>()->Set_WorldMatrix(PJ_WorldMat);
		m_pSpread_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	}

	
}

void CRanrok_Point::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CRanrok_Point::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	 /* 초기 객체 비지블*/

	 /* 초기 객체 위치 초기화*/

	 /* 초기 방향 */
	 XMStoreFloat3(&m_vLook, m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK));

	 /* PJ 월드 구성하기  */


	 _vector vRight = XMVector3Normalize(XMLoadFloat3(&m_vLook));
	 _vector vLook = XMVector3Normalize(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	 _vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
	 _vector vPos = m_pOwner->Get_WorldPostion();

	 _matrix PJ_WorldMat = { vRight , vUp ,vLook , vPos };


	 m_pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	 m_pSpread_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	 m_pGooRod_0->Get_Component<CTransform>()->Set_WorldMatrix(PJ_WorldMat);

	 m_pSmoke->Set_Visible(true);
	 m_pGooRod_0->Set_Visible(true);
	 m_pSpread_PT->Set_Visible(true);

	return S_OK;
}

HRESULT CRanrok_Point::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRanrok_Point::Ready_Child()
{
	return S_OK;
}

CRanrok_Point* CRanrok_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok_Point* pInstance = new CRanrok_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrok_Point");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CRanrok_Point::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok_Point* pInstance = new CRanrok_Point(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrok_Point");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRanrok_Point::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);


	for (auto& pPair : m_PartObjects)
	{
		pPair.second->Set_Visible(true);
		pPair.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	}


}

void CRanrok_Point::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);

	SAFE_RELEASE(m_pGooRod_0);
	SAFE_RELEASE(m_pSmoke);
	SAFE_RELEASE(m_pSpread_PT);

}
#ifdef _DEBUG
void CRanrok_Point::Describe_Entity()
{

}
#endif

HRESULT CRanrok_Point::Bind_ShaderResources()
{
	return S_OK;
}


