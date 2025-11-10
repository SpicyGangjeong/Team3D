#include "pch.h"
#include "Level_ObjectViewer.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "RootModelPart.h"
#include "Body.h"
#include "Head.h"
#include "Hair.h"
#include "Dummy_Cube.h"
#include "DebugCamera.h"
#include "TombProtector.h"
#include "Troll.h"
#include "Layer.h"
#include "GameObject.h"

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

	Show_AnimList();

	Object_Setting();
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
		Add_Creature();
		
		Add_Human();
		
	}
	GUI::EndTabBar();
	GUI::End();
	
}

void CLevel_ObjectViewer::Add_Creature()
{
	if (ImGui::BeginTabItem("Creatures"))
	{
		if (GUI::Button("Add TombProtector"))
		{
			CGameObject* pTempObject = { nullptr };
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTombProtector>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, TEXT("Layer_Monster"), nullptr, nullptr, &pTempObject)))
				return;
			m_Objects.push_back(pTempObject);
		}

		if (GUI::Button("Add Troll"))
		{
			CGameObject* pTempObject = { nullptr };
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTroll>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, TEXT("Layer_Monster"), nullptr, nullptr, &pTempObject)))
				return;
			m_Objects.push_back(pTempObject);
		}

		GUI::EndTabItem();
	}
}

void CLevel_ObjectViewer::Add_Human()
{
	if (ImGui::BeginTabItem("Human"))
	{
		if (GUI::CollapsingHeader("Body"))
		{
			if (GUI::Button("Add Body"))
			{
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRootModelPart>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, TEXT("Layer_Root"), nullptr, nullptr, &m_HumanRoot)))
					return;

				CGameObject* pTempObject = { nullptr };
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBody>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, TEXT("Layer_Body"), nullptr, m_HumanRoot, &pTempObject)))
					return;
				m_Objects.push_back(pTempObject);
			}
		}
		_bool bDisabled = (m_HumanRoot == nullptr);
		if (bDisabled) ImGui::BeginDisabled();
		if (GUI::CollapsingHeader("Head"))
		{
			if (GUI::Button("Add Head"))
			{
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CHead>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, TEXT("Layer_Head"), nullptr, m_HumanRoot)))
					return;
			}
		}
		if (GUI::CollapsingHeader("Hair"))
		{
			if (GUI::Button("Add Hair"))
			{
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CHair>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, TEXT("Layer_Hair"), nullptr, m_HumanRoot)))
					return;
			}
		}
		if (bDisabled) ImGui::EndDisabled();
		GUI::EndTabItem();
	}
}

void CLevel_ObjectViewer::Show_ModelFilePath()
{
	/*GUI::Begin("Model_List");
	if(GUI::BeginTabBar("Model_List"))
	{
		if (GUI::BeginTabItem("Assimp_Model"))
		{
			for (_uint i = 0; i < m_pGameInstance->ModelFilePathCount(); i++)
			{
				GUI::Text(m_pGameInstance->Load_ModelFilePath(i));
				GUI::SameLine();

				std::string label = "Binary##" + std::to_string(i);
				if (GUI::Button(label.c_str()))
				{
					m_pGameInstance->SaveAssimpModel(m_pGameInstance->Load_ModelFilePath(i));
				}
			}
			GUI::EndTabItem();
		}

		if (GUI::BeginTabItem("Binary_Model"))
		{
			for (_uint i = 0; i < m_pGameInstance->BinaryModelFilePathCount(); i++)
			{
				GUI::Text(m_pGameInstance->Load_BinaryModelFilePath(i));
			}
			GUI::EndTabItem();
		}
	}
	GUI::EndTabBar();
	GUI::End();*/
}

void CLevel_ObjectViewer::Show_AnimList()
{
	GUI::Begin("Anim_List");
	if (!m_Objects.empty())
	{
		if (!m_Objects[m_iObjectIndex]->isDead())
		{
			CModel* pModel = m_Objects[m_iObjectIndex]->Get_Component<CModel>();
			for (_uint i = 0; i < pModel->Get_AnimSize(); i++)
			{
				if (GUI::Button(pModel->Get_AnimList(i)))
				{
					pModel->Set_AnimationIndex(i);
				}
			}
		}
	}
	GUI::End();
}

void CLevel_ObjectViewer::Show_ObjectList()
{
	for (size_t i = 0; i < m_Objects.size(); i++)
	{
		if (GUI::Selectable(typeid(*m_Objects[i]).name()))
		{
			m_iObjectIndex = (_int)i;
		}
	}
}

void CLevel_ObjectViewer::Object_Setting()
{
	GUI::Begin("Object_Info");
	if (!m_Objects.empty())
	{
		Show_ObjectList();
		CModel* pModel = m_Objects[m_iObjectIndex]->Get_Component<CModel>();

		_float AnimTrack = pModel->Get_CurrentTrackPosition();
		GUI::DragFloat("AnimTrack", &AnimTrack);

		_float AnimSpeed = pModel->Get_AnimSpeed();
		GUI::DragFloat("AnimSpeed", &AnimSpeed, 0.1f);
		pModel->Set_AnimSpeed(AnimSpeed);

		_bool bPlayAnim = pModel->Get_PlayAnim();
		if (GUI::Checkbox("Play Anim", &bPlayAnim))
		{
			pModel->Set_PlayAnim(bPlayAnim);
		}
		if (GUI::Button("Delete"))
		{

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
	CameraDesc.vEye = _float3(0.f, 50.f, -10.f);
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
