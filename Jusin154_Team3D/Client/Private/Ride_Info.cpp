#include "pch.h"
#include "Ride_Info.h"
#include "GameInstance.h"

CRide_Info::CRide_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CRide_Info::CRide_Info(const CRide_Info& rhs)
	:CElementObject(rhs)
{
}

HRESULT CRide_Info::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRide_Info::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = -0.f;
	Desc.fSizeX = 256.f;
	Desc.fSizeY = 128.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_fTimeMult = 1.f;
	m_fAlpha = 1.f;
	m_fAlphaTime = 10.f;
	m_vNine_Slice = _float4(256.f, 500.f, 80.f, 150.f);
	SizeUpX(500.f);
	SizeUpY(400.f);
	MoveX(210);
	MoveY(20);
	Set_Posi();
	m_bActive = false;
	return S_OK;
}

void CRide_Info::Set_Posi()
{
	m_vImagePosi1 = _float4(65.f, 70.f, 32.f, 32.f);
	m_vImagePosi2 = _float4(50.f, 137.5f, 32.f, 32.f);
	m_vImagePosi3 = _float4(50.f, 192.5f, 32.f, 32.f);
	m_vImagePosi4 = _float4(90.f, 192.5f, 32.f, 32.f);
}

void CRide_Info::Set_Hover(_bool bVisible)
{
	Visible(bVisible);
	m_bHover = true;
	m_fTime = 0.f;
	m_fAlpha = 1.f;
}

void CRide_Info::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CRide_Info::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_bFadeIn == true)
	{
		if (m_fAlpha <= 1.f)
			m_fAlpha += fTimeDelta * m_fAlphaTime;

		if (m_fAlpha >= 1.f)
		{
			m_bFadeIn = false;
			m_fAlpha = 1.f;
		}
	}

	if (m_bFadeOut == true)
	{
		if (m_fAlpha >= 0.f)
			m_fAlpha -= fTimeDelta;

		if (m_fAlpha <= 0.f)
		{
			m_bFadeOut = false;
			m_fAlpha = 0.f;
			Visible(false);
		}
	}

	if (m_bHover == true)
		m_fTime += fTimeDelta * m_fTimeMult;

	if (m_fTime >= 1.f)
	{
		Set_FadeOut();
	}

	__super::Update(fTimeDelta);
}

void CRide_Info::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
		__super::Late_Update(fTimeDelta);
	}
}

HRESULT CRide_Info::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::RIDE_INFO)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	m_pGameInstance->Render_Text(TEXT("Font_size15"), TEXT("빗자루 컨트롤"), _float2(1480.f + m_fX, 585.f + m_fY), XMVectorSet((208.f / 255.f) * m_fAlpha, (177.f / 255.f) * m_fAlpha, (52.f / 255.f) * m_fAlpha, m_fAlpha));
	m_pGameInstance->Render_Text(TEXT("Font_size15"), TEXT("스피드 버스트"), _float2(1500.f + m_fX, 635.f + m_fY), XMVectorSet(1.f * m_fAlpha, 1.f * m_fAlpha, 1.f * m_fAlpha, m_fAlpha));
	m_pGameInstance->Render_Text(TEXT("Font_size15"), TEXT("(전환)비행 가속"), _float2(1490.f + m_fX, 700.f + m_fY), XMVectorSet(1.f * m_fAlpha, 1.f * m_fAlpha, 1.f * m_fAlpha, m_fAlpha));
	m_pGameInstance->Render_Text(TEXT("Font_size15"), TEXT("하강 / 상승"), _float2(1525.f + m_fX, 755.f + m_fY), XMVectorSet(1.f * m_fAlpha, 1.f * m_fAlpha, 1.f * m_fAlpha, m_fAlpha));

	return S_OK;
}

_vector CRide_Info::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}


HRESULT CRide_Info::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	{
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
	if (FAILED(m_pDiffuse_TextureCom2->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom3->Bind_ShaderResource(m_pShaderCom, "g_Texture3", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom4->Bind_ShaderResource(m_pShaderCom, "g_Texture4", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOwnerAlpha", &m_fOwnerAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCanvasAlpha", &m_fCanvasAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOrigin_Size", &m_fOrigin_Size, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_vScale, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNine_Slice", &m_vNine_Slice, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos1", &m_vImagePosi1, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos2", &m_vImagePosi2, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos3", &m_vImagePosi3, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos4", &m_vImagePosi4, sizeof(_float4))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CRide_Info::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_MissionBanner_BorderOrnament_Left"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Mouse_LeftClick"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Keyboard_Shift"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom2), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Keyboard_Control"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom3), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Keyboard_Space"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom4), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CRide_Info* CRide_Info::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRide_Info* pInstance = new CRide_Info(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRide_Info");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CRide_Info::Clone(void* pArg, CGameObject* pOwner)
{
	CRide_Info* pInstance = new CRide_Info(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroom_Finish");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRide_Info::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom2);
	SAFE_RELEASE(m_pDiffuse_TextureCom3);
	SAFE_RELEASE(m_pDiffuse_TextureCom4);
	SAFE_RELEASE(m_pVIBufferCom);
	SAFE_RELEASE(m_pShaderCom);
}

#ifdef _DEBUG
void CRide_Info::Describe_Entity()
{
}
#endif // _DEBUG
