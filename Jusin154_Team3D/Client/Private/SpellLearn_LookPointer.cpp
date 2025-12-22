#include "pch.h"
#include "SpellLearn_LookPointer.h"
#include "GameInstance.h"

CSpellLearn_LookPointer::CSpellLearn_LookPointer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSpellLearn_LookPointer::CSpellLearn_LookPointer(const CSpellLearn_LookPointer& rhs)
	:CElementObject(rhs)
{
}

HRESULT CSpellLearn_LookPointer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpellLearn_LookPointer::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 100.f;
	Desc.fY = 100.f;
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
	m_fAlphaTime = 5.f;
	m_fMoveSpeed = 130.f;
	Set_SpellLearn(0);
	m_fAngle = XMConvertToRadians(0);
	Visible(true);
	m_localX = 0.f;
	m_localY = 20.f;
	return S_OK;
}

void CSpellLearn_LookPointer::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpellLearn_LookPointer::Update(_float fTimeDelta)
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

	m_fAngle = XMConvertToRadians(static_cast<CUIObject*>(m_pOwner)->Get_Angle());

	float cosA = cos(m_fAngle);
	float sinA = sin(m_fAngle);

	float rotatedX = m_localX * cosA - m_localY * sinA;
	float rotatedY = m_localX * sinA + m_localY * cosA;

	MoveX(rotatedX);
	MoveY(rotatedY);

	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);
}

void CSpellLearn_LookPointer::Late_Update(_float fTimeDelta)
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

HRESULT CSpellLearn_LookPointer::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::ROTATION)))) {
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

_vector CSpellLearn_LookPointer::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CSpellLearn_LookPointer::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMapAngle", &m_fAngle, sizeof(_float))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSpellLearn_LookPointer::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_SU_CursorArrow"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CSpellLearn_LookPointer::Set_SpellLearn(_int Index)
{
	m_bMoveStart = false;
	m_iLineIndex = _int(static_cast<CUIObject*>(m_pOwner)->Get_Learninfo(Index).Lines.size());
	m_MoveLine = static_cast<CUIObject*>(m_pOwner)->Get_Learninfo(Index).Lines;
	m_iCurrentLine = 0;
	m_fX = static_cast<CUIObject*>(m_pOwner)->Get_Learninfo(Index).fStartPosition.x;
	m_fY = static_cast<CUIObject*>(m_pOwner)->Get_Learninfo(Index).fStartPosition.y;
}

CSpellLearn_LookPointer* CSpellLearn_LookPointer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpellLearn_LookPointer* pInstance = new CSpellLearn_LookPointer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpellLearn_LookPointer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpellLearn_LookPointer::Clone(void* pArg, CGameObject* pOwner)
{
	CSpellLearn_LookPointer* pInstance = new CSpellLearn_LookPointer(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpellLearn_LookPointer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpellLearn_LookPointer::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CSpellLearn_LookPointer::Describe_Entity()
{
}
#endif // _DEBUG
