#include "pch.h"
#include "MainLight.h"

#include "GameInstance.h"
#include "Camera_Debug.h"
#include "GameTime.h"
#include "Layer.h"
#include "GameInstance.h"

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

	m_pTransformCom->Set_State(STATE::LOOK, XMVectorSet(-1.716f, 0.121f, 0.211f, 0.f));

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
	_float4 vLook = {};
	XMStoreFloat4(&vLook, m_pTransformCom->Get_State(STATE::LOOK));
	m_pGameInstance->Ready_Shadow_Light(vLook);
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

HRESULT CMainLight::Capture_PreShadow()
{
	CLayer* pLayer = m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_BACKGROUND);
	assert(nullptr != pLayer);

	const list<CGameObject*>* pObjects = pLayer->Get_Objects();
	for (auto& pObject : *pObjects) {
		if (FAILED(m_pGameInstance->Add_RenderGroup(RENDER::PRESHADOW, pObject))) {
			assert(false);
		}
	}
	vector<_float3> vNeededPos = {};
	vNeededPos.reserve(12);
	{
		// 외곽이 더 많아지면 많아질수록 더 정밀한 경계가 가능함
		// 하지만 더 실제보다 멀리 외곽을 두르면 필요한 것보다 더 많이 찍게 됨
		// 결국 딱 맞춰서 찍는게 제일 중요함
		vNeededPos.emplace_back(_float3(-112.f, 155.f, 362.f));
		vNeededPos.emplace_back(_float3(-188.f, 10.f, 476.f));
		vNeededPos.emplace_back(_float3(-571.f, 19.f, 476.f));
		vNeededPos.emplace_back(_float3(-326.f, -64.f, 480.f));
		vNeededPos.emplace_back(_float3(569.f, 77.f, 250.f));
		vNeededPos.emplace_back(_float3(320.f, -236.f, 360.f));
		vNeededPos.emplace_back(_float3(570.f, 277.f, -277.f));
		vNeededPos.emplace_back(_float3(570.f, 176.f, -527.f));
		vNeededPos.emplace_back(_float3(-108.f, -64.f, -1146.f));
		vNeededPos.emplace_back(_float3(-824.f, 128.f, -533.f));
		vNeededPos.emplace_back(_float3(-826.f, 145.f, -150.f));
		vNeededPos.emplace_back(_float3(-256.f, -92.f, -414.f));
	}
	_matrix xmViewMatrix = m_pTransformCom->Get_WorldMatrixInv();
	_float4x4 ViewMatrix = {}; XMStoreFloat4x4(&ViewMatrix, xmViewMatrix);
	_float4x4 ProjMatrix = {}; XMStoreFloat4x4(&ProjMatrix, Get_OffCenterProjMatrix(xmViewMatrix, vNeededPos));

	m_pGameInstance->Render_PreShadow(ViewMatrix, ProjMatrix);

	m_pGameInstance->Update_Volumetric();
	return S_OK;
}

_matrix CMainLight::Get_OffCenterProjMatrix(_fmatrix ViewMatrix, vector<_float3>& WorldPositions)
{
	_float fMinX = FLT_MAX;
	_float fMinY = FLT_MAX;
	_float fMinZ = FLT_MAX;
	_float fMaxX = -FLT_MAX;
	_float fMaxY = -FLT_MAX;
	_float fMaxZ = -FLT_MAX;

	_uint iPosNum = (_uint)WorldPositions.size();
	for (_uint iIndexPoint = 0; iIndexPoint < iPosNum; ++iIndexPoint)
	{
		_vector vLightViewPos = XMVector4Transform(XMVectorSetW(XMLoadFloat3(&WorldPositions[iIndexPoint]), 1.f), ViewMatrix);

		_float x = XMVectorGetX(vLightViewPos);
		_float y = XMVectorGetY(vLightViewPos);
		_float z = XMVectorGetZ(vLightViewPos);

		fMinX = min(fMinX, x);  fMaxX = max(fMaxX, x);
		fMinY = min(fMinY, y);  fMaxY = max(fMaxY, y);
		fMinZ = min(fMinZ, z);  fMaxZ = max(fMaxZ, z);
	}
	_float fShadowDepthMArgin = 10.f; // 캐릭터 이동 보정
	fMinZ -= fShadowDepthMArgin;
	fMaxZ += fShadowDepthMArgin;
	return XMMatrixOrthographicOffCenterLH(fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ);
}

HRESULT CMainLight::Ready_Components()
{
	__super::Ready_Components(nullptr);

	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.361f, 0.451f, 0.451f, 0.204f);
	LightDesc.vAmbient = _float4(0.161f, 0.161f, 0.161f, 0.0f);
	LightDesc.vSpecular = _float4(0.05f, 0.05f, 0.05f, 0.f);
	m_pTransformCom->Rotation(XMConvertToRadians(-4.f), XMConvertToRadians(271.f), 0.f);
	LightDesc.pDirection = m_pTransformCom->Get_StatePtr(STATE::LOOK);
	LightDesc.iLevel = NEXT_LEVEL;

#ifdef gimch
	//LightDesc.vDiffuse = _float4(0.361f, 0.451f, 0.451f, 0.204f);
	//LightDesc.vAmbient = _float4(0.161f, 0.161f, 0.161f, 0.0f);
	LightDesc.vDiffuse = _float4(0.361f, 0.451f, 0.451f, 0.204f);
	LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 0.0f);

#endif // gimch

	/* Com_Light*/
	if (FAILED(Add_Component<CLight>(g_iStaticLevel, &m_pLightCom, &LightDesc)))
	{
		return E_FAIL;
	}

	m_pGameInstance->Add_Light(NEXT_LEVEL, m_pLightCom);

	_float4 vColor = _float4(0.7f, 0.7f, 0.7f, 0.3f);
	m_pGameInstance->Set_FogColor(vColor);
	m_pGameInstance->Set_Fog(5.f, 30.f);
	
#ifdef gimch
	vColor = _float4(0.2f, 0.246f, 0.256f, 1.f);
	m_pGameInstance->Set_FogColor(vColor);
	m_pGameInstance->Set_Fog(10.f, 10.f);

#endif // gimch

	

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
