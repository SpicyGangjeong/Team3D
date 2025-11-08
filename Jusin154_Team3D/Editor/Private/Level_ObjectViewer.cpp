#include "pch.h"
#include "Level_ObjectViewer.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Body.h"
#include "Head.h"
#include "Dummy_Cube.h"
#include "DebugCamera.h"
#include "Layer.h"

CLevel_ObjectViewer::CLevel_ObjectViewer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{

}

HRESULT CLevel_ObjectViewer::Initialize()
{
	if (FAILED(Ready_Layer_Light())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Dummy(TEXT("Layer_Dummy"))))
	{
		return E_FAIL;
	}


	return S_OK;
}

void CLevel_ObjectViewer::Update(_float fTimeDelta)
{
	Add_Object();

	Show_ModelFilePath();
}

HRESULT CLevel_ObjectViewer::Render()
{
	SetWindowText(g_hWnd, TEXT("오브젝트 레벨입니다"));
	//GUI::ShowDemoWindow();
	return S_OK;
}

void CLevel_ObjectViewer::Add_Object()
{
	GUI::Begin("Add Object");

	if (GUI::BeginTabBar("ObjectTabs"))
	{
		if (ImGui::BeginTabItem("Creatures"))
		{
			GUI::EndTabItem();
		}
		if (ImGui::BeginTabItem("Human"))
		{
			if (GUI::CollapsingHeader("Body"))
			{
				if (GUI::Button("Add"))
				{
					if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBody>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, TEXT("Layer_Body"))))
						return;
					CBody* pBody = m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_Body"))->Get_Object<CBody>();
				}
				if (GUI::Button("Delete"))
				{
					m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_Body"))->Clear_Layer();
				}
			}
			if (GUI::CollapsingHeader("Head"))
			{
				if (GUI::Button("Add"))
				{
					CGameObject* pOwner = m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_Body"))->Get_Object<CBody>();
					if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CHead>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, TEXT("Layer_Head"), nullptr, pOwner)))
						return;
				}
			}
			if (GUI::CollapsingHeader("Hair"))
			{
			}
			GUI::EndTabItem();
		}
		
		GUI::EndTabBar();

		GUI::End();
	}
}

void CLevel_ObjectViewer::Show_ModelFilePath()
{
	GUI::Begin("Model_List");
	for (_uint i = 0; i < m_pGameInstance->ModelFilePathCount(); i++)
	{
		/*GUI::Text(m_pGameInstance->Load_BinaryModelFilePath(i));*/
		GUI::SameLine();

		std::string label = "Binary##" + std::to_string(i);
		if (GUI::Button(label.c_str()))
		{
			m_pGameInstance->SaveAssimpModel(m_pGameInstance->Load_ModelFilePath(i));
		}
	}
	GUI::End();
}

HRESULT CLevel_ObjectViewer::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CDebugCamera::CAMERA_DEBUG_DESC			CameraDesc{};
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fSpeedPerSec = 5.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDebugCamera>(g_iStaticLevel, NEXT_LEVEL, 
		strLayerTag, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_ObjectViewer::Ready_Layer_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 0.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 0.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->On_Light(NEXT_LEVEL, TEXT("Main_Light"), LightDesc, nullptr))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_ObjectViewer::Ready_Layer_UI(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_ObjectViewer::Ready_Layer_Dummy(const _wstring& strLayerTag)
{
	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_Cube>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE)))
		return E_FAIL;*/

	return S_OK;
}

CLevel_ObjectViewer* CLevel_ObjectViewer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_ObjectViewer* pInstance = new CLevel_ObjectViewer(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_ObjectViewer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}



void CLevel_ObjectViewer::Free()
{
	__super::Free();


}
