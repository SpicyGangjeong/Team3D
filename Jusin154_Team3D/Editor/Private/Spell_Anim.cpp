#include "pch.h"
#include "Spell_Anim.h"
#include "GameInstance.h"

CSpell_Anim::CSpell_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSpell_Anim::CSpell_Anim(const CSpell_Anim& rhs)
	:CElementObject(rhs)
{
}

HRESULT CSpell_Anim::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpell_Anim::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 590.f;
	Desc.fY = -100.f;
	Desc.fSizeX = 470.f;
	Desc.fSizeY = 250.f;

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
	m_fAlphaTime = 9.f;
	m_fSortZ = 0.01f;
	m_iTotalFrames = 151;
	m_fFrameTime = 0.1f;
	m_iCurrentFrame = 0;
	m_bAnim_Start = false;
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("Slot_Hover"), [this](void* p) {this->Set_SkillType(*reinterpret_cast<_int*>(p)); });
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("FadeIn"), [this](void* p) {this->Set_FadeIn(); });
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("FadeOut"), [this](void* p) {this->Set_FadeOut(); });
	return S_OK;
}

void CSpell_Anim::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpell_Anim::Update(_float fTimeDelta)
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
		m_bFadeOut = false;
		m_fAlpha = 0.f;
		m_bAnim_Start = false;
	}


	if (m_bAnim_Start == true)
	{
		m_fTime += fTimeDelta * m_fTimeMult;
	}
	else
	{
		m_iCurrentFrame = 0;
	}

	if (m_fTime >= m_fFrameTime)
	{
		m_iCurrentFrame++;
		m_fTime -= m_fFrameTime;

		if (m_iCurrentFrame >= m_iTotalFrames)
			m_iCurrentFrame = 0;
	}

	__super::Update(fTimeDelta);
}

void CSpell_Anim::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
		if (m_pDiffuse_TextureCom != nullptr)
			m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CSpell_Anim::Render()
{

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::ALPHABLEND))))
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

_vector CSpell_Anim::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CSpell_Anim::Set_FadeIn()
{
	if (!m_bFadeIn && m_fAlpha < 1.f)
	{
		m_bFadeIn = true;
		m_bFadeOut = false;
	}
	m_bAnim_Start = true;
}

HRESULT CSpell_Anim::Change_Image(_int SpellID)
{
	_wstring pImageName = static_cast<CUIObject*>(m_pOwner)->Get_Info(SpellID).pImage_Name;
	m_iTotalFrames = static_cast<CUIObject*>(m_pOwner)->Get_Info(SpellID).iAnimNum;
	if (m_pDiffuse_TextureCom)
	{
		Remove_Component<CTexture>();
		SAFE_RELEASE(m_pDiffuse_TextureCom);
	}
	if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(LEVEL::UI), pImageName, reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom))))
		return E_FAIL;

	return S_OK;
}

void CSpell_Anim::Set_SkillType(_int eType)
{
	if (eType != -1 && eType != m_iPerSpell)
	{
		m_iPerSpell = eType;
		m_iCurrentFrame = 0;
		Change_Image(eType);
	}
	else if (eType == -1)
		m_iPerSpell = eType;
}

HRESULT CSpell_Anim::Bind_ShaderResources()
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
	if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iCurrentFrame)))
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
	return S_OK;
}

HRESULT CSpell_Anim::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Accio"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}


CSpell_Anim* CSpell_Anim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpell_Anim* pInstance = new CSpell_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpell_Anim");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpell_Anim::Clone(void* pArg, CGameObject* pOwner)
{
	CSpell_Anim* pInstance = new CSpell_Anim(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Anim");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpell_Anim::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CSpell_Anim::Describe_Entity()
{
}
