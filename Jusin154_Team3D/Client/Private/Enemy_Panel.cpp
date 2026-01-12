#include "pch.h"
#include "Enemy_Panel.h"
#include "GameInstance.h"
#include "Enemy_HpBar.h"
#include "Enemy_Info.h"
#include "Boss_HpBar.h"
#include "InfoInstance.h"
#include "Monster.h"
#include "Enemy_SkillUI.h"

CEnemy_Panel::CEnemy_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPanelObject(pDevice, pContext)
{
}

CEnemy_Panel::CEnemy_Panel(const CEnemy_Panel& rhs)
    :CPanelObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CEnemy_Panel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEnemy_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 960.f;
	Desc.fY = -50.f;
	Desc.fSizeX = 960.f;
	Desc.fSizeY = 150.f;


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

	m_fAlpha = 1.f;
	m_fCanvasAlpha = 1.f;
	for (_int i = 0; i < 5; ++i)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEnemy_SkillUI>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CEnemy_SkillUI**>(&m_pRanrokProp)))) {
			return E_FAIL;
		}

		static_cast<CUIObject*>(m_pRanrokProp)->Visible(false);
		m_Pos.Prop = m_pRanrokProp;
		m_Pos.Position = _float4(0.f, 0.f, 0.f, 0.f);
		m_RanrokProps.push_back(m_Pos);
	}
	m_pInfoInstance->Add_Event(TEXT("RANROKPROP"), [this](void* p) {this->RanRokPropCreate(*reinterpret_cast<_vector*>(p)); });

	Visible(true);
	ElementAllVisible(true);
	return S_OK;
}

void CEnemy_Panel::Update_Target()
{
	if (m_pInfoInstance->Get_TargetMonster() == nullptr)
	{
		static_cast<CUIObject*>(m_pBoss_HpBar)->Set_FadeOut();
		static_cast<CUIObject*>(m_pEnemy_HpBar)->Set_FadeOut();
		static_cast<CUIObject*>(m_pEnemy_Info)->Set_FadeOut();
		return;
	}
	if (m_pInfoInstance->Get_TargetMonster()->Get_Stat()->Get_Stat().bBoss == true)
	{
		static_cast<CBoss_HpBar*>(m_pBoss_HpBar)->Update_Target();
		static_cast<CEnemy_HpBar*>(m_pEnemy_HpBar)->Set_FadeOut();
		static_cast<CEnemy_Info*>(m_pEnemy_Info)->Update_Target();
		static_cast<CEnemy_Info*>(m_pEnemy_Info)->Set_Font_Move(true);
	}
	else
	{
		static_cast<CBoss_HpBar*>(m_pBoss_HpBar)->Set_FadeOut();
		static_cast<CEnemy_HpBar*>(m_pEnemy_HpBar)->Update_Target();
		static_cast<CEnemy_Info*>(m_pEnemy_Info)->Update_Target();
		static_cast<CEnemy_Info*>(m_pEnemy_Info)->Set_Font_Move(false);
	}
}

void CEnemy_Panel::RanRokPropCreate(_fvector Position)
{
	if (m_pCurrentRanrokProps.size() >= 5)
	{
		Pos old{};
		old = m_pCurrentRanrokProps.back();
		m_pCurrentRanrokProps.pop_back();
		m_RanrokProps.push_back(old);
	}

	if (m_RanrokProps.empty() == true)
		return;

	Pos RanRokProp = m_RanrokProps.back();
	m_RanrokProps.pop_back();

	static_cast<CUIObject*>(RanRokProp.Prop)->Visible(true);
	XMStoreFloat4(&RanRokProp.Position, Position);
	XMStoreFloat3(&RanRokProp.TargetPos, Position);
	//static_cast<CUIObject*>(RanRokProp)->Set_Time(Info.fTime);
	static_cast<CUIObject*>(RanRokProp.Prop)->Set_Hover(true);

	m_pCurrentRanrokProps.push_front(RanRokProp);
}

void CEnemy_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CEnemy_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	Update_Target();

	if (m_pCurrentRanrokProps.empty() == false)
	{
		for (auto& it : m_pCurrentRanrokProps)
		{
			_bool Screen = World_to_ScreenUI(it.TargetPos, it.UIScreenPos, 480.f);

			static_cast<CEnemy_SkillUI*>(it.Prop)->Visible(Screen);

			if (Screen == true)
			{
				static_cast<CEnemy_SkillUI*>(it.Prop)->Move(it.UIScreenPos.x, it.UIScreenPos.y);
			}
		}
	}

	__super::Update(fTimeDelta);
}

void CEnemy_Panel::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
			return;
	}

	if (m_bVisible) 
	{
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
		__super::Late_Update(fTimeDelta);
	}
}

HRESULT CEnemy_Panel::Render()
{
	return S_OK;
}

_vector CEnemy_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CEnemy_Panel::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CEnemy_Panel::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CEnemy_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEnemy_HpBar>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CEnemy_HpBar**>(&m_pEnemy_HpBar))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Enemy_HpBar"), m_pEnemy_HpBar);
	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEnemy_Info>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CEnemy_Info**>(&m_pEnemy_Info))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Enemy_Info"), m_pEnemy_Info);
	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBoss_HpBar>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CBoss_HpBar**>(&m_pBoss_HpBar))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Boss_Bar"), m_pBoss_HpBar);
	
	return S_OK;
}

CEnemy_Panel* CEnemy_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Panel* pInstance = new CEnemy_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnemy_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CEnemy_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CEnemy_Panel* pInstance = new CEnemy_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEnemy_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CEnemy_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CEnemy_Panel::Describe_Entity()
{
}
#endif // _DEBUG
