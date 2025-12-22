#include "pch.h"
#include "SpellLearn_ChaserPointer.h"
#include "GameInstance.h"
#include "SpellLearn_MovePointer.h"
#include "InfoInstance.h"

CSpellLearn_ChaserPointer::CSpellLearn_ChaserPointer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSpellLearn_ChaserPointer::CSpellLearn_ChaserPointer(const CSpellLearn_ChaserPointer& rhs)
	:CElementObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CSpellLearn_ChaserPointer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpellLearn_ChaserPointer::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 476.f;
	Desc.fY = -253.f;
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
	m_fAlpha = 0.f;
	m_fAlphaTime = 5.f;
	m_fMoveSpeed = 165.f;
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("ChaseBooster"), [this](void* p) {this->SpeedUp(); });
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("LearnClear"), [this](void* p) {this->Clear(); });
	Set_SpellLearn(0);
	Visible(false);
	return S_OK;
}

void CSpellLearn_ChaserPointer::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpellLearn_ChaserPointer::Update(_float fTimeDelta)
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

	if (m_bMoveStart == true)
	{
		Line(fTimeDelta);
	}

	if (m_fMoveSpeed >= 165.f)
	{
		m_fMoveSpeed -= fTimeDelta;
	}

	m_fTime += fTimeDelta * m_fTimeMult;


	__super::Update(fTimeDelta);
}

void CSpellLearn_ChaserPointer::Late_Update(_float fTimeDelta)
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

HRESULT CSpellLearn_ChaserPointer::Render()
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

_vector CSpellLearn_ChaserPointer::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CSpellLearn_ChaserPointer::Set_SpellLearn(_int Index)
{
	m_fAlpha = 0.f;
	m_bHover = false;
	m_bMoveStart = false;
	m_fTime = 0.f;
	m_iLineIndex = _int(m_pInfoInstance->Get_SpellLearn(Index).Lines.size());
	m_MoveLine = m_pInfoInstance->Get_SpellLearn(Index).Lines;
	m_iCurrentLine = 0;
	m_fX = m_pInfoInstance->Get_SpellLearn(Index).fStartPosition.x;
	m_fY = m_pInfoInstance->Get_SpellLearn(Index).fStartPosition.y;
	m_fMoveSpeed = 165.f;
}

void CSpellLearn_ChaserPointer::Set_Move()
{
	m_bHover = true;
	m_bMoveStart = true;
}

void CSpellLearn_ChaserPointer::Line(_float fTime)
{
	m_vLerp_Position = m_MoveLine[m_iCurrentLine];

	if (Start_Lerp(fTime) == true)
	{
		if (m_iCurrentLine < m_iLineIndex - 1)
		{
			++m_iCurrentLine;
		}
		else
		{
			m_bMoveStart = false;
		}
	}

}

void CSpellLearn_ChaserPointer::SpeedUp()
{
	m_fMoveSpeed = 190.f;
}

void CSpellLearn_ChaserPointer::Clear()
{
	m_fMoveSpeed = 0.f;
	Set_FadeOut();
}

HRESULT CSpellLearn_ChaserPointer::Bind_ShaderResources()
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

HRESULT CSpellLearn_ChaserPointer::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_SpellMinigame_SpeedBurstDropBack"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSpellLearn_ChaserPointer* CSpellLearn_ChaserPointer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpellLearn_ChaserPointer* pInstance = new CSpellLearn_ChaserPointer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpellLearn_ChaserPointer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpellLearn_ChaserPointer::Clone(void* pArg, CGameObject* pOwner)
{
	CSpellLearn_ChaserPointer* pInstance = new CSpellLearn_ChaserPointer(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpellLearn_ChaserPointer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpellLearn_ChaserPointer::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CSpellLearn_ChaserPointer::Describe_Entity()
{
}
#endif // _DEBUG
