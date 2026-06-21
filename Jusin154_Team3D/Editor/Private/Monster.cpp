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
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	m_pInfoInstance->Regist_ActiveMonster(this);

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->ResetVelocityVector();
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

float  CMonster::Get_HpRatio()
{
	return m_fHp / m_fMaxHp;
}

HRESULT CMonster::Render_OutLine()
{
	m_bDrawOutLine = false;
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	GUI::SetNextItemWidth(80.f);
	static _float3 vOutLineColor = _float3(1.f, 0.960784376f, 0.933333397f);
	static _float fOutLineScale = { 1.f };
	static _float fOutLinePower = { 1.f };
	GUI::ColorPicker3("vOutLineColor", (_float*)&vOutLineColor);
	GUI::SliderFloat("Scale", &fOutLineScale, 0.1f, 2.f, "%.1f");
	GUI::SliderFloat("Power", &fOutLinePower, 0.1f, 2.f, "%.1f");

	Compute_Depth();
	_float fCamFar = *m_pGameInstance->Get_CurrentCameraFar();
	_float fRatio = CMyTools::Saturate((m_fCamDepth / (fCamFar * fCamFar)));
	_float fOutLineThickness = CMyTools::Lerp_f1D(1.5f, 2.f, fRatio);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vOutLineColor", &vOutLineColor, sizeof(_float3)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLineThickness", &(fOutLineThickness), sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLineScale", &(fOutLineScale), sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLinePower", &(fOutLinePower), sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)))) {
		return E_FAIL;
	}
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices"))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::OUTLINE_READ)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

void CMonster::Set_DrawOutLine()
{
	m_bDrawOutLine = true;
}

pair<_float, _float> CMonster::Get_Damage(_float fDamage)
{
	return  m_pStat->Get_Damage(fDamage);
}

_float2 CMonster::Get_Hp()
{
	return { m_pStat->Get_Stat().fCurrentHp, m_pStat->Get_Stat().fMaxHp};
}

HRESULT CMonster::Ready_Components(void* pArg)
{
	__super::Ready_Components(pArg);

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}

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

	SAFE_RELEASE(m_pStat);
	SAFE_RELEASE(m_pTarget);
	if (nullptr != m_pInfoInstance) {
		CInfoInstance* pInfo = m_pInfoInstance;
		m_pInfoInstance = nullptr;
		pInfo->Deregist_ActiveMonster(this);
	}
}
#ifdef _DEBUG

void CMonster::Describe_Entity()
{

	if (GUI::TreeNode("ANIM STATE")) {

		for (auto& pState : m_States)
		{
			if (GUI::Button(to_string(pState.first).c_str()))
			{
				m_pFSM->Change_State(pState.first);
			}

		}

		GUI::Text(to_string(m_pModelCom->Get_CurrentTrackProgressRatio()).c_str());

		GUI::TreePop();
	}
}

#endif // _DEBUG
