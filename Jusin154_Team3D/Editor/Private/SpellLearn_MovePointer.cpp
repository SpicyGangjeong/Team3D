#include "pch.h"
#include "SpellLearn_MovePointer.h"
#include "GameInstance.h"

CSpellLearn_MovePointer::CSpellLearn_MovePointer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSpellLearn_MovePointer::CSpellLearn_MovePointer(const CSpellLearn_MovePointer& rhs)
	:CElementObject(rhs)
{
}

HRESULT CSpellLearn_MovePointer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpellLearn_MovePointer::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 454.f;
	Desc.fY = -240.f;
	Desc.fSizeX = 32.f;
	Desc.fSizeY = 32.f;
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
	m_fAlphaTime = 5.f;
	m_fMoveSpeed = 15.f;
	m_MoveLine.push_back(XMVectorSet(454.f, -240.f, 0.f, 1.f));
	m_MoveLine.push_back(XMVectorSet(454.f, 90.f, 0.f, 1.f));



	Index = 0;

	Visible(false);
	return S_OK;
}

void CSpellLearn_MovePointer::SpellLearn(_int Index)
{
	m_iSpell = Index;
	m_bMoveStart = true;
}

void CSpellLearn_MovePointer::Line(_float fTime)
{
	m_vLerp_Position = m_MoveLine[Index];

	if (Start_Lerp(m_fMoveSpeed * fTime) == true)
	{
		if (Index < m_MoveLine.size() -1)
		{
			++Index;
		}
		else
		{
			m_bMoveStart = false;
		}
	}

}

void CSpellLearn_MovePointer::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpellLearn_MovePointer::Update(_float fTimeDelta)
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
			m_fAlpha -= fTimeDelta * m_fAlphaTime;;

		if (m_fAlpha <= 0.f)
		{
			m_bFadeOut = false;
			m_fAlpha = 0.f;
		}
	}

	if (m_pGameInstance->Key_Down(DIK_L))
	{
		m_bMoveStart = true;
	}

	if (m_bMoveStart == true)
	{
		if (m_iSpell == ENUM_CLASS(SPELL_LINE::LINE))
		{
			Line(fTimeDelta);
		}
		else if (m_iSpell == ENUM_CLASS(SPELL_LINE::CURVE))
		{
			//Curve();
		}
	}

	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);
}

void CSpellLearn_MovePointer::Late_Update(_float fTimeDelta)
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

HRESULT CSpellLearn_MovePointer::Render()
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

_vector CSpellLearn_MovePointer::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CSpellLearn_MovePointer::Bind_ShaderResources()
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

HRESULT CSpellLearn_MovePointer::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_SU_Cursor"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSpellLearn_MovePointer* CSpellLearn_MovePointer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpellLearn_MovePointer* pInstance = new CSpellLearn_MovePointer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpellLearn_MovePointer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpellLearn_MovePointer::Clone(void* pArg, CGameObject* pOwner)
{
	CSpellLearn_MovePointer* pInstance = new CSpellLearn_MovePointer(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpellLearn_MovePointer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpellLearn_MovePointer::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CSpellLearn_MovePointer::Describe_Entity()
{
}
