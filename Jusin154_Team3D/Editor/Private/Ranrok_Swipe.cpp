#include "pch.h"
#include "Ranrok_Swipe.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CRanrok_Swipe::CRanrok_Swipe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CRanrok_Swipe::CRanrok_Swipe(const CRanrok_Swipe& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CRanrok_Swipe::Initialize_Prototype()
{


	return S_OK;

}

HRESULT CRanrok_Swipe::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/Ranrok/RanrokSwipe")))
		return E_FAIL;

	m_wstrEffectName = L"RanrokSwipe";

	m_pRock_PT = Get_PartObject<CEditEffect>("Rock_PT");
	SAFE_ADDREF(m_pRock_PT);

	m_fDuration = 5.f;


	m_Events.emplace(0.4f, [&]() {
		
		CEditEffect* pClawDecal = Get_PartObject<CEditEffect>("ClawDecal");
		pClawDecal->Set_Visible(true);

		CEditEffect* pSmoke = Get_PartObject<CEditEffect>("Swipe_Smoke");
		pSmoke->Set_Visible(true);

		_vector vComputePos = Compute_Pos();
		
		pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vComputePos);

		_vector vLook = XMVector3Normalize(m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK));
		_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f) , vLook));
		_vector vUp = XMVector3Normalize(XMVector3Cross(vLook , vRight));
		_vector vPos = vComputePos;

		_matrix PJ_WorldMat = { vRight , vUp ,vLook , vPos };

		pClawDecal->Get_Component<CTransform>()->Set_WorldMatrix(PJ_WorldMat);
		});

	m_Events.emplace(0.8f, [&]() {
		m_isParticleEnd = true;
		m_pRock_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -999.f, 0.f, 1.f));
		});

	return S_OK;
}

void CRanrok_Swipe::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CRanrok_Swipe::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


	if (m_isParticleEnd == false)
	{
		_vector vComputePos = Compute_Pos();
		m_pRock_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vComputePos);

	}

	
}

void CRanrok_Swipe::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CRanrok_Swipe::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	 /* 초기 객체 비지블*/
	m_pRock_PT->Set_Visible(true);


	 /* 초기 객체 위치 초기화*/
	_vector vComputePos = Compute_Pos();
	m_pRock_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vComputePos);

	m_isParticleEnd = false;
	return S_OK;
}

HRESULT CRanrok_Swipe::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRanrok_Swipe::Ready_Child()
{
	return S_OK;
}

CRanrok_Swipe* CRanrok_Swipe::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok_Swipe* pInstance = new CRanrok_Swipe(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrok_Swipe");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CRanrok_Swipe::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok_Swipe* pInstance = new CRanrok_Swipe(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrok_Swipe");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRanrok_Swipe::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);


}

_fvector CRanrok_Swipe::Compute_Pos()
{
	_matrix BoneMat = XMLoadFloat4x4(m_pOwner->Get_Component<CModel>()->Get_BoneMatrixPtr("wrist_left_target")) * m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix();

	_vector vBonePos = BoneMat.r[3];


	CUnit* pPlayer = m_pInfoInstance->Get_NearestPlayerAlly(m_pOwner->Get_WorldPostion()).first;



	_vector vPlayer_FootPos = pPlayer->Get_Component<CCharacter_Controller>()->Get_FootPosition();

	XMStoreFloat4(&m_vTargetPos, vPlayer_FootPos);

	_vector vCompute_Pos = XMVectorSetY(vBonePos, XMVectorGetY(vPlayer_FootPos));

	return vCompute_Pos;
}

void CRanrok_Swipe::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);
	SAFE_RELEASE(m_pRock_PT);

}
#ifdef _DEBUG
void CRanrok_Swipe::Describe_Entity()
{

}
#endif

HRESULT CRanrok_Swipe::Bind_ShaderResources()
{
	return S_OK;
}


