#include "pch.h"
#include "Light_Main.h"
#include "GameInstance.h"
#include "Layer.h"

CLight_Main::CLight_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLight_Main::CLight_Main(const CLight_Main& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CLight_Main::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLight_Main::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))){
		return E_FAIL;
	}
	if (FAILED(Ready_Components())){
		return E_FAIL;
	}

	return S_OK;
}

void CLight_Main::Priority_Update(_float fTimeDelta)
{
	_float4 vRPYQuat = {};
	XMStoreFloat4(&vRPYQuat, m_pTransformCom->Get_QuarternionVector());
	m_pGameInstance->Ready_Shadow_Light(vRPYQuat);
}

void CLight_Main::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG
}

void CLight_Main::Late_Update(_float fTimeDelta)
{
}

HRESULT CLight_Main::Render()
{
	return S_OK;
}

HRESULT CLight_Main::Capture_PreShadow()
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
		// 외곽이 더 많아지면 많아질수록 더 정밀한 경계가 가능
		// 하지만 더 실제보다 멀리 외곽을 두르면 필요한 것보다 더 많이 찍게 됨
		// 결국 딱 맞춰서 찍는게 제일 중요함
		vNeededPos.emplace_back(_float3(-112.f, 155.f, 362.f));
		vNeededPos.emplace_back(_float3(- 188.f, 10.f, 476.f));
		vNeededPos.emplace_back(_float3(- 571.f, 19.f, 476.f));
		vNeededPos.emplace_back(_float3(- 326.f, -64.f, 480.f));
		vNeededPos.emplace_back(_float3(569.f, 77.f, 250.f));
		vNeededPos.emplace_back(_float3(320.f, -236.f, 360.f));
		vNeededPos.emplace_back(_float3(570.f, 277.f, -277.f));
		vNeededPos.emplace_back(_float3(570.f, 176.f, -527.f));
		vNeededPos.emplace_back(_float3(- 108.f, -64.f, -1146.f));
		vNeededPos.emplace_back(_float3(- 824.f, 128.f, -533.f));
		vNeededPos.emplace_back(_float3(- 826.f, 145.f, -150.f));
		vNeededPos.emplace_back(_float3(- 256.f, -92.f, -414.f));
	}
	_matrix xmViewMatrix = m_pTransformCom->Get_WorldMatrixInv();
	_float4x4 ViewMatrix = {}; XMStoreFloat4x4(&ViewMatrix, xmViewMatrix);
	_float4x4 ProjMatrix = {}; XMStoreFloat4x4(&ProjMatrix, Get_OffCenterProjMatrix(xmViewMatrix, vNeededPos));

	m_pGameInstance->Render_PreShadow(ViewMatrix, ProjMatrix);
	return S_OK;
}

HRESULT CLight_Main::Ready_Components()
{
	__super::Ready_Components(nullptr);

	m_pTransformCom->LookAt(XMVectorSet(1.f, -4.f, 1.f, 1.f));
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	//LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 0.f);
	//LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 0.f);
	//LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
	LightDesc.vDiffuse = _float4(0.3f, 0.3f, 0.1f, 0.f);
	LightDesc.vAmbient = _float4(0.12f, 0.12f, 0.24f, 0.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
	LightDesc.pDirection = m_pTransformCom->Get_StatePtr(STATE::LOOK);
	LightDesc.iLevel = NEXT_LEVEL;

	/* Com_Light*/
	if (FAILED(Add_Component<CLight>(g_iStaticLevel, &m_pLightCom, &LightDesc)))
	{
		return E_FAIL;
	}
	m_pGameInstance->Add_Light(NEXT_LEVEL, m_pLightCom);

	_float4 vColor = _float4(0.7f, 0.7f, 0.7f, 0.3f);
	m_pGameInstance->Set_FogColor(vColor);
	m_pGameInstance->Set_Fog(5.f, 30.f);

	return S_OK;
}

HRESULT CLight_Main::Bind_ShaderResources()
{
	return S_OK;
}

_matrix CLight_Main::Get_OffCenterProjMatrix(_fmatrix ViewMatrix, vector<_float3>& WorldPositions)
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


CLight_Main* CLight_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLight_Main* pInstance = new CLight_Main(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLight_Main");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLight_Main::Clone(void* pArg, CGameObject* pOwner)
{
	CLight_Main* pInstance = new CLight_Main(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLight_Main");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight_Main::Free()
{
	__super::Free();

	Safe_Release(m_pLightCom);
}
#ifdef _DEBUG

void CLight_Main::Describe_Entity()
{
	GUI::Begin("LIGHT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	if (GUI::CollapsingHeader("Main_Light")) {
		if (GUI::Button("Capture_PreShadow")) {
			if (FAILED(Capture_PreShadow())) {
				assert(false);
			}
		}
		m_pLightCom->Describe_Entity();
		if (GUI::Button("Sync_CamToLight")) {
			m_pTransformCom->Set_WorldMatrix(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW_INV));
			m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		}
	}
	GUI::End();
}

#endif // _DEBUG
