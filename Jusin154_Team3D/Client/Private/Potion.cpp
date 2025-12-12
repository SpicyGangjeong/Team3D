#include "pch.h"
#include "Potion.h"
#include "GameInstance.h"
#include "InfoInstance.h"

CPotion::CPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CPotion::CPotion(const CPotion& rhs)
	:CElementObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CPotion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPotion::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 700.f;
	Desc.fY = 150.f;
	Desc.fSizeX = 80.f;
	Desc.fSizeY = 80.f;

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
	m_fAlphaTime = 1.f;
	m_vImageSlotPos = _float2(45.f, 45.f);
	m_vImageSize = _float2(32.f, 32.f);
	m_fFontX = 1163.f;
	m_fFontY = 860.f;
	m_iPotionIndex = 10;
	m_iPerPotionIndex = -1;
	m_iPotion_Level = 1;
	return S_OK;
}

_float CPotion::Use_Potion()
{
	if (m_iPotionIndex == 0)
		return -1.f;

	m_iPotionIndex--;

	return floor(m_pInfoInstance->Get_PlayerStatPtr()->Get_Stat().fMaxHp * (m_iPotion_Level * 0.25f));
}

void CPotion::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}


void CPotion::Update(_float fTimeDelta)
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

	if (m_iPerPotionIndex != m_iPotionIndex)
	{
		m_strPotion = to_wstring(m_iPotionIndex);
		m_fFontOffSet = (m_pGameInstance->FontSizeX(TEXT("UI_size15"), m_strPotion.c_str()) - 13.f) * 0.5f;
		m_iPerPotionIndex = m_iPotionIndex;
	}

	m_fTime += fTimeDelta * m_fTimeMult;

	__super::Update(fTimeDelta);
}

void CPotion::Late_Update(_float fTimeDelta)
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

HRESULT CPotion::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::POTION)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}
	m_pGameInstance->Render_Text(TEXT("UI_size15"), m_strPotion.c_str(), _float2((m_fFontX + m_fX) - m_fFontOffSet, m_fFontY + m_fY), XMVectorSet((208.f / 255.f) * m_fAlpha, (177.f / 255.f) * m_fAlpha, (52.f / 255.f) * m_fAlpha, m_fAlpha));

	return S_OK;
}

_vector CPotion::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CPotion::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_vScale, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Pos", &m_vImageSlotPos, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOrigin_Size", &m_vImageSize, sizeof(_float2))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPotion::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_ActionItemBack_4K"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_FG_IndexIconBorder"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_ActionItemGoldleaf_4K"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom2), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_WoundCleaning"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom3), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_AmountFrame"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom4), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CPotion* CPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPotion* pInstance = new CPotion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPotion");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CPotion::Clone(void* pArg, CGameObject* pOwner)
{
	CPotion* pInstance = new CPotion(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Image");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CPotion::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom2);
	SAFE_RELEASE(m_pDiffuse_TextureCom3);
	SAFE_RELEASE(m_pDiffuse_TextureCom4);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CPotion::Describe_Entity()
{
}
#endif // _DEBUG
