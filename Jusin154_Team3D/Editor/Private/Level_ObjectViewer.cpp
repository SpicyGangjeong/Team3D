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
		Add_Human();

	}
	GUI::EndTabBar();
	GUI::End();

}

void CLevel_ObjectViewer::Add_Human()
{
	if (GUI::BeginTabItem("Body"))
	{
		Category_PartsModelList("Human/Body","Body");

		GUI::EndTabItem();
	}

	if (GUI::BeginTabItem("Head"))
	{
		_bool bDisabled = (m_HumanRoot == nullptr);
		if (bDisabled) GUI::BeginDisabled();

		Category_PartsModelList("Human/Head", "Head");

		if (bDisabled) GUI::EndDisabled();
		GUI::EndTabItem();
	}

	if (GUI::BeginTabItem("Hair"))
	{
		_bool bDisabled = (m_HumanRoot == nullptr);
		if (bDisabled) GUI::BeginDisabled();

		Category_PartsModelList("Human/Hair", "Hair");

		if (bDisabled) GUI::EndDisabled();
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
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRootModelPart>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, TEXT("Layer_Root"), nullptr, nullptr, reinterpret_cast<CRootModelPart**>(&m_HumanRoot))))
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

					Desc.pModelPrototypeTag = Save_ModelName(szCategory);

					dynamic_cast<CRootModelPart*>(m_HumanRoot)->Set_PrototypeModelName(Desc.pModelPrototypeTag);
					dynamic_cast<CRootModelPart*>(m_HumanRoot)->Set_ModelName(szCategory);
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
		CModel* pModel = dynamic_cast<CRootModelPart*>(m_HumanRoot)->Get_ModelParts(0)->Get_Component<CModel>();
		if (pModel)
		{
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
			m_pGameInstance->Get_Layer(NEXT_LEVEL, m_wszPreLayer)->Clear_Layer();
			m_Objects.pop_back();
		}
	}
	GUI::End();

	GUI::Begin("PartsObject_Info");
	if (m_HumanRoot)
	{
		CModel* pModel = dynamic_cast<CRootModelPart*>(m_HumanRoot)->Get_ModelParts(0)->Get_Component<CModel>();
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
			if (GUI::Button("Delete"))
			{
				m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_Root"))->Clear_Layer();
				m_HumanRoot = nullptr;
			}
		}
	}

	GUI::End();
}

void CLevel_ObjectViewer::Save_LayerName(const _char* Category)
{
	_wstring szLayer = TEXT("Layer_");

	_wchar szCategory[MAX_PATH];

	wcscpy_s(m_wszLayer, szLayer.c_str());

	MultiByteToWideChar(CP_ACP, 0, Category, -1, szCategory, MAX_PATH);

	wcscat_s(m_wszLayer, szCategory);
}

_wchar* CLevel_ObjectViewer::Save_ModelName(const _char* Category)
{
	_splitpath_s(Category, nullptr, 0, nullptr, 0, m_szName, MAX_PATH, nullptr, 0);

	_wstring szDir = TEXT("Prototype_Component_");
	_wstring szDir2 = TEXT("_Model");

	_wchar wszName[MAX_PATH] = { 0 };

	MultiByteToWideChar(CP_ACP, 0, m_szName, -1, wszName, MAX_PATH);

	wcscpy_s(m_wszName, szDir.c_str());
	wcscat_s(m_wszName, wszName);
	wcscat_s(m_wszName, szDir2.c_str());

	return m_wszName;
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
