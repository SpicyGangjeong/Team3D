#include "pch.h"
#include "Enemy_Detection.h"
#include "GameInstance.h"

CEnemy_Detection::CEnemy_Detection(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CElementObject(pDevice, pContext)
{
}

CEnemy_Detection::CEnemy_Detection(const CEnemy_Detection& Prototype)
	: CElementObject(Prototype)
{
}

HRESULT CEnemy_Detection::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnemy_Detection::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 500.f;
	Desc.fSizeX = 150.f;
	Desc.fSizeY = 150.f;

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
	m_bStep1 = true;
	m_vImageposi = _float4(37.5f, 37.5f, 75.f, 75.f);
	return S_OK;
}

void CEnemy_Detection::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CEnemy_Detection::Update(_float fTimeDelta)
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

	if (m_pGameInstance->Key_Pressing(DIK_Q))
	{
		m_fTime += fTimeDelta * m_fTimeMult;
		if (m_fTime >= 2.f)
		{
			if (m_bStep1 == true)
			{
				m_bStep1 = false;
				m_bStep2 = true;
				m_bStep3 = false;
				m_fTime = 0.f;
			}
			else if (m_bStep2 == true)
			{
				m_bStep1 = false;
				m_bStep2 = false;
				m_bStep3 = true;
				m_fTime = 0.f;
			}
			else if (m_bStep3 == true)
			{
				m_bStep1 = false;
				m_bStep2 = false;
				m_bStep3 = true;
				m_fAlpha -= fTimeDelta * m_fAlphaTime;
			}
		}
	}

	else if (m_pGameInstance->Key_Up(DIK_Q))
	{
		m_fTime = 0.f;
		m_bStep1 = true;
		m_bStep2 = false;
		m_bStep3 = false;
		m_bHover = false;
		m_fAlpha = 1.f;
		SizeUpX(m_fOrigin_Size.x);
		SizeUpY(m_fOrigin_Size.y);
		m_vImageposi.z = 75.f;
		m_vImageposi.w = 75.f;
	}

	if (m_bStep3 == true)
	{
		if (m_bHover == false)
		{
			SizeUpX(m_fSizeX + 10.f);
			SizeUpY(m_fSizeY + 10.f);
			m_vImageposi.z += 10.f;
			m_vImageposi.w += 10.f;
			m_bHover = true;
		}
	}

	if (m_fSizeX != m_fOrigin_Size.x)
	{
		SizeUpdate(-fTimeDelta * 10.f);
		m_vImageposi.z -= fTimeDelta;
		m_vImageposi.w -= fTimeDelta;
	}

	if (m_fAlpha <= 0.f)
		m_fAlpha = 0.f;

	__super::Update(fTimeDelta);

}

void CEnemy_Detection::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CEnemy_Detection::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::ENEMY_DETECTION))))
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

_vector CEnemy_Detection::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CEnemy_Detection::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iSpellType", &m_iSpellType, sizeof(_int))))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iStep1", &m_bStep1, sizeof(_bool))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iStep2", &m_bStep2, sizeof(_bool))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iStep3", &m_bStep3, sizeof(_bool))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos1", &m_vImageposi, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_vScale, sizeof(_float2))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CEnemy_Detection::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_HUD_Ring_Aggroed_Burst"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_HUD_NPC_WhiteFrameBlip"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_HUD_NPC_Alert"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom2), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_HUD_Ring_Alert_Fill"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom3), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_HUD_Ring_Alert_Yellow"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom4), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_HUD_Ring_Aggro_Blip"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom5), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CEnemy_Detection::SizeUpdate(_float fSizeX)
{
	m_fSizeX += fSizeX;
	m_fSizeY += fSizeX;
	m_vImageposi.z += fSizeX;
	m_vImageposi.w += fSizeX;
	if (m_fSizeX - m_fOrigin_Size.x <= fSizeX)
	{
		m_fSizeX = m_fOrigin_Size.x;
		m_fSizeY = m_fOrigin_Size.y;
		m_vImageposi.z = 75.f;
		m_vImageposi.w = 75.f;
	}
}

CEnemy_Detection* CEnemy_Detection::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Detection* pInstance = new CEnemy_Detection(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnemy_Detection");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CEnemy_Detection::Clone(void* pArg, CGameObject* pOwner)
{
	CEnemy_Detection* pInstance = new CEnemy_Detection(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEnemy_Detection");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CEnemy_Detection::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom2);
	SAFE_RELEASE(m_pDiffuse_TextureCom3);
	SAFE_RELEASE(m_pDiffuse_TextureCom4);
	SAFE_RELEASE(m_pDiffuse_TextureCom5);
	SAFE_RELEASE(m_pVIBufferCom);

}

#ifdef _DEBUG
void CEnemy_Detection::Describe_Entity()
{
}
#endif // _DEBUG
