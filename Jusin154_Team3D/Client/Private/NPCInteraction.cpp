#include "pch.h"
#include "NPCInteraction.h"
#include "GameInstance.h"
#include "Unit.h"
#include "InfoInstance.h"

CNPCInteraction::CNPCInteraction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CNPCInteraction::CNPCInteraction(const CNPCInteraction& rhs)
	: CGameObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CNPCInteraction::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPCInteraction::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}


	m_fOrigin_Size = _float3(256.f, 64.f, 1.f);
	m_fSize.x = m_fOrigin_Size.x;
	m_fSize.y = m_fOrigin_Size.y;
	m_fSize.z = m_fOrigin_Size.z;

	m_fViewPort = _float2(_float(g_iWinSizeX), _float(g_iWinSizeY));


	//m_fX = 630.f;
	//m_fY = -125.f;
	m_fAlpha = 1.f;
	m_vNine_Slice = _float4(160.f, 256.f, 32.f, 32.f);
	m_fFontSize = 0.f;
	Set_ImageSizePosition();
	m_pInfoInstance->Add_Event(TEXT("NPCInteractionOn"), [this](void* p) {this->NpcInfo(p); });
	m_pInfoInstance->Add_Event(TEXT("NPCInteractionOff"), [this](void* p) {this->ActiveOff(); });
	return S_OK;
}

_float4 CNPCInteraction::ComputeUV(_tchar Alphabat)
{
	_uint Number = CMyTools::AlphabetToInt(Alphabat);

	_float2 fIamge_Size = { 320.f, 384.f };

	_uint iXCount = 5;
	_uint iYCount = 6;

	_float frameWidth = 64.f;
	_float frameHeight = 64.f;

	_uint frameX = Number % iXCount;
	_uint frameY = Number / iXCount;

	_float2 UVStart;
	UVStart.x = frameX * frameWidth / fIamge_Size.x;
	UVStart.y = frameY * frameHeight / fIamge_Size.y;

	_float2 UVEnd;
	UVEnd.x = UVStart.x + (frameWidth / fIamge_Size.x);
	UVEnd.y = UVStart.y + (frameHeight / fIamge_Size.y);


	return _float4(UVStart.x, UVStart.y, UVEnd.x, UVEnd.y);
}

void CNPCInteraction::Set_ImageSizePosition()
{
	m_vUV = ComputeUV('F');
	m_vImagePosi1 = _float4(25.f, 15.f, 35.f, 35.f);
}

void CNPCInteraction::ActiveOff()
{
	m_bActive = false;
	m_pInfo.pOwner = nullptr;
}

void CNPCInteraction::NpcInfo(void* pArg)
{
	NPCINTERACTIONINFO* Info = reinterpret_cast<NPCINTERACTIONINFO*>(pArg);

	m_bActive = true;

	m_pInfo.pOwner = Info->pOwner;
	m_pInfo.pNPCName = Info->pNPCName;
	m_pInfo.pNPCPosition = Info->pNPCPosition;

	_vector MonsterPos = static_cast<CUnit*>(Info->pOwner)->Get_WorldPostion();
	_vector HeadPos = MonsterPos + XMVectorSet(0.5f, 2.f, 0.f, 0.f);
	XMStoreFloat4(&m_pInfo.pNPCPosition, HeadPos);
}

void CNPCInteraction::Priority_Update(_float fTimeDelta)
{
	if (m_pInfo.pOwner != nullptr)
	{
		_vector MonsterPos = static_cast<CUnit*>(m_pInfo.pOwner)->Get_WorldPostion();
		_vector HeadPos = XMVectorAdd(MonsterPos, XMVectorSet(0.5f, 2.f, 0.f, 0.f));
		XMStoreFloat4(&m_pInfo.pNPCPosition, HeadPos);

		_vector ScreenPos = XMVector3Project(
			HeadPos,
			0.f, 0.f,
			g_iWinSizeX, g_iWinSizeY,
			0.f, 1.f,
			m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ),
			m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW),
			XMMatrixIdentity()
		);

		ScreenPos = ScreenPos + XMVectorSet(m_fFontSize, -50.f, 0.f, 0.f);

		_vector WorldOffsetPos = XMVector3Unproject(
			ScreenPos,
			0.f, 0.f,
			g_iWinSizeX, g_iWinSizeY,
			0.f, 1.f,
			m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ),
			m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW),
			XMMatrixIdentity()
		);

		XMStoreFloat4(&m_vFontPosition, WorldOffsetPos);
		m_fSize = _float3(m_fSize.x, m_fOrigin_Size.y, m_fOrigin_Size.z);
		m_fFontSize = 0;
		m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_pInfo.pNPCPosition));
	}

}

void CNPCInteraction::Update(_float fTimeDelta)
{
	if (m_bVisible != true)
		return;
}

void CNPCInteraction::Late_Update(_float fTimeDelta)
{
	if (m_bVisible != true)
		return;

	if (m_bActive == true)
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CNPCInteraction::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::INTERACTION_NPC)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	// 5글자 까지는 256
	// 6글자 300
	// 7글자 344
	// 8글자 : 가나다라 마바사아 , 388
	// 대충 44씩 늘어날듯

	m_pGameInstance->Perspective_Render_Text(
		m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW),
		m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ),
		TEXT("Font_size20"),
		m_pInfo.pNPCName.c_str(),
		XMLoadFloat4(&m_vFontPosition));

	m_pGameInstance->Perspective_Render_Text(
		m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW),
		m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ),
		TEXT("Font_size15"),
		TEXT("대화하기"),
		XMLoadFloat4(&m_pInfo.pNPCPosition));


	return S_OK;
}

HRESULT CNPCInteraction::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamaraPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float3))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom1->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOrigin_Size", &m_fOrigin_Size, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_fSize, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNine_Slice", &m_vNine_Slice, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageUV", &m_vUV, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos1", &m_vImagePosi1, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fViewport", &m_fViewPort, sizeof(_float2))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CNPCInteraction::Ready_Components()
{
	__super::Ready_Components(nullptr);

	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_InteractBlipDivider"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Atlas_Keyboard"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CNPCInteraction* CNPCInteraction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNPCInteraction* pInstance = new CNPCInteraction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNPCInteraction");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CNPCInteraction::Clone(void* pArg, CGameObject* pOwner)
{
	CNPCInteraction* pInstance = new CNPCInteraction(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNPCInteraction");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CNPCInteraction::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom2);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}
#ifdef _DEBUG
void CNPCInteraction::Describe_Entity()
{
}
#endif // DEBUG


