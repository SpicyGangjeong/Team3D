#include "pch.h"
#include "Spell_UI.h"
#include "GameInstance.h"

CSpell_UI::CSpell_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSpell_UI::CSpell_UI(const CSpell_UI& rhs)
	:CElementObject(rhs)
{
}

HRESULT CSpell_UI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpell_UI::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 650.f;
	Desc.fY = -60.f;
	Desc.fSizeX = 180.f;
	Desc.fSizeY = 50.f;

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
	//m_vImageUV[0] = Image_UV(0);
	//m_vImageUV[1] = Alphabet_UV('R');
	//m_vImageUV[2] = Image_UV(1);
	//m_vImageUV[3] = Alphabet_UV('T');
	Set_ImagePos();
	m_bActive = true;
	return S_OK;
}

_float4 CSpell_UI::Alphabet_UV(_tchar Alphabet)
{
	_uint Number = CMyTools::AlphabetToInt(Alphabet);

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

	_float4 UV{};

	return UV = _float4(UVStart.x, UVStart.y, UVEnd.x, UVEnd.y);
}

_float4 CSpell_UI::Image_UV(_uint Index)
{
	_float2 fIamge_Size = { 256.f, 128.f };

	_uint iXCount = 2;
	_uint iYCount = 1;

	_float frameWidth = 128.f;
	_float frameHeight = 128.f;

	_uint frameX = Index % iXCount;
	_uint frameY = Index / iXCount;

	_float2 UVStart;
	UVStart.x = frameX * frameWidth / fIamge_Size.x;
	UVStart.y = frameY * frameHeight / fIamge_Size.y;

	_float2 UVEnd;
	UVEnd.x = UVStart.x + (frameWidth / fIamge_Size.x);
	UVEnd.y = UVStart.y + (frameHeight / fIamge_Size.y);

	_float4 UV{};

	return UV = _float4(UVStart.x, UVStart.y, UVEnd.x, UVEnd.y);
}

void CSpell_UI::Set_ImagePos()
{
	m_vUV[0] = _float2(16,25);
	m_vUV[1] = _float2(42,25);
	m_vUV[2] = _float2(138,25);
	m_vUV[3] = _float2(164,25);
}

void CSpell_UI::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpell_UI::Update(_float fTimeDelta)
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

	__super::Update(fTimeDelta);
}

void CSpell_UI::Late_Update(_float fTimeDelta)
{
	if (__super::Chack_Visible())
	{
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
		__super::Late_Update(fTimeDelta);
	}
}

HRESULT CSpell_UI::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::DEFAULT)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	return S_OK;
}

_vector CSpell_UI::Get_WorldPostion()
{
	return m_pOwner->Get_WorldPostion();
}

HRESULT CSpell_UI::Bind_ShaderResources()
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
	//if (FAILED(m_pDiffuse_TextureCom1->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
	//{
	//	return E_FAIL;
	//}
	//if (FAILED(m_pDiffuse_TextureCom2->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
	//{
	//	return E_FAIL;
	//}
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
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fPosition", m_vUV, sizeof(m_vUV))))
	//{
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fImagesUV", m_vImageUV, sizeof(m_vImageUV))))
	//{
	//	return E_FAIL;
	//}
	return S_OK;
}

HRESULT CSpell_UI::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Revelio_SpellSelectionGridIcon"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Revelio_SpellSelectionGridIcon"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Atlas_Keyboard"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom2), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSpell_UI* CSpell_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpell_UI* pInstance = new CSpell_UI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpell_UI");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpell_UI::Clone(void* pArg, CGameObject* pOwner)
{
	CSpell_UI* pInstance = new CSpell_UI(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_UI");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpell_UI::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom2);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CSpell_UI::Describe_Entity()
{
}
