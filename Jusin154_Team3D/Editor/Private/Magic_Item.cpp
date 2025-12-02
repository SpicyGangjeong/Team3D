#include "pch.h"
#include "Magic_Item.h"
#include "GameInstance.h"

CMagic_Item::CMagic_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CMagic_Item::CMagic_Item(const CMagic_Item& rhs)
	:CElementObject(rhs)
{
}

HRESULT CMagic_Item::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMagic_Item::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = -800.f;
	Desc.fY = 70.f;
	Desc.fSizeX = 100.f;
	Desc.fSizeY = 100.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_fAlpha = 1.f;
	m_fTimeMult = 0.5f;
	m_fAngle = XMConvertToRadians(-135.f);
	m_fAlphaTime = 1.f;
	m_fSortZ = 0.2f;
	m_fCoolTime = 5.f;
	m_vUVScale.y = 1.f;
	m_iArratCount = 2;
	Compute_UV(0);
	Compute_Image();
	return S_OK;
}

void CMagic_Item::Compute_UV(_uint iItemID)
{
	_float2 fImage_Size = { 1024.f, 512.f };

	_uint iCountX = 4;
	_uint iCountY = 2;

	_float iImageX = 256.f;
	_float iImageY = 256.f;

	_uint iframeX = iItemID % iCountX;
	_uint iframeY = iItemID / iCountX;

	_float2 UVStart{};
	UVStart.x = iframeX * iImageX / fImage_Size.x;
	UVStart.y = iframeY * iImageY / fImage_Size.y;

	_float2 UVEnd{};
	UVEnd.x = UVStart.x + (iImageX / fImage_Size.x);
	UVEnd.y = UVStart.y + (iImageY / fImage_Size.y);

	_float4 UV = _float4{ UVStart.x, UVStart.y, UVEnd.x, UVEnd.y };
	m_vUV = UV;
}

void CMagic_Item::Compute_Image()
{
	m_vImagePos1 = _float2(72.f, 0.f);
	m_vImageSize1 = _float2(30.f, 30.f);
	m_vImagePos2 = _float2(65.f, 65.f);
	m_vImageSize2 = _float2(30.f, 30.f);
}

void CMagic_Item::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}


void CMagic_Item::Update(_float fTimeDelta)
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
		}
	}

	if (m_vUVScale.y >= 1.f)
	{
		if (m_pGameInstance->Key_Down(DIK_TAB))
		{
			m_vUVScale.y = 0.f;
		}
	}

	if (m_vUVScale.y <= 1)
	{
		m_vUVScale.y += fTimeDelta * (1.f / m_fCoolTime);
	}

	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);
}

void CMagic_Item::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CMagic_Item::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::MAGIC_ITEM))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

_vector CMagic_Item::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CMagic_Item::Bind_ShaderResources()
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
	if (FAILED(m_pDiffuse_TextureCom5->Bind_ShaderResource(m_pShaderCom, "g_Texture5", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom6->Bind_ShaderResource(m_pShaderCom, "g_MaskingTexture", 0)))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAngle", &m_fAngle, sizeof(_float))))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageUV", &m_vUV, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCoolTime", &m_fCoolTime, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDeltaV", &m_vUVScale.y, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageUV", &m_vUV, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iArrayCount", &m_iArratCount, sizeof(_uint))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fItemPosition1", &m_vImagePos1, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fItemImageSizes1", &m_vImageSize1, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fItemPosition2", &m_vImagePos2, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fItemImageSizes2", &m_vImageSize2, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_vScale, sizeof(_float2))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMagic_Item::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_ActionItemBack_4K"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_spellmeter_Generic"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_ActionItemGoldleaf_4K"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom2), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_Atlas_Item"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom3), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_Keyboard_Tab"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom4), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_AmountFrame"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom5), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_tillingSmokes"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom6), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CMagic_Item* CMagic_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMagic_Item* pInstance = new CMagic_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMagic_Item");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMagic_Item::Clone(void* pArg, CGameObject* pOwner)
{
	CMagic_Item* pInstance = new CMagic_Item(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMagic_Item");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMagic_Item::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom2);
	SAFE_RELEASE(m_pDiffuse_TextureCom3);
	SAFE_RELEASE(m_pDiffuse_TextureCom4);
	SAFE_RELEASE(m_pDiffuse_TextureCom5);
	SAFE_RELEASE(m_pDiffuse_TextureCom6);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CMagic_Item::Describe_Entity()
{
}
