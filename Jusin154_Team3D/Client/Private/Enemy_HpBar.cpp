#include "pch.h"
#include "Enemy_HpBar.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "Monster.h"

CEnemy_HpBar::CEnemy_HpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CEnemy_HpBar::CEnemy_HpBar(const CEnemy_HpBar& rhs)
	:CElementObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CEnemy_HpBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnemy_HpBar::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 40.f;
	Desc.fSizeX = 128.f;
	Desc.fSizeY = 20.f;

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
	m_fAlpha = 0.f;
	m_fAlphaTime = 10.f;
	m_vNine_Slice = _float4(15.f, 100.f, 5.f, 15.f);
	m_fMaxHp = 0.f;
	m_fCurrentHp = 0.f;
	m_fTargetHp = 0.f;
	SizeUpX(360.f);
	m_fMoveSpeed = 5.f;
	m_bAnimation = false;
	m_fLerpY = m_fSizeY;
	return S_OK;
}

void CEnemy_HpBar::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CEnemy_HpBar::Update(_float fTimeDelta)
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

	Update_Target();

	if (m_fTargetHp < m_fCurrentHp)
		Hit(fTimeDelta);

	m_fHpBar = m_fCurrentHp / m_fMaxHp;

	if (m_fCurrentHp <= 0.f)
	{
		m_bAnimation = true;
	}

	if (m_bAnimation == true)
	{
		m_fTime += fTimeDelta * m_fTimeMult;
		m_fSizeX += 3.f;
		m_fSizeY += 3.f;
		static_cast<CUIObject*>(m_pOwner)->Function_Callback(TEXT("Enemy_HpDie"));
	}
	if (m_fTime >= 0.9f)
	{
		SizeUpdate(m_fLerpX, m_fLerpY);
		m_bAnimation = false;
		m_bHover = false;
		m_fTime = 0.f;
	}
	__super::Update(fTimeDelta);
}

void CEnemy_HpBar::Late_Update(_float fTimeDelta)
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

HRESULT CEnemy_HpBar::Render()
{
	if (m_bHover == true)
	{
		if (FAILED(Bind_ShaderResources())) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::ENEMY_HPBAR)))) {
			return E_FAIL;
		}
		if (FAILED(m_pVIBufferCom->Bind_Resources())) {
			return E_FAIL;
		}
		if (FAILED(m_pVIBufferCom->Render())) {
			return E_FAIL;
		}
	}

	return S_OK;
}

_vector CEnemy_HpBar::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CEnemy_HpBar::Lerp_PosX(_float X)
{
	m_fDamage += X;
}

void CEnemy_HpBar::Hit(_float fTimeDelta)
{
	m_fCurrentHp = CMyTools::Lerp_f1D(m_fCurrentHp, m_fTargetHp, fTimeDelta * m_fMoveSpeed);
}

void CEnemy_HpBar::SizeUpX(_float SizeX)
{
	m_fSizeX = SizeX;
	m_fLerpX = SizeX;
}

void CEnemy_HpBar::Update_Target()
{
	if (m_pInfoInstance->Get_TargetMonster() == nullptr)
	{
		Set_FadeOut();
		return;
	}
	m_fMaxHp = m_pInfoInstance->Get_TargetMonster()->Get_Stat()->Get_Stat().fMaxHp;
	m_fCurrentHp = m_pInfoInstance->Get_TargetMonster()->Get_Stat()->Get_Stat().fCurrentHp;
	m_fTargetHp = m_pInfoInstance->Get_TargetMonster()->Get_Stat()->Get_Stat().fTargetHp;
	Set_FadeIn();
}

HRESULT CEnemy_HpBar::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNine_Slice", &m_vNine_Slice, sizeof(_float4))))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHp", &m_fHpBar, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iHover", &m_bAnimation, sizeof(_int))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_HpBar::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_Monster_HpBar"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_HUD_Monster_HealthMeterFill"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_VFX_T_FlameNoise_E"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom2), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CEnemy_HpBar* CEnemy_HpBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_HpBar* pInstance = new CEnemy_HpBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnemy_HpBar");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CEnemy_HpBar::Clone(void* pArg, CGameObject* pOwner)
{
	CEnemy_HpBar* pInstance = new CEnemy_HpBar(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Image");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CEnemy_HpBar::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom2);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CEnemy_HpBar::Describe_Entity()
{
}
#endif // _DEBUG
