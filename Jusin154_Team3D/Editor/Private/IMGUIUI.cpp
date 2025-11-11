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

void CIMGUIUI::PanelwstringTostring(vector<std::wstring>& panelNames)
{
	m_iPanelNamestring.clear();
	m_iPanelName.clear();
	m_iPanelNamewstring.clear();

	m_iPanelNamewstring = panelNames;

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
		m_iPanelNamestring.push_back(std::move(result));
	}

	// vector<const char*> 업데이트
	for (auto& str : m_iPanelNamestring)
		m_iPanelName.push_back(str.c_str());

	m_iPanelCount = 999;
}

void CIMGUIUI::ElementwstringTostring(vector<wstring>& panelNames)
{
	m_pElementNamestring.clear();
	m_pElementName.clear();
	m_pElementNamewstring.clear();

	m_pElementNamewstring = panelNames;

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
		m_pElementNamestring.push_back(std::move(result));
	}

	// vector<const char*> 업데이트
	for (auto& str : m_pElementNamestring)
		m_pElementName.push_back(str.c_str());

	m_iElementCount = 999;
}

void CIMGUIUI::Priority_Update(_float fTimeDelta)
{
}

void CIMGUIUI::Update(_float fTimeDelta)
{
	if (m_pGamePlay_Canvas != nullptr)
	{
		m_fCanvasAlpha = static_cast<CCanvasObject*>(m_pGamePlay_Canvas)->Get_Alpha();
	}
	if (m_pPanelObject != nullptr)
	{
		m_fPanelPos.x = static_cast<CPanelObject*>(m_pPanelObject)->Get_Current_Position().m128_f32[0];
		m_fPanelPos.y = static_cast<CPanelObject*>(m_pPanelObject)->Get_Current_Position().m128_f32[1];

		m_fPanelSize.x = static_cast<CPanelObject*>(m_pPanelObject)->Get_Current_Size().x;
		m_fPanelSize.y = static_cast<CPanelObject*>(m_pPanelObject)->Get_Current_Size().y;

		m_fPanelSizeXY = static_cast<CPanelObject*>(m_pPanelObject)->Get_Current_Size();

		m_fPanelTimeMult = static_cast<CPanelObject*>(m_pPanelObject)->Get_TimeMult();
		m_fPanelAlpha = static_cast<CElementObject*>(m_pPanelObject)->Get_Alpha();
	}

	if (m_pElementObject != nullptr)
	{
		m_fPos.x = static_cast<CElementObject*>(m_pElementObject)->Get_Current_Position().m128_f32[0];
		m_fPos.y = static_cast<CElementObject*>(m_pElementObject)->Get_Current_Position().m128_f32[1];

		m_fSize.x = static_cast<CElementObject*>(m_pElementObject)->Get_Current_Size().x;
		m_fSize.y = static_cast<CElementObject*>(m_pElementObject)->Get_Current_Size().y;

		m_fSizeXY = static_cast<CElementObject*>(m_pElementObject)->Get_Current_Size();
		m_fAlpha = static_cast<CElementObject*>(m_pElementObject)->Get_Alpha();

		m_fEndTime = static_cast<CElementObject*>(m_pElementObject)->Get_EndTime();
		m_fDelayTime = static_cast<CElementObject*>(m_pElementObject)->Get_DelayTime();
		m_fTimeMult = static_cast<CElementObject*>(m_pElementObject)->Get_TimeMult();
		m_bElementFadeIn = static_cast<CElementObject*>(m_pElementObject)->Get_FadeIn();
		m_bElementFadeOut = static_cast<CElementObject*>(m_pElementObject)->Get_FadeOut();
		m_fAlphaTime = static_cast<CElementObject*>(m_pElementObject)->Get_AlphaTime();
	}
	GUI::Begin("Current_PanelObject_Info");
	if (m_pGamePlay_Canvas != nullptr)
	{
		if (m_pGamePlay_Canvas != nullptr)
		{
			m_bCanvasVisible = static_cast<CCanvasObject*>(m_pGamePlay_Canvas)->Get_Visible();
			auto panelNames = static_cast<CCanvasObject*>(m_pGamePlay_Canvas)->Panel_Name();
			if (panelNames != m_iPanelNamewstring)
			{
				PanelwstringTostring(panelNames);
			}
			if (GUI::Combo("Panels", &m_iPanelCount, m_iPanelName.data(), static_cast<_int>(m_iPanelName.size())))
			{
				m_pPanelObject = static_cast<CCanvasObject*>(m_pGamePlay_Canvas)->Get_Panel(m_iPanelNamewstring[m_iPanelCount]);
			}
			if (GUI::Checkbox("Canvas Active", &m_bCanvasVisible))
			{
				static_cast<CCanvasObject*>(m_pGamePlay_Canvas)->Visible(m_bCanvasVisible);
			}
			if (GUI::Button("CanvasFadeIn"))
			{
				static_cast<CCanvasObject*>(m_pGamePlay_Canvas)->Set_FadeIn();
			}
			if (GUI::Button("CanvasFadeOut"))
			{
				static_cast<CCanvasObject*>(m_pGamePlay_Canvas)->Set_FadeOut();
			}
			GUI::Text("PanelAlph : %.1f", m_fCanvasAlpha);

		}

		if (m_pPanelObject != nullptr)
		{
			if (m_bCanvasVisible == false)
			{
				m_bPanelVisible = m_bCanvasVisible;
			}
			else
			{
				m_bPanelVisible = static_cast<CPanelObject*>(m_pPanelObject)->Get_Visible();
			}
			if (GUI::Checkbox("Panel Active", &m_bPanelVisible))
			{
				static_cast<CPanelObject*>(m_pPanelObject)->Visible(m_bPanelVisible);
			}
			if (GUI::Button("PanelFadeIn"))
			{
				static_cast<CPanelObject*>(m_pPanelObject)->Set_FadeIn();
			}
			if (GUI::Button("PanelFadeOut"))
			{
				static_cast<CPanelObject*>(m_pPanelObject)->Set_FadeOut();
			}
			GUI::Text("Origin Position : %.1f, %.1f", static_cast<CPanelObject*>(m_pPanelObject)->Get_Origin_Position().x, static_cast<CUIObject*>(m_pPanelObject)->Get_Origin_Position().y);
			GUI::Text("Current Position : %.1f, %.1f", static_cast<CPanelObject*>(m_pPanelObject)->Get_Current_Position().m128_f32[0], static_cast<CUIObject*>(m_pPanelObject)->Get_Current_Position().m128_f32[1]);
			GUI::Text("Origin Size : %.1f, %.1f", static_cast<CPanelObject*>(m_pPanelObject)->Get_Origin_Size().x, static_cast<CUIObject*>(m_pPanelObject)->Get_Origin_Size().y);
			GUI::Text("Current Size : %.1f, %.1f", static_cast<CPanelObject*>(m_pPanelObject)->Get_Current_Size().x, static_cast<CUIObject*>(m_pPanelObject)->Get_Current_Size().y);
			GUI::Text("PanelAlph : %.1f", m_fPanelAlpha);

			GUI::Text("PanelPosition");
			if (GUI::SliderFloat("X##PanelPos", &m_fPanelPos.x, -1920.f, 1920.f))
			{
				static_cast<CPanelObject*>(m_pPanelObject)->MoveX(m_fPanelPos.x);
			}

			if (GUI::SliderFloat("Y##PanelPos", &m_fPanelPos.y, -1080.f, 1080))
			{
				static_cast<CPanelObject*>(m_pPanelObject)->MoveY(m_fPanelPos.y);
			}

			GUI::Text("PanelSize");
			if (GUI::SliderFloat("X##PanelSize", &m_fPanelSize.x, 1.f, 1920.f))
			{
				static_cast<CPanelObject*>(m_pPanelObject)->SizeUpdate(m_fPanelSize.x, m_fPanelSize.y);
			}

			if (GUI::SliderFloat("Y##PanelSize", &m_fPanelSize.y, 1.f, 1080.f))
			{
				static_cast<CPanelObject*>(m_pPanelObject)->SizeUpdate(m_fPanelSize.x, m_fPanelSize.y);
			}

			if (GUI::SliderFloat("PanelSize", &m_fPanelSizeXY.x, 1.f, 1980.f))
			{
				m_fPanelSizeXY.y = m_fPanelSizeXY.x;
				static_cast<CPanelObject*>(m_pPanelObject)->SizeUpdate_float(m_fPanelSizeXY);
			}

			if (GUI::DragFloat("PanelTimeMult", &m_fPanelTimeMult, 0.01f, 0.f, 10.f))
			{
				static_cast<CPanelObject*>(m_pPanelObject)->Set_TimeMult(m_fPanelTimeMult);
			}
		}

	}

	GUI::End();

	GUI::Begin("Current_ElementObject_Info");
	if (m_pPanelObject != nullptr)
	{
		auto ElementNames = static_cast<CPanelObject*>(m_pPanelObject)->Element_Name();
		if (ElementNames != m_pElementNamewstring)
		{
			ElementwstringTostring(ElementNames);
		}
		if (GUI::Combo("Element", &m_iElementCount, m_pElementName.data(), static_cast<_int>(m_pElementName.size())))
		{
			m_pElementObject = static_cast<CPanelObject*>(m_pPanelObject)->Get_Element(m_pElementNamewstring[m_iElementCount]);
		}

	}
	if (m_pElementObject != nullptr)
	{
		if (m_bPanelVisible == false)
		{
			m_bElementVisible = m_bPanelVisible;
		}
		else
		{
			m_bElementVisible = static_cast<CElementObject*>(m_pElementObject)->Get_Visible();
		}
		if (GUI::Checkbox("Panel Active", &m_bElementVisible))
		{
			static_cast<CElementObject*>(m_pElementObject)->Visible(m_bElementVisible);
		}
		if (GUI::Button("FadeIn"))
		{
			static_cast<CElementObject*>(m_pElementObject)->Set_FadeIn();
		}
		if (GUI::Button("FadeOut"))
		{
			static_cast<CElementObject*>(m_pElementObject)->Set_FadeOut();
		}
		GUI::Text("Origin Position : %.1f, %.1f", static_cast<CElementObject*>(m_pElementObject)->Get_Origin_Position().x, static_cast<CUIObject*>(m_pElementObject)->Get_Origin_Position().y);
		GUI::Text("Current Position : %.1f, %.1f", static_cast<CElementObject*>(m_pElementObject)->Get_Current_Position().m128_f32[0], static_cast<CUIObject*>(m_pElementObject)->Get_Current_Position().m128_f32[1]);
		GUI::Text("Origin Size : %.1f, %.1f", static_cast<CElementObject*>(m_pElementObject)->Get_Origin_Size().x, static_cast<CUIObject*>(m_pElementObject)->Get_Origin_Size().y);
		GUI::Text("Current Size : %.1f, %.1f", static_cast<CElementObject*>(m_pElementObject)->Get_Current_Size().x, static_cast<CUIObject*>(m_pElementObject)->Get_Current_Size().y);
		GUI::Text("Alpha : %.1f", m_fAlpha);

		GUI::Text("Position");
		if (GUI::SliderFloat("X##Pos", &m_fPos.x, -1920.f, 1920.f))
		{
			static_cast<CElementObject*>(m_pElementObject)->MoveX(m_fPos.x);
		}

		if (GUI::SliderFloat("Y##Pos", &m_fPos.y, -1080.f, 1080))
		{
			static_cast<CElementObject*>(m_pElementObject)->MoveY(m_fPos.y);
		}

		GUI::Text("Size");
		if (GUI::SliderFloat("X##Size", &m_fSize.x, 1.f, 1920.f))
		{
			static_cast<CElementObject*>(m_pElementObject)->SizeUpdate(m_fSize.x, m_fSize.y);
		}

		if (GUI::SliderFloat("Y##Size", &m_fSize.y, 1.f, 1080.f))
		{
			static_cast<CElementObject*>(m_pElementObject)->SizeUpdate(m_fSize.x, m_fSize.y);
		}

		if (GUI::SliderFloat("Size", &m_fSizeXY.x, 1.f, 1980.f))
		{
			m_fSizeXY.y = m_fSizeXY.x;
			static_cast<CElementObject*>(m_pElementObject)->SizeUpdate_float(m_fSizeXY);
		}

		if (GUI::DragFloat("EndTime", &m_fEndTime, 0.01f, 0.f, 10.f))
		{
			static_cast<CElementObject*>(m_pElementObject)->Set_TimeMult(m_fEndTime);
		}

		if (GUI::DragFloat("m_fDelayTime", &m_fDelayTime, 0.01f, 0.f, 10.f))
		{
			static_cast<CElementObject*>(m_pElementObject)->Set_TimeMult(m_fDelayTime);
		}

		if (GUI::DragFloat("TimeMult", &m_fTimeMult, 0.01f, 0.f, 10.f))
		{
			static_cast<CElementObject*>(m_pElementObject)->Set_TimeMult(m_fTimeMult);
		}

		if (GUI::DragFloat("AlphaTime", &m_fAlphaTime, 0.01f, 0.f, 10.f))
		{
			static_cast<CElementObject*>(m_pElementObject)->Set_AlphaTime(m_fAlphaTime);
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
