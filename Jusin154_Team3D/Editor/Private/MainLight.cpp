#include "pch.h"
#include "MainLight.h"

#include "GameInstance.h"
#include "Camera_Debug.h"
#include "GameTime.h"

CMainLight::CMainLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMainLight::CMainLight(const CMainLight& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CMainLight::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMainLight::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::LOOK, XMVectorSet(1.f, -1.f, 1.f, 0.f));

	m_pGameTime = CGameTime::Create();

	if (nullptr == m_pGameTime)
		return E_FAIL;

	// --- m_Diffuse_Colors 초기화 (주요 색상) ---
	m_Diffuse_Colors[ENUM_CLASS(DAY_PHASE::DAWN)] = { 0.7f, 0.6f, 0.4f, 1.0f }; // 따뜻한 새벽 햇살
	m_Diffuse_Colors[ENUM_CLASS(DAY_PHASE::DAY)] = { 1.0f, 1.0f, 1.0f, 1.0f }; // 순백색 주광
	m_Diffuse_Colors[ENUM_CLASS(DAY_PHASE::DUSK)] = { 0.9f, 0.4f, 0.2f, 1.0f }; // 붉은 석양
	m_Diffuse_Colors[ENUM_CLASS(DAY_PHASE::NIGHT)] = { 0.2f, 0.2f, 0.4f, 1.0f }; // 푸른 달빛

	// --- m_Ambient_Colors 초기화 (그림자 및 전체적인 밝기) ---
	m_Ambient_Colors[ENUM_CLASS(DAY_PHASE::DAWN)] = { 0.3f, 0.3f, 0.4f, 1.0f }; // 그림자도 약간 푸른빛
	m_Ambient_Colors[ENUM_CLASS(DAY_PHASE::DAY)] = { 0.4f, 0.4f, 0.4f, 1.0f }; // 밝은 환경광
	m_Ambient_Colors[ENUM_CLASS(DAY_PHASE::DUSK)] = { 0.2f, 0.2f, 0.3f, 1.0f }; // 환경광 감소
	m_Ambient_Colors[ENUM_CLASS(DAY_PHASE::NIGHT)] = { 0.1f, 0.1f, 0.2f, 1.0f }; // 최소한의 어두운 환경광

	// --- m_Specular_Colors 초기화 (반짝이는 하이라이트) ---
	m_Specular_Colors[ENUM_CLASS(DAY_PHASE::DAWN)] = { 0.7f, 0.7f, 0.7f, 1.0f };
	m_Specular_Colors[ENUM_CLASS(DAY_PHASE::DAY)] = { 1.0f, 1.0f, 1.0f, 1.0f }; // 가장 선명한 반사
	m_Specular_Colors[ENUM_CLASS(DAY_PHASE::DUSK)] = { 0.8f, 0.6f, 0.4f, 1.0f }; // 석양 색이 입혀진 반사
	m_Specular_Colors[ENUM_CLASS(DAY_PHASE::NIGHT)] = { 0.3f, 0.3f, 0.5f, 1.0f };


	return S_OK;
}

void CMainLight::Priority_Update(_float fTimeDelta)
{
	m_pGameTime->Update_GameTime(fTimeDelta);
}

void CMainLight::Update(_float fTimeDelta)
{
	_float fProgress = m_pGameTime->Get_DayTime() / SECONDS_PER_GAME_DAY;
	_float fStartProgress = {};
	
	if (0.25f > fProgress)
	{
		m_eCurPhase = DAY_PHASE::DAWN;
		m_eNextPhase = DAY_PHASE::DAY;
		fStartProgress = 0.f;
	}
	else if (0.25f <= fProgress && 0.5f > fProgress)
	{
		m_eCurPhase = DAY_PHASE::DAY;
		m_eNextPhase = DAY_PHASE::DUSK;
		fStartProgress = 0.25f;
	}
	else if (0.5f <= fProgress && 0.75f > fProgress)
	{
		m_eCurPhase = DAY_PHASE::DUSK;
		m_eNextPhase = DAY_PHASE::NIGHT;
		fStartProgress = 0.5f;
	}
	else if (0.75f <= fProgress)
	{
		m_eCurPhase = DAY_PHASE::NIGHT;
		m_eNextPhase = DAY_PHASE::DAWN;
		fStartProgress = 0.75f;
	}

	_float fRatio = (fProgress - fStartProgress) / 0.25f;

	XMStoreFloat4(&m_vCurDiffuse, XMVectorLerp(
		XMLoadFloat4(&m_Diffuse_Colors[ENUM_CLASS(m_eCurPhase)]),
		XMLoadFloat4(&m_Diffuse_Colors[ENUM_CLASS(m_eNextPhase)]), fRatio));

	XMStoreFloat4(&m_vAmbient, XMVectorLerp(
		XMLoadFloat4(&m_Ambient_Colors[ENUM_CLASS(m_eCurPhase)]),
		XMLoadFloat4(&m_Ambient_Colors[ENUM_CLASS(m_eNextPhase)]), fRatio));

	XMStoreFloat4(&m_vSpecular, XMVectorLerp(
		XMLoadFloat4(&m_Specular_Colors[ENUM_CLASS(m_eCurPhase)]),
		XMLoadFloat4(&m_Specular_Colors[ENUM_CLASS(m_eNextPhase)]), fRatio));

	//m_pLightCom->Set_Color(m_vCurDiffuse, m_vAmbient, m_vSpecular);

	Describe_Entity();
}

void CMainLight::Late_Update(_float fTimeDelta)
{
}

HRESULT CMainLight::Render()
{
	return S_OK;
}



HRESULT CMainLight::Ready_Components()
{
	__super::Ready_Components(nullptr);

	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.3f, 0.3f, 0.1f, 0.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.5f, 0.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
	LightDesc.pDirection = m_pTransformCom->Get_StatePtr(STATE::LOOK);
	LightDesc.iLevel = NEXT_LEVEL;

	/* Com_Light*/
	if (FAILED(Add_Component<CLight>(g_iStaticLevel, &m_pLightCom, &LightDesc)))
	{
		return E_FAIL;
	}

	m_pGameInstance->Add_Light(NEXT_LEVEL, m_pLightCom);

	_float4 vColor = _float4(0.1f, 0.1f, 0.15f, 0.5f);
	m_pGameInstance->Set_FogColor(vColor);

	return S_OK;
}

HRESULT CMainLight::Bind_ShaderResources()
{
	return S_OK;
}

CMainLight* CMainLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMainLight* pInstance = new CMainLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMainLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMainLight::Clone(void* pArg, CGameObject* pOwner)
{
	CMainLight* pInstance = new CMainLight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMainLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainLight::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLightCom);
	SAFE_RELEASE(m_pGameTime);
}

void CMainLight::Describe_Entity()
{
	GUI::Begin("MainLight");
	m_pLightCom->Describe_Entity();
	GUI::End();
}
