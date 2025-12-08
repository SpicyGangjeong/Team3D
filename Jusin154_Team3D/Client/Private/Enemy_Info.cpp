#include "pch.h"
#include "Enemy_Info.h"
#include "GameInstance.h"

CEnemy_Info::CEnemy_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CEnemy_Info::CEnemy_Info(const CEnemy_Info& rhs)
	:CElementObject(rhs)
{
}

HRESULT CEnemy_Info::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnemy_Info::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = -180.f;
	Desc.fY = 40.f;
	Desc.fSizeX = 50.f;
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

	m_fTimeMult = 1.f;
	m_fAlpha = 0.f;
	m_fAlphaTime = 10.f;
	m_fFontX = 952.f;
	m_fFontY = 39.f;
	m_fFontPos = _float2(1115.f, 0.f);
	m_fFont2OffSet = 0.f;
	m_pEnemy_Name = TEXT("나는 문어");
	m_pLevel = TEXT("99");
	m_bAnimation = false;
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("Enemy_HpDie"), [this](void* p) {this->Set_FadeOut(); });
	return S_OK;
}

void CEnemy_Info::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CEnemy_Info::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_bFadeIn == true)
	{
		if (m_fAlpha <= 1.f)
		{
			m_fAlpha += fTimeDelta * m_fAlphaTime;
			m_bHover = true;
		}

		if (m_fAlpha >= 1.f)
		{
			m_bFadeIn = false;
			m_fAlpha = 1.f;
		}
	}

	if (m_bFadeOut == true)
	{
		if (m_fAlpha >= 0.f)
			m_fAlpha -= fTimeDelta * m_fAlphaTime;

		if (m_fAlpha <= 0.f)
		{
			m_bFadeOut = false;
			m_bHover = false;
			m_fAlpha = 0.f;
		}
	}

	if (m_pGameInstance->Key_Down(DIK_K))
	{
		Set_FadeIn();
	}
	if (m_pGameInstance->Key_Down(DIK_L))
	{
		Set_FadeOut();
	}

	__super::Update(fTimeDelta);
}

void CEnemy_Info::Late_Update(_float fTimeDelta)
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

HRESULT CEnemy_Info::Render()
{
	if (m_bHover == true)
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

		m_fFontOffSet = (m_pGameInstance->FontSizeX(TEXT("Font_size20"), m_pEnemy_Name.c_str()) - 29.f) * 0.5f;
		m_fFont2OffSet = (m_pGameInstance->FontSizeX(TEXT("UI_size15"), m_pLevel.c_str()) - 13.f) * 0.5f;

		m_pGameInstance->Render_Text(TEXT("Font_size20"), m_pEnemy_Name.c_str(), _float2(m_fFontPos.x + m_fX - m_fFontOffSet, m_fFontPos.y + m_fY), XMVectorSet(1.f* m_fAlpha,1.f* m_fAlpha,1.f* m_fAlpha, m_fAlpha));
		m_pGameInstance->Render_Text(TEXT("UI_size15"), m_pLevel.c_str(), _float2(m_fFontX + m_fX - m_fFont2OffSet, m_fFontY + m_fY), XMVectorSet(0.f, (120.f / 255.f)* m_fAlpha, 0.f, m_fAlpha));
	}
	
	return S_OK;
}

_vector CEnemy_Info::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CEnemy_Info::Bind_ShaderResources()
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
	return S_OK;
}

HRESULT CEnemy_Info::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_HealthMeter_LevelBox"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CEnemy_Info::Set_Info(_int Level, _wstring Name)
{
	if (m_bAnimation == true)
	{
		return;
	}
	m_pLevel = to_wstring(Level);
	m_pEnemy_Name = Name;
	Set_FadeIn();
}

CEnemy_Info* CEnemy_Info::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Info* pInstance = new CEnemy_Info(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnemy_Info");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CEnemy_Info::Clone(void* pArg, CGameObject* pOwner)
{
	CEnemy_Info* pInstance = new CEnemy_Info(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Image");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CEnemy_Info::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CEnemy_Info::Describe_Entity()
{
}
#endif // _DEBUG
