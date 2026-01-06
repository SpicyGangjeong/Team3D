#include "pch.h"
#include "SpellLearn_Panel.h"
#include "GameInstance.h"
#include "SpellLearn_Name.h"
#include "SpellLearn.h"
#include "SpellLearn_MovePointer.h"
#include "SPellLearn_ChaserPointer.h"
#include "SpellLearn_Data.h"
#include "SpellLearn_Booster.h"
#include "SpellLearn_Slot.h"
#include "SpellLearn_Overlay.h"
#include "InfoInstance.h"

CSpellLearn_Panel::CSpellLearn_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CSpellLearn_Panel::CSpellLearn_Panel(const CSpellLearn_Panel& rhs)
	:CPanelObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CSpellLearn_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpellLearn_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 960.f;
	Desc.fY = -540.f;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Element(pArg)))
	{
		return E_FAIL;
	}
	m_fCanvasAlpha = 1.f;
	m_fSortZ = 0.05f;
	m_fDelayTime = 1.f;
	static_cast<CSpellLearn*>(m_pSpellLearn)->Set_Pointer(
		static_cast<CSpellLearn_MovePointer*>(m_pSpellLearn_MovePointer),
		static_cast<CSpellLearn_ChaserPointer*>(m_pSpellLearn_ChaserPointer));

	for (size_t i = 0; i < m_pBooster.size(); ++i)
	{
		static_cast<CSpellLearn_Booster*>(m_pBooster[i])->Move(m_pInfoInstance->Get_SpellLearn(Index).Booster[i].x,
			m_pInfoInstance->Get_SpellLearn(Index).Booster[i].y);
	}
	for (size_t i = 0; i < m_pBooster.size(); ++i)
	{
		m_PointerStates.push_back(m_State);
		m_ChaseStates.push_back(m_State);
	}
	static_cast<CSpellLearn_Overlay*>(m_pSpellLearn_Overlay)->Set_SpellLearn(0);
	static_cast<CSpellLearn_Slot*>(m_pSpellLearn_Slot)->Set_SpellLearn(0);
	Visible(true);
	ElementAllVisible(true);
	return S_OK;
}

void CSpellLearn_Panel::Set_Learn(_int Index)
{
	m_fTime = 0.f;
	static_cast<CSpellLearn*>(m_pSpellLearn)->Change_Image(Index);
	static_cast<CSpellLearn_MovePointer*>(m_pSpellLearn_MovePointer)->Set_SpellLearn(Index);
	static_cast<CSpellLearn_ChaserPointer*>(m_pSpellLearn_ChaserPointer)->Set_SpellLearn(Index);
	static_cast<CSpellLearn_Name*>(m_pSpellLearn_Name)->Set_Name(Index);
	static_cast<CSpellLearn_Overlay*>(m_pSpellLearn_Overlay)->Set_SpellLearn(Index);
	static_cast<CSpellLearn_Slot*>(m_pSpellLearn_Slot)->Set_SpellLearn(Index);
	for (size_t i = 0; i < m_pBooster.size(); ++i)
	{
		static_cast<CSpellLearn_Booster*>(m_pBooster[i])->Move(
			m_pInfoInstance->Get_SpellLearn(Index).Booster[i].x,
			m_pInfoInstance->Get_SpellLearn(Index).Booster[i].y);
		static_cast<CSpellLearn_Booster*>(m_pBooster[i])->Reset();
	}
}

void CSpellLearn_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpellLearn_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_pGameInstance->Key_Down(DIK_M))
	{
		if (Index >= m_pInfoInstance->Get_SpellLearnIndex() - 1)
		{
			Index = 0;
		}
		else
		{
			Index++;
		}
		Set_Learn(Index);
	}

	if (static_cast<CSpellLearn_MovePointer*>(m_pSpellLearn_MovePointer)->Get_MoveStart() == true)
	{
		m_fTime += fTimeDelta;

		for (size_t i = 0; i < m_pBooster.size(); ++i)
		{
			m_PointerStates[i].bPerHit = m_PointerStates[i].bHit;

			_bool bPointer = Chack_Element(static_cast<CUIObject*>(m_pBooster[i])->Get_Position(),
				static_cast<CUIObject*>(m_pSpellLearn_MovePointer)->Get_Position(),
				static_cast<CUIObject*>(m_pSpellLearn_MovePointer)->Get_Current_Size().x,
				static_cast<CUIObject*>(m_pSpellLearn_MovePointer)->Get_Current_Size().x);

			m_PointerStates[i].bHit = bPointer;

			if (m_PointerStates[i].bHit == true && m_PointerStates[i].bPerHit == false)
			{
				m_Booster.iBoosterIndex = _int(i);
				m_Booster.bBoosterOn = true;
				Function_Callback(TEXT("Booster"), &m_Booster);
			}

			if (m_PointerStates[i].bHit == false && m_PointerStates[i].bPerHit == true)
			{
				m_Booster.iBoosterIndex = _int(i);
				m_Booster.bBoosterOn = false;
				Function_Callback(TEXT("Booster"), &m_Booster);
			}
		}

		for (size_t i = 0; i < m_pBooster.size(); ++i)
		{
			m_ChaseStates[i].bPerHit = m_ChaseStates[i].bHit;

			_bool bPointer = Chack_Element(static_cast<CUIObject*>(m_pBooster[i])->Get_Position(),
				static_cast<CUIObject*>(m_pSpellLearn_ChaserPointer)->Get_Position(),
				static_cast<CUIObject*>(m_pSpellLearn_ChaserPointer)->Get_Current_Size().x,
				static_cast<CUIObject*>(m_pSpellLearn_ChaserPointer)->Get_Current_Size().x);

			m_ChaseStates[i].bHit = bPointer;

			if (m_ChaseStates[i].bHit == true && m_ChaseStates[i].bPerHit == false)
			{
				m_Booster.iBoosterIndex = _int(i);
				m_Booster.bBoosterOn = true;
				Function_Callback(TEXT("BoosterOff"), &m_Booster);
			}

			static_cast<CSpellLearn_Overlay*>(m_pSpellLearn_Overlay)->Set_Count(
				static_cast<CSpellLearn_MovePointer*>(m_pSpellLearn_MovePointer)->Get_SpellTrail());

		}

		if (m_fTime >= 1.f)
		{
			static_cast<CSpellLearn_ChaserPointer*>(m_pSpellLearn_ChaserPointer)->Set_FadeIn();
		}

		if (m_fTime >= 2.f)
		{
			static_cast<CSpellLearn_ChaserPointer*>(m_pSpellLearn_ChaserPointer)->Set_Move();

			if (_bool bPointer = Chack_Element(static_cast<CUIObject*>(m_pSpellLearn_MovePointer)->Get_Position(),
				static_cast<CUIObject*>(m_pSpellLearn_ChaserPointer)->Get_Position(),
				static_cast<CUIObject*>(m_pSpellLearn_MovePointer)->Get_Current_Size().x,
				static_cast<CUIObject*>(m_pSpellLearn_ChaserPointer)->Get_Current_Size().x) == true)
			{
				Set_Learn(Index);

				m_Booster.iBoosterIndex = _int(-1);
				m_Booster.bBoosterOn = false;
				Function_Callback(TEXT("Booster"), &m_Booster);
				for (size_t i = 0; i < m_PointerStates.size(); ++i)
				{
					m_PointerStates[i].bPerHit = false;
					m_PointerStates[i].bHit = false;
				}
			}

		}
	}
	__super::Update(fTimeDelta);
}

void CSpellLearn_Panel::Late_Update(_float fTimeDelta)
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

HRESULT CSpellLearn_Panel::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::SPELL_LEARN)))) {
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

_vector CSpellLearn_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CSpellLearn_Panel::Bind_ShaderResources()
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
	return S_OK;
}

HRESULT CSpellLearn_Panel::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_SpellLearn"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_SpellLinkDivide"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpellLearn_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpellLearn_Name>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpellLearn_Name**>(&m_pSpellLearn_Name))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("SpellLearn_Name"), m_pSpellLearn_Name);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpellLearn>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpellLearn**>(&m_pSpellLearn))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("SpellLearn"), m_pSpellLearn);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpellLearn_MovePointer>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpellLearn_MovePointer**>(&m_pSpellLearn_MovePointer))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("SpellLearn_MovePointer"), m_pSpellLearn_MovePointer);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpellLearn_ChaserPointer>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpellLearn_ChaserPointer**>(&m_pSpellLearn_ChaserPointer))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("SPellLeam_ChaserPointer"), m_pSpellLearn_ChaserPointer);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpellLearn_Slot>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpellLearn_Slot**>(&m_pSpellLearn_Slot))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("SpellLearn_Slot"), m_pSpellLearn_Slot);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpellLearn_Overlay>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpellLearn_Overlay**>(&m_pSpellLearn_Overlay))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("SpellLearn_Overlay"), m_pSpellLearn_Overlay);

	for (_int i = 0; i < 3; ++i)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpellLearn_Booster>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpellLearn_Booster**>(&m_pSpellLearn_Booster))))
		{
			return E_FAIL;
		}

		static_cast<CSpellLearn_Booster*>(m_pSpellLearn_Booster)->Set_Index(i);

		m_pBooster.push_back(m_pSpellLearn_Booster);
	}

	return S_OK;
}

CSpellLearn_Panel* CSpellLearn_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpellLearn_Panel* pInstance = new CSpellLearn_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpellLearn_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpellLearn_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CSpellLearn_Panel* pInstance = new CSpellLearn_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpellLearn_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpellLearn_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CSpellLearn_Panel::Describe_Entity()
{
}
#endif // _DEBUG
