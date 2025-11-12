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
#include "Layer.h"
#include "GameObject.h"
#include "DummyObject.h"

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
	GUI::Begin("Add PartObject");

	if (GUI::BeginTabBar("ObjectTabs"))
	{
		Add_Parts();
	}
	GUI::EndTabBar();
	GUI::End();

}

void CLevel_ObjectViewer::Add_Parts()
{
	if (GUI::BeginTabItem("Male"))
	{
		if (GUI::BeginTabBar("##MaleTabBar"))
		{
			if (GUI::BeginTabItem("Head"))
			{
				Category_PartsModelList("Human/Head/Male", "Head");

				GUI::EndTabItem();
			}
			_bool bDisabled = {};
			if (m_HumanRoot)
				bDisabled = (m_HumanRoot->Get_MainModel());
			if (!bDisabled) GUI::BeginDisabled();
			if (GUI::BeginTabItem("Body"))
			{
				Category_PartsModelList("Human/Body/Male", "Body");

				GUI::EndTabItem();
			}

			if (GUI::BeginTabItem("Hair"))
			{
				Category_PartsModelList("Human/Hair/Male", "Hair");

				GUI::EndTabItem();
			}
			if (!bDisabled) GUI::EndDisabled();
			GUI::EndTabBar();
		}
		GUI::EndTabItem();
	}

	if (GUI::BeginTabItem("FeMale"))
	{
		if (GUI::BeginTabBar("##FeMaleTabBar")) 
		{
			if (GUI::BeginTabItem("Head"))
			{
				Category_PartsModelList("Human/Head/FeMale", "Head");

				GUI::EndTabItem();
			}
			_bool bDisabled = {};
			if (m_HumanRoot)
				bDisabled = (m_HumanRoot->Get_MainModel());
			if (!bDisabled) GUI::BeginDisabled();
			if (GUI::BeginTabItem("Body"))
			{
				Category_PartsModelList("Human/Body/FeMale", "Body");

				GUI::EndTabItem();
			}

			if (GUI::BeginTabItem("Hair"))
			{
				Category_PartsModelList("Human/Hair/FeMale", "Hair");

				GUI::EndTabItem();
			}
			if (!bDisabled) GUI::EndDisabled();
			GUI::EndTabBar();
		}
		GUI::EndTabItem();
	}

	
}

void CLevel_ObjectViewer::Show_ModelFilePath()
{
	GUI::Begin("Model_List");
	if (GUI::BeginTabBar("Model_List"))
	{
		Category_ModelList("Monster");

		Category_ModelList("Human");
	}
	GUI::EndTabBar();
	GUI::End();
}

void CLevel_ObjectViewer::Category_ModelList(const _char* Category)
{
	if (GUI::BeginTabItem(Category))
	{
		if (m_pGameInstance->BinaryModelFilePathCount() > 0)
		{
			for (_uint i = 0; i < m_pGameInstance->BinaryModelFilePathCount(); i++)
			{
				const _char* szCategory = m_pGameInstance->Load_BinaryModelFilePath(i);

				if (strstr(szCategory, Category))
				{
					if (GUI::Button(szCategory))
					{
						CGameObject* pTempObject = { nullptr };

						CDummyObject::PARTS_OBJECT_DESC Desc = {};

						Desc.pModelPrototypeTag = Save_ModelName(szCategory);

						Save_LayerName(Category);

						if (m_Objects.size() > 0)
						{
							m_pGameInstance->Get_Layer(NEXT_LEVEL, m_wszPreLayer)->Clear_Layer();
							m_Objects.pop_back();
						}

						if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummyObject>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, m_wszLayer, &Desc, nullptr, reinterpret_cast<CDummyObject**>(&pTempObject))))
							return;

						wcscpy_s(m_wszPreLayer, m_wszLayer);

						m_Objects.push_back(pTempObject);
					}
				}
			}
		}
		GUI::EndTabItem();
	}
}

void CLevel_ObjectViewer::Category_PartsModelList(const _char* Category, const _char* Layer)
{
	if (m_pGameInstance->BinaryModelFilePathCount() > 0)
	{
		if (!m_HumanRoot)
		{
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRootModelPart>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, TEXT("Layer_Root"),
				nullptr, nullptr, reinterpret_cast<CRootModelPart**>(&m_HumanRoot))))
				return;
		}
		for (_uint i = 0; i < m_pGameInstance->BinaryModelFilePathCount(); i++)
		{
			const _char* szCategory = m_pGameInstance->Load_BinaryModelFilePath(i);

			if (strstr(szCategory, Category))
			{
				if (GUI::Button(szCategory))
				{
					Save_LayerName(Layer);

					CGameObject* pTempObject = { nullptr };

					CDummyObject::PARTS_OBJECT_DESC Desc = {};

					m_HumanRoot->Set_PrototypeModelName(Save_ModelName(szCategory));

					m_HumanRoot->Set_ModelName(szCategory);

					_uint iIndex = 0;
					if (strstr(szCategory,"Head"))
						iIndex = ENUM_CLASS(CRootModelPart::PARTSTYPE::HEAD);
					else if (strstr(szCategory,"Body"))
						iIndex = ENUM_CLASS(CRootModelPart::PARTSTYPE::BODY);
					else if (strstr(szCategory,"Hair"))
						iIndex = ENUM_CLASS(CRootModelPart::PARTSTYPE::HAIR);

					m_HumanRoot->Change_Model(iIndex);
				}
			}
		}
	}
}

void CLevel_ObjectViewer::Show_AnimList()
{
	GUI::Begin("Anim_List");
	if (!m_Objects.empty())
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
	if (m_HumanRoot)
	{
		CModel* pModel = m_HumanRoot->Get_MainModel();
		if (pModel)
		{
			for (_uint i = 0; i < pModel->Get_AnimSize(); i++)
			{
				if (GUI::Button(pModel->Get_AnimList(i)))
				{
					m_HumanRoot->Set_Animation(i);
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
			m_pGameInstance->Get_Layer(NEXT_LEVEL, m_wszPreLayer)->Clear_Layer();
			m_Objects.pop_back();
		}
	}
	GUI::End();

	GUI::Begin("PartsObject_Info");
	if (m_HumanRoot)
	{
		CModel* pModel = m_HumanRoot->Get_MainModel();
		if (pModel)
		{
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

			if (GUI::Button("Add Event"))
			{
				m_fAnimFrame = m_HumanRoot->Get_MainModel()->Get_CurrentTrackPosition();
			}

			GUI::DragFloat("Anim Frame", &m_fAnimFrame);
			GUI::SameLine();
			if(GUI::Button("Event"))
			{
				ImDrawList* draw = ImGui::GetForegroundDrawList();
				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				float scale = 50.f;

				draw->AddLine(center, ImVec2(center.x + scale, center.y), IM_COL32(255, 0, 0, 255), 2.0f);
				draw->AddLine(center, ImVec2(center.x, center.y - scale), IM_COL32(0, 255, 0, 255), 2.0f);
				draw->AddLine(center, ImVec2(center.x + scale * 0.5f, center.y + scale * 0.5f), IM_COL32(0, 0, 255, 255), 2.0f);
			}

			if (GUI::Button("Delete"))
			{
				m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_Root"))->Clear_Layer();
				m_HumanRoot = nullptr;
			}
		}

		if (m_HumanRoot)
			m_HumanRoot->Get_ModelParts(ENUM_CLASS(CRootModelPart::PARTSTYPE::HAIR))->Describe_Entity();
	}

	GUI::End();
}

void CLevel_ObjectViewer::Save_LayerName(const _char* Layer)
{
	_wstring szLayer = TEXT("Layer_");

	_wstring wstrPath = szLayer + CMyTools::ToWstring(Layer);

	wcscpy_s(m_wszLayer, wstrPath.c_str());
}

_wchar* CLevel_ObjectViewer::Save_ModelName(const _char* Category)
{
	_splitpath_s(Category, nullptr, 0, nullptr, 0, m_szName, MAX_PATH, nullptr, 0);

	_wstring szDir = TEXT("Prototype_Component_");
	_wstring szDir2 = TEXT("_Model");

	wcscpy_s(m_wszName, szDir.c_str());
	wcscat_s(m_wszName, CMyTools::ToWstring(m_szName).c_str());
	wcscat_s(m_wszName, szDir2.c_str());

	return m_wszName;
}

HRESULT CLevel_ObjectViewer::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CDebugCamera::CAMERA_DEBUG_DESC			CameraDesc{};
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.vEye = _float3(0.f, 30.f, -10.f);
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
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_Cube>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE)))
		return E_FAIL;

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
