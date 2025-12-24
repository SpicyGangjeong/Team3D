#include "pch.h"
#include "Transformation.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CTransformation::CTransformation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CTransformation::CTransformation(const CTransformation& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CTransformation::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Transformation")))
		return E_FAIL;

	return S_OK;

}

HRESULT CTransformation::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;
	
	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_iSkillType = ENUM_CLASS(SKILL_TYPE::TRANSFORMATION);

	m_wstrEffectName = L"Transformation";

	m_pTransformation_PJ = Get_PartObject<CEffectParts>("Transformation_PJ");
	SAFE_ADDREF(m_pTransformation_PJ);

	m_fDuration = 3.5f;

	return S_OK;
}

void CTransformation::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	XMStoreFloat4(&m_vStartPos, m_pTransformation_PJ->Get_WorldPostion());
}

void CTransformation::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	CTransform* pPJTransform = m_pTransformation_PJ->Get_Component<CTransform>();

	pPJTransform->Translation(XMLoadFloat3(&m_vCameraLook) * m_fLinearSpeed);


}

void CTransformation::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);

	XMStoreFloat4(&m_vEndPos, m_pTransformation_PJ->Get_WorldPostion());

	_vector vDir = XMLoadFloat4(&m_vEndPos) - XMLoadFloat4(&m_vStartPos);


	if (false == m_bHit) {
		_vector vStartPos = XMLoadFloat4(&m_vStartPos);
		_vector vEndPos = XMLoadFloat4(&m_vEndPos);
		ON_COLLISION_INFO CollisionInfo = SweepTarget(vStartPos, vEndPos, 0.002f);

		OnCollision(this, &CollisionInfo);
	}

}

HRESULT CTransformation::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	CWand* pWand = static_cast<CPlayer*>(m_pOwner)->Get_PartObject<CWand>();

	if (pWand == nullptr)
		return E_FAIL;

	m_pTransformation_PJ->Get_Effect_Info()->isDissolve = false;
	m_pTransformation_PJ->Set_Visible(true);

	CEffectParts* pWand_Distortion = Get_PartObject<CEffectParts>("Wand_Distortion");
	CEffectParts* pWand_Light = Get_PartObject<CEffectParts>("Wand_Light");
	CEffectParts* pWand_PT = Get_PartObject<CEffectParts>("Wand_PT");
	
	pWand_Distortion->Set_Visible(true);
	pWand_Light->Set_Visible(true);
	pWand_PT->Set_Visible(true);

	pWand_Distortion->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	pWand_Light->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	pWand_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

	_vector vDirection = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);
	XMStoreFloat3(&m_vCameraLook, vDirection);

	_vector vStartPos = pWand->Get_WorldPostion();
	XMStoreFloat4(&m_vStartPos, vStartPos);

	{ /* 대상 위치 지정 */

		m_pInfoInstance->Get_LockOnInfo(m_Info);
		if (nullptr != m_Info.pUnit) {

			XMStoreFloat4(&m_vTargetPos, m_Info.pUnit->Get_LockOnPos());

			XMStoreFloat3(&m_vCameraLook, XMVector3Normalize(XMLoadFloat4(&m_vTargetPos) - XMLoadFloat4(&m_vStartPos)));
		}
		else {
			// 타겟이 없다면 현재위치 -> 카메라 룩벡터 * duration간 예상 이동거리 를 대상으로 지정
			XMStoreFloat4(&m_vTargetPos, vStartPos + vDirection * m_fLinearSpeed * 0.5f);
		}
	}

	/* PJ 월드 구성하기  */


	_vector vRight = XMVector3Normalize(XMLoadFloat3(&m_vCameraLook));
	_vector vLook = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vRight));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));
	_vector vPos = pWand->Get_WorldPostion();

	_matrix PJ_WorldMat = { vRight , vUp ,vLook , vPos };

	m_pTransformation_PJ->Get_Component<CTransform>()->Set_WorldMatrix(PJ_WorldMat);
	return S_OK;
}

HRESULT CTransformation::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTransformation::Ready_Child()
{
	return S_OK;
}

CTransformation* CTransformation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransformation* pInstance = new CTransformation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransformation");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CTransformation::Clone(void* pArg, CGameObject* pOwner)
{
	CTransformation* pInstance = new CTransformation(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransformation");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTransformation::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	CCharacter_Controller* pCCT = CollisionDesc.pObject->Get_Component<CCharacter_Controller>();

	_vector vHitPos = XMLoadFloat4(&CollisionDesc.vWorldPos);

	_vector vFootPos = pCCT->Get_FootPosition();

	_float4 vCCTPos = {};

	XMStoreFloat4(&vCCTPos , pCCT->Get_Position());

	//m_pCharacter_Controller->Move(fTimeDelta);

	m_pInfoInstance->ActiveAt_Interactive(XMLoadFloat4(&vCCTPos));

	CollisionDesc.pObject->Set_Visible(false);
	//m_pTransformCom->Set_WorldMatrix(CollisionDesc.pObject->Get_Component<CRigidBody_Dynamic>()->Get_Actor()->getGlobalPose());



	CEffectParts* pHit_Light = Get_PartObject<CEffectParts>("Hit_Light");
	CEffectParts* pHit_PT = Get_PartObject<CEffectParts>("Hit_PT");

	pHit_Light->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHitPos);
	pHit_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHitPos);

	pHit_Light->Set_Visible(true);
	pHit_PT->Set_Visible(true);

	CEffectParts* pBlur_Mesh = Get_PartObject<CEffectParts>("Blur_Mesh");
	CEffectParts* pRotate_0 = Get_PartObject<CEffectParts>("Rotate_0");
	CEffectParts* pRotate_1 = Get_PartObject<CEffectParts>("Rotate_1");


	pBlur_Mesh->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);
	pRotate_0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);
	pRotate_1->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);

	pBlur_Mesh->Set_Visible(true);
	pRotate_0->Set_Visible(true);
	pRotate_1->Set_Visible(true);

	m_pTransformation_PJ->Get_Effect_Info()->isDissolve = true;
}

void CTransformation::Free()
{
	__super::Free();

	Safe_Release(m_pTransformation_PJ);

}
#ifdef _DEBUG
void CTransformation::Describe_Entity()
{

}
#endif

HRESULT CTransformation::Bind_ShaderResources()
{
	return S_OK;
}


