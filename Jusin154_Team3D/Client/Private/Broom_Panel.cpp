#include "pch.h"
#include "Broom_Panel.h"
#include "GameInstance.h"
#include "Broom_Flag.h"
#include "Broom_Circle.h"
#include "Broom_Scoreboard.h"
#include "Broom_Finish.h"
#include "Broom_Record.h"
#include "Broom_Exit.h"
#include "Broom_Trophy.h"
#include "InfoInstance.h"

CBroom_Panel::CBroom_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CBroom_Panel::CBroom_Panel(const CBroom_Panel& rhs)
	:CPanelObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CBroom_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroom_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 960.f;
	Desc.fY = -400.f;
	Desc.fSizeX = 1920.f;
	Desc.fSizeY = 700.f;
	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };


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

	m_pInfoInstance->Add_Event(TEXT("CurrentRing"), [this](void* p) {this->Current_Ring(); });
	m_pInfoInstance->Add_Event(TEXT("Ready_Race"), [this](void* p) {this->Ready_Race(); });
	m_pInfoInstance->Add_Event(TEXT("Race_Count"), [this](void* p) {this->Race_Count(*reinterpret_cast<_int*>(p)); });
	m_pInfoInstance->Add_Event(TEXT("Race_Start"), [this](void* p) {this->Race_Start(*reinterpret_cast<_int*>(p)); });
	m_pInfoInstance->Add_Event(TEXT("RaceEnd"), [this](void* p) {this->Race_End(); });

	Visible(false);
	ElementAllVisible(false);
	return S_OK;
}

void CBroom_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CBroom_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	Set_BroomTimer();

	if (m_bDinish == true)
		m_fTime += fTimeDelta;

	if (m_fTime >= 1.f && m_bResults == false)
	{
		m_fTime = 0.f;
		m_bDinish = false;
		Race_Results();
	}

	if (m_bResults == true)
	{
		if (m_pGameInstance->Key_Down(DIK_ESCAPE))
		{
			m_fTime = 0.f;
			m_bDinish = false;
			m_bResults = false;
			ElementAllVisible(false);
		}
	}

	__super::Update(fTimeDelta);
}

void CBroom_Panel::Late_Update(_float fTimeDelta)
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

HRESULT CBroom_Panel::Render()
{
	return S_OK;
}

_vector CBroom_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CBroom_Panel::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CBroom_Panel::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBroom_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom_Flag>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast <CBroom_Flag**>(&m_pBroom_Flag))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Broom_Flag"), m_pBroom_Flag);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom_Circle>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast <CBroom_Circle**>(&m_pBroom_Circle))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Broom_Circle"), m_pBroom_Circle);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom_Scoreboard>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast <CBroom_Scoreboard**>(&m_pBroom_Scoreboard))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Broom_Scoreboard"), m_pBroom_Scoreboard);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom_Finish>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast <CBroom_Finish**>(&m_pBroom_Finish))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Broom_Fiish"), m_pBroom_Finish);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom_Record>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast <CBroom_Record**>(&m_pBroom_Record))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Broom_Record"), m_pBroom_Record);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom_Exit>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast <CBroom_Exit**>(&m_pBroom_Exit))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Broom_Exit"), m_pBroom_Exit);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom_Trophy>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast <CBroom_Trophy**>(&m_pBroom_Trophy))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Broom_Trophy"), m_pBroom_Trophy);

	return S_OK;
}

void CBroom_Panel::Ready_Race()
{
	Visible(true);
	static_cast<CBroom_Circle*>(m_pBroom_Circle)->Race_Setting();
	static_cast<CBroom_Flag*>(m_pBroom_Flag)->Race_Setting();
	static_cast<CBroom_Circle*>(m_pBroom_Circle)->Visible(true);
	static_cast<CBroom_Flag*>(m_pBroom_Flag)->Visible(true);
	static_cast<CBroom_Scoreboard*>(m_pBroom_Scoreboard)->Visible(false);
	static_cast<CBroom_Finish*>(m_pBroom_Finish)->Visible(false);
	static_cast<CBroom_Record*>(m_pBroom_Record)->Visible(false);
	static_cast<CBroom_Exit*>(m_pBroom_Exit)->Visible(false);
	static_cast<CBroom_Trophy*>(m_pBroom_Trophy)->Score(false);
}

void CBroom_Panel::Race_Count(_int Count)
{
	static_cast<CBroom_Circle*>(m_pBroom_Circle)->Race_Start(Count);
	static_cast<CBroom_Flag*>(m_pBroom_Flag)->Set_Start();
}

void CBroom_Panel::Race_Start(_int iRing)
{
	static_cast<CBroom_Flag*>(m_pBroom_Flag)->Set_Hover(true);
	static_cast<CBroom_Scoreboard*>(m_pBroom_Scoreboard)->Visible(true);
	static_cast<CBroom_Scoreboard*>(m_pBroom_Scoreboard)->Set_MaxRing(iRing);
}

void CBroom_Panel::Current_Ring()
{
	static_cast<CBroom_Scoreboard*>(m_pBroom_Scoreboard)->Set_CurrentRing();
}

void CBroom_Panel::Race_End()
{
	static_cast<CBroom_Scoreboard*>(m_pBroom_Scoreboard)->Visible(false);
	static_cast<CBroom_Finish*>(m_pBroom_Finish)->Visible(true);
	static_cast<CBroom_Finish*>(m_pBroom_Finish)->Finish(m_fRaceTime);
	static_cast<CBroom_Record*>(m_pBroom_Record)->Finish(m_fRaceTime);
	m_bDinish = true;
}

void CBroom_Panel::Race_Results()
{
	m_bResults = true;
	static_cast<CBroom_Circle*>(m_pBroom_Circle)->Rece_Results();
	static_cast<CBroom_Flag*>(m_pBroom_Flag)->Rece_Results();
	static_cast<CBroom_Record*>(m_pBroom_Record)->Rece_Results();
	static_cast<CBroom_Exit*>(m_pBroom_Exit)->Rece_Results();
	static_cast<CBroom_Trophy*>(m_pBroom_Trophy)->Score(static_cast<CBroom_Record*>(m_pBroom_Record)->NewScore());
}

void CBroom_Panel::Set_BroomTimer()
{
	m_fRaceTime = m_pInfoInstance->Get_Broom_Timer();
	static_cast<CBroom_Scoreboard*>(m_pBroom_Scoreboard)->Set_Timer(m_fRaceTime);
}

CBroom_Panel* CBroom_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_Panel* pInstance = new CBroom_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBroom_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CBroom_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CBroom_Panel* pInstance = new CBroom_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroom_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBroom_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CBroom_Panel::Describe_Entity()
{
}
#endif // _DEBUG
