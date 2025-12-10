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

_bool CMonster::Get_Damage(_float fDamage)
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

_matrix CMonster::Get_HeadMatrix()
{
	return m_pModelCom->Get_BoneMatrix("Head");
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
}

#endif // _DEBUG
