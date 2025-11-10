#include "pch.h"
#include "IMGUIUI.h"
#include "UIObject.h"
#include "GamePlay_Canvas.h"
#include "PanelObject.h"
#include "ElementObject.h"
#include "Mouse_Cursor.h"

CIMGUIUI::CIMGUIUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CIMGUIUI::CIMGUIUI(const CIMGUIUI& rhs)
	: CGameObject(rhs)
{
}

HRESULT CIMGUIUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CIMGUIUI::Initialize(void* pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;
	return S_OK;
}

void CIMGUIUI::wstringTostring(vector<std::wstring>& panelNames)
{
	m_pNamestring.clear();
	m_pName.clear();
	m_pNamewstring.clear();

	m_pNamewstring = panelNames;

	for (auto& wname : panelNames)
	{
		// wstring -> UTF-8 string 변환
		int size = WideCharToMultiByte(CP_UTF8, 0,
			wname.c_str(), (int)wname.size(),
			nullptr, 0, nullptr, nullptr);

		std::string result(size, 0);

		WideCharToMultiByte(CP_UTF8, 0,
			wname.c_str(), (int)wname.size(),
			&result[0], size, nullptr, nullptr);

		// 변환된 string 저장
		m_pNamestring.push_back(std::move(result));
	}

	// vector<const char*> 업데이트
	for (auto& str : m_pNamestring)
		m_pName.push_back(str.c_str());
}

void CIMGUIUI::Priority_Update(_float fTimeDelta)
{
}

void CIMGUIUI::Update(_float fTimeDelta)
{
	if (m_pElementObject != nullptr)
	{
		m_fPos.x = static_cast<CUIObject*>(m_pElementObject)->Get_Current_Position().m128_f32[0];
		m_fPos.y = static_cast<CUIObject*>(m_pElementObject)->Get_Current_Position().m128_f32[1];

		m_fSize.x = static_cast<CUIObject*>(m_pElementObject)->Get_Current_Size().x;
		m_fSize.y = static_cast<CUIObject*>(m_pElementObject)->Get_Current_Size().y;

		m_fSizeXY = static_cast<CUIObject*>(m_pElementObject)->Get_Current_Size();
		
		m_fTimeMult = static_cast<CUIObject*>(m_pElementObject)->Get_TimeMult();
	}
	GUI::Begin("Current_PanelObject_Info");
	if(m_pGamePlay_Canvas != nullptr)
	{
		if (m_pGamePlay_Canvas != nullptr)
		{
			auto panelNames = static_cast<CCanvasObject*>(m_pGamePlay_Canvas)->Panel_Name();
			wstringTostring(panelNames);
			if (GUI::Combo("Panels", &m_iPanelCount, m_pName.data(), static_cast<_int>(m_pName.size())))
			{
				m_pPanelObject = static_cast<CCanvasObject*>(m_pGamePlay_Canvas)->Get_Panel(m_pNamewstring[m_iPanelCount]);
			}
		}

		if (m_pPanelObject != nullptr)
		{

		}

	}

	GUI::End();

	GUI::Begin("Current_ElementObject_Info");
	if (m_pPanelObject != nullptr)
	{
		auto ElementNames = static_cast<CPanelObject*>(m_pPanelObject)->Element_Name();
		wstringTostring(ElementNames);
		if (GUI::Combo("Panels", &m_iElementCount, m_pName.data(), static_cast<_int>(m_pName.size())))
		{
			m_pElementObject = static_cast<CPanelObject*>(m_pPanelObject)->Get_Element(m_pNamewstring[m_iElementCount]);
		}
	}
	if (m_pElementObject != nullptr)
	{
		GUI::Text("Origin Position : %.1f, %.1f", static_cast<CUIObject*>(m_pElementObject)->Get_Origin_Position().x, static_cast<CUIObject*>(m_pElementObject)->Get_Origin_Position().y);
		GUI::Text("Current Position : %.1f, %.1f", static_cast<CUIObject*>(m_pElementObject)->Get_Current_Position().m128_f32[0], static_cast<CUIObject*>(m_pElementObject)->Get_Current_Position().m128_f32[1]);
		GUI::Text("Origin Size : %.1f, %.1f", static_cast<CUIObject*>(m_pElementObject)->Get_Origin_Size().x, static_cast<CUIObject*>(m_pElementObject)->Get_Origin_Size().y);
		GUI::Text("Current Size : %.1f, %.1f", static_cast<CUIObject*>(m_pElementObject)->Get_Current_Size().x, static_cast<CUIObject*>(m_pElementObject)->Get_Current_Size().y);

		GUI::Text("Position");
		if (GUI::SliderFloat("X##Pos", &m_fPos.x, 0.f, 1920.f))
		{
			static_cast<CUIObject*>(m_pElementObject)->MoveX(m_fPos.x);
		}

		if (GUI::SliderFloat("Y##Pos", &m_fPos.y, 0.f, 1080))
		{
			static_cast<CUIObject*>(m_pElementObject)->MoveY(m_fPos.y);
		}

		GUI::Text("Size");
		if (GUI::SliderFloat("X##Size", &m_fSize.x, 1.f, 1920.f))
		{
			static_cast<CUIObject*>(m_pElementObject)->SizeUpdate(m_fSize.x, m_fSize.y);
		}

		if (GUI::SliderFloat("Y##Size", &m_fSize.y, 1.f, 1080.f))
		{
			static_cast<CUIObject*>(m_pElementObject)->SizeUpdate(m_fSize.x, m_fSize.y);
		}

		if (GUI::SliderFloat("Size", &m_fSizeXY.x, 1.f, 1980.f))
		{
			m_fSizeXY.y = m_fSizeXY.x;
			static_cast<CUIObject*>(m_pElementObject)->SizeUpdate_float(m_fSizeXY);
		}

		if (GUI::DragFloat("TimeMult", &m_fTimeMult, 0.01f, 0.f, 10.f))
		{
			static_cast<CUIObject*>(m_pElementObject)->Set_TimeMult(m_fTimeMult);
		}
	}
	GUI::End();
}

void CIMGUIUI::Late_Update(_float fTimeDelta)
{
}

HRESULT CIMGUIUI::Render()
{
	return S_OK;
}

HRESULT CIMGUIUI::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CIMGUIUI::Ready_Components(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CGamePlay_Canvas>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, &m_pGamePlay_Canvas)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CIMGUIUI* CIMGUIUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIMGUIUI* pInstance = new CIMGUIUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CIMGUIUI");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CIMGUIUI::Clone(void* pArg, CGameObject* pOwner)
{
	CIMGUIUI* pInstance = new CIMGUIUI(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CIMGUIUI");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CIMGUIUI::Free()
{
	__super::Free();
}

void CIMGUIUI::Describe_Entity()
{
}
