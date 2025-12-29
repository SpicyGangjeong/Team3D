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
	if (FAILED(__super::Initialize(pArg))){
		return E_FAIL;
	}

	m_pInfoInstance->Regist_ActiveMonster(this);

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


	_vector vDir = XMLoadFloat4(&m_vTargetPos) - Get_WorldPostion();
	vDir = XMVector4Normalize(vDir);
	_vector vLook = XMVector3Normalize(
		XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));
	_float dot = XMVectorGetX(XMVector3Dot(vLook, vDir));
	dot = max(-1.f, min(1.f, dot));

	_float angle = acosf(dot);
	m_fDegree = XMConvertToDegrees(angle);

	m_fCross = XMVectorGetY(XMVector3Cross(vLook, vDir));
}

HRESULT CMonster::Render_DeadDisolve()
{
	if (FLT_EPSILON3 * 10 < m_fDeadRatio) {
		_bool bDisolve = true;
		_float fDisolveAmount = 0.1f;
		_float fDisolveEdgeWidth = 0.1f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bDisolve", &bDisolve, sizeof(_bool)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveRatio", &m_fDeadRatio, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveAmount", &fDisolveAmount, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveEdgeWidth", &fDisolveEdgeWidth, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Bind_GlobalSRV(m_pShaderCom, TEXT("GLOBAL_DISOLVE_NOISE_05"), "g_DeadDisolveTexture"))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Bind_GlobalSRV(m_pShaderCom, TEXT("GLOBAL_DISOLVE_BURN_VERTICAL"), "g_DeadDisolveBurnTexture"))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CMonster::Render_OutLine()
{
	m_bDrawOutLine = false;
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", m_pTransformCom->Get_PrevWorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevViewMatrix", D3DTS::VIEW))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevProjMatrix", D3DTS::PROJ))) {
		return E_FAIL;
	}
	Compute_Depth();
	_float fRatio = (m_fCamDepth / *m_pGameInstance->Get_CurrentCameraFar());
	m_fOutLineThickness = CMyTools::Lerp_f1D(2.f, 6.f, fRatio);
	if (m_fOutLineThickness > 6.f) {
		m_fOutLineThickness = 6.f;
	}
	else if (m_fOutLineThickness < 2.f) {
		m_fOutLineThickness = 2.f;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vOutLineColor", &m_vOutLineColor, sizeof(_float3)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLineThickness", &m_fOutLineThickness, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLineScale", &m_fOutLineScale, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLinePower", &m_fOutLinePower, sizeof(_float)))) {
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
	return { m_pStat->Get_Stat().fCurrentHp, m_pStat->Get_Stat().fMaxHp };
}

CStat* CMonster::Get_Stat()
{
	return m_pStat;
}

const _float4x4* CMonster::Get_HeadMatrix()
{
	return m_pModelCom->Get_BoneMatrixPtr("Head");
}

void CMonster::CameraShake(_float ClampValue, _float Min, _float Max, _float Time)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pInfoInstance->Get_NearestPlayerAlly(Get_WorldPostion()).first);
	if (pPlayer == nullptr)
		return;
	_float fDistance = XMVectorGetX(XMVector4Length(pPlayer->Get_WorldPostion() - Get_WorldPostion()));
	_float fShakeValue = clamp(ClampValue / fDistance, Min, Max);
	pPlayer->Start_CameraShake(Time, fShakeValue);
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

HRESULT CMonster::Render_Disolve()
{
	if (FLT_EPSILON3 * 10 < m_fDisolveTime)
	{
		m_bDrawOutLine = false;
		_int bDisolve = 1;
		_float fDisolveAmount = 0.1f;
		_float fDisolveEdgeWidth = 0.1f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bDisolve", &bDisolve, sizeof(_int)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveRatio", &m_fDisolveTime, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveAmount", &fDisolveAmount, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveEdgeWidth", &fDisolveEdgeWidth, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Bind_GlobalSRV(m_pShaderCom, TEXT("GLOBAL_DISOLVE_NOISE_05"), "g_DeadDisolveTexture"))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Bind_GlobalSRV(m_pShaderCom, TEXT("GLOBAL_DISOLVE_BURN_VERTICAL"), "g_DeadDisolveBurnTexture"))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

void CMonster::Set_Easing(_uint iAnimIndex,_float fEasingStartRatio,_float fEasingEndRatio,_float fEasingTime)
{
	Add_Event(iAnimIndex,
		[this, fEasingTime]() {m_fEasing = fEasingTime;},
		fEasingStartRatio);

	Add_Event(iAnimIndex,
		[this]() {m_fEasing = 1.f; },
		fEasingEndRatio);
}


void CMonster::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pStat);
	SAFE_RELEASE(m_pTarget);
	if (nullptr != m_pInfoInstance){
		CInfoInstance* pInfo = m_pInfoInstance;  
		m_pInfoInstance = nullptr;
		pInfo->Deregist_ActiveMonster(this);
	}
}
#ifdef _DEBUG

void CMonster::Describe_Entity()
{
	if (ImGui::TreeNode("ANIM STATE")) {

		for (auto& pState : m_States)
		{
			if (ImGui::Button(to_string(pState.first).c_str()))
			{
				m_pFSM->Change_State(pState.first);
			}

		}

		GUI::Text(to_string(m_pModelCom->Get_CurrentTrackProgressRatio()).c_str());

		ImGui::TreePop();
	}
}

#endif // _DEBUG
