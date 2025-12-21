#include "pch.h"
#include "Blink.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Player.h"
#include "Wand.h"
#include "TrailObject.h"


CBlink::CBlink(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CBlink::CBlink(const CBlink& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CBlink::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Blink")))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlink::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"Blink";


	m_pBlink_Swirl = Get_PartObject<CEffectParts>("BlinkSwirl");
	m_pBlink_Circle = Get_PartObject<CEffectParts>("Blink_Circle");
	m_pBlink_Circle2 = Get_PartObject<CEffectParts>("Blink_Circle2");
	m_pBlink_Distortion = Get_PartObject<CEffectParts>("Blink_Distortion");

	for (size_t i = 0; i < 10; i++)
	{
		_string strPartsName = "Blink_Trail" + to_string(i);
		CTrailObject* pTrail = Get_PartObject<CTrailObject>(strPartsName);

		m_Blink_Trails.push_back(pTrail);
		SAFE_ADDREF(pTrail);
	}

	m_BoneNames[0] = "SKT_Back";
	m_BoneNames[1] = "Neck";
	m_BoneNames[2] = "Spine2";
	m_BoneNames[3] = "LeftForeArm";
	m_BoneNames[4] = "Spine";
	m_BoneNames[5] = "RightArm";
	m_BoneNames[6] = "LeftArm";
	m_BoneNames[7] = "RightUpLegTwist1";
	m_BoneNames[8] = "LeftUpLegTwist1";
	m_BoneNames[9] = "RightForeArm";


	SAFE_ADDREF(m_pBlink_Swirl);
	SAFE_ADDREF(m_pBlink_Circle);
	SAFE_ADDREF(m_pBlink_Distortion);
	SAFE_ADDREF(m_pBlink_Circle2);


	m_Events.emplace(0.85f, [&]() {
		for (auto& pTrail : m_Blink_Trails)
		{
			pTrail->SetDissolve(true);
		}


		});

	m_Events.emplace(1.1f, [&]() {

		m_isParticleEnd = true;
		m_pBlink_Swirl->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -500.f, 0.f, 1.f));
		});

	m_fAmountSize = 1.f;
	m_fSpeed = 1.6f;


	m_fDuration = 1.45f;



	return S_OK;
}

void CBlink::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CBlink::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	_uint iIndex = {};


	for (auto& pTrail : m_Blink_Trails)
	{
		_matrix BoneMatrix = XMLoadFloat4x4(m_pOwner->Get_Component<CModel>()->Get_BoneMatrixPtr(m_BoneNames[iIndex++].c_str()));
		_matrix PlayerWorld = m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix();
		_matrix CombinedMat = BoneMatrix * PlayerWorld;



		if (m_fMoveAccTime > XM_2PI)
			m_fMoveAccTime = 0.f;

		m_fMoveAccTime += fTimeDelta * m_fMoveSpeed;

		_float fAmount = sinf(m_fMoveAccTime) * m_fAmountMove;

		CombinedMat.r[3] += XMVectorSet(0.f, fAmount, 0.f, 0.f);

		pTrail->Oneside_Rope_Trail_Update(CombinedMat, fTimeDelta);
	}


	if (m_isParticleEnd == false)
	{
		m_pBlink_Swirl->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	}


	m_pBlink_Circle->Get_Component<CTransform>()->Set_WorldMatrix(m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix());
	m_pBlink_Circle2->Get_Component<CTransform>()->Set_WorldMatrix(m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix());

	m_pBlink_Distortion->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pGameInstance->Get_CamXMPosition() + m_pGameInstance->Get_CameraLook() * m_fLength);


	if (m_fSizeAccTime > XM_PIDIV2)
		return;

	m_fSizeAccTime += fTimeDelta * m_fSpeed;

	_float fSize = sinf(m_fSizeAccTime) * m_fAmountSize;
	_float3 vSize = _float3(fSize, fSize, fSize);

	m_pBlink_Circle->Get_Component<CTransform>()->Set_Scale(vSize);
	m_pBlink_Circle2->Get_Component<CTransform>()->Set_Scale(vSize);
}

void CBlink::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CBlink::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	m_pBlink_Swirl->Set_Visible(true);
	m_pBlink_Circle->Set_Visible(true);
	m_pBlink_Circle2->Set_Visible(true);
	/*트레일 초기화 */

	m_isTrailEnd = false;
	m_isParticleEnd = false;

	for (auto& pTrail : m_Blink_Trails)
	{
		pTrail->SetDissolve(false);
		pTrail->Set_Visible(true);
		pTrail->Get_Component<CTrail>()->Reset_Trail();
		pTrail->Get_TrailInfo()->vDissolveTime.y = 0.5f;
	}


	m_pBlink_Swirl->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());



	m_pBlink_Distortion->Set_Visible(true);


	m_fSizeAccTime = 0.f;
	m_fMoveAccTime = 0.f;

	return S_OK;
}

HRESULT CBlink::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBlink::Ready_Child()
{
	return S_OK;
}

CBlink* CBlink::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlink* pInstance = new CBlink(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBlink");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CBlink::Clone(void* pArg, CGameObject* pOwner)
{
	CBlink* pInstance = new CBlink(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBlink");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBlink::OnCollision(CGameObject* pOther, void* pDesc)
{
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CBlink::Free()
{
	__super::Free();


	SAFE_RELEASE(m_pBlink_Swirl);
	SAFE_RELEASE(m_pBlink_Circle);
	SAFE_RELEASE(m_pBlink_Distortion);
	SAFE_RELEASE(m_pBlink_Circle2);

	for (auto& pTrail : m_Blink_Trails)
	{
		SAFE_RELEASE(pTrail);
	}

	m_Blink_Trails.clear();
}
#ifdef _DEBUG

void CBlink::Describe_Entity()
{
	GUI::Begin("Blink");

	GUI::DragFloat("Length", &m_fLength);
	GUI::DragFloat("MoveAmount", &m_fAmountMove);
	GUI::DragFloat("m_fMoveSpeed", &m_fMoveSpeed);

	GUI::End();
}

#endif // _DEBUG

HRESULT CBlink::Bind_ShaderResources()
{
	return S_OK;
}


