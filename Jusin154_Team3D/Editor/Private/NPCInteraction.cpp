#include "pch.h"
#include "NPCInteraction.h"
#include "GameInstance.h"

CNPCInteraction::CNPCInteraction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CNPCInteraction::CNPCInteraction(const CNPCInteraction& rhs)
	:CElementObject(rhs)
{
}

HRESULT CNPCInteraction::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPCInteraction::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 630.f;
	Desc.fY = -125.f;
	Desc.fSizeX = 256.f;
	Desc.fSizeY = 96.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_fTimeMult = 3.f;
	m_fAlpha = 1.f;
	m_fAlphaTime = 3.f;
	m_fFontX = 1090.f;
	m_fFontY = 770.f;
	m_fFontSize.x = 1100;
	m_fFontSize.y = 830;
	m_vNine_Slice = _float4(160.f, 280.f, 48.f, 48.f);
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
	m_vImagePosi1 = _float4(46.f, 32.f, 74.f, 74.f);
}

void CNPCInteraction::SizeUpX(_float fSizeX)
{
	_float fX = (fSizeX - m_vScale.x) * 0.5f;
	m_fSizeX = fSizeX;
	m_fX += fX;
	m_fFontX -= fX;
	m_fFontSize.x -= fX;
}

void CNPCInteraction::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CNPCInteraction::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	m_fTime += fTimeDelta * m_fTimeMult;

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
	Set_ImageSizePosition();
	__super::Update(fTimeDelta);
}

void CNPCInteraction::Late_Update(_float fTimeDelta)
{
	if (__super::Chack_Visible())
	{
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
		__super::Late_Update(fTimeDelta);
	}
}

HRESULT CNPCInteraction::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::NPCINTERACTION)))) {
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

	m_pGameInstance->Render_Text(TEXT("Font_size20"), TEXT("가나다라 마바사"), _float2(m_fFontX + m_fX, m_fFontY - m_fY));
	m_pGameInstance->Render_Text(TEXT("Font_size15"), TEXT("대화하기"), _float2(m_fFontSize.x + m_fX, m_fFontSize.y - m_fY));

	return S_OK;
}

_vector CNPCInteraction::Get_WorldPostion()
{
	return m_pOwner->Get_WorldPostion();
}

HRESULT CNPCInteraction::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float))))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageUV", &m_vUV, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos1", &m_vImagePosi1, sizeof(_float4))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CNPCInteraction::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_InteractBlipDivider"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_Atlas_Keyboard"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
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

void CNPCInteraction::Describe_Entity()
{
}
