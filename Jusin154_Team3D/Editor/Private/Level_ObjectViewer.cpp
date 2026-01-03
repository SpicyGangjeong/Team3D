#include "pch.h"
#include "Level_ObjectViewer.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "RootModelPart.h"
#include "Body.h"
#include "Head.h"
#include "Hair.h"
#include "Dummy_Cube.h"
#include "Camera_Debug.h"
#include "Layer.h"
#include "GameObject.h"
#include "DummyObject.h"
#include "EffectPool.h"

#include "DummySkyBox.h"
#include "MainLight.h"
#include "Terrain.h"
#include "Player.h"
#include "Goblin.h"
#include "Broom.h"
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

	Dummy_Object_Setting();

	Parts_Object_Setting();

	Find_Anim();
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
			if (GUI::BeginTabItem("Body"))
			{
				Category_PartsModelList("Human/Body/Male", "Body");

				GUI::EndTabItem();
			}
			
			_bool bDisabled = {};
			if (m_HumanRoot)
				bDisabled = (m_HumanRoot->Get_MainModel());
			if (!bDisabled) {
				GUI::BeginDisabled();
			}
			if (GUI::BeginTabItem("Head"))
			{
				Category_PartsModelList("Human/Head/Male", "Head");

				GUI::EndTabItem();
			}

			if (GUI::BeginTabItem("Hair"))
			{
				Category_PartsModelList("Human/Hair/Male", "Hair");

				if (m_HumanRoot)
					m_HumanRoot->Get_ModelParts(ENUM_CLASS(CRootModelPart::PARTSTYPE::HAIR))->Describe_Entity();

				GUI::EndTabItem();
			}
			if (!bDisabled) {
				GUI::EndDisabled();
			}
			GUI::EndTabBar();
		}
		GUI::EndTabItem();
	}

	if (GUI::BeginTabItem("FeMale"))
	{
		if (GUI::BeginTabBar("##FeMaleTabBar")) 
		{
			if (GUI::BeginTabItem("Body"))
			{
				Category_PartsModelList("Human/Body/FeMale", "Body");

				GUI::EndTabItem();
			}
			
			_bool bDisabled = {};
			if (m_HumanRoot)
				bDisabled = (m_HumanRoot->Get_MainModel());
			if (!bDisabled) {
				GUI::BeginDisabled();
			}
			if (GUI::BeginTabItem("Head"))
			{
				Category_PartsModelList("Human/Head/FeMale", "Head");

				GUI::EndTabItem();
			}

			if (GUI::BeginTabItem("Hair"))
			{
				Category_PartsModelList("Human/Hair/FeMale", "Hair");

				if (m_HumanRoot)
					m_HumanRoot->Get_ModelParts(ENUM_CLASS(CRootModelPart::PARTSTYPE::HAIR))->Describe_Entity();

				GUI::EndTabItem();
			}
			if (!bDisabled) {
				GUI::EndDisabled();
			}
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

		Category_ModelList("Object");

		Category_ModelList("VFX");
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
						strcpy_s(m_DummyPath, szCategory);
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
					CGameObject* pTempObject = { nullptr };

					CDummyObject::PARTS_OBJECT_DESC Desc = {};

					m_HumanRoot->Set_PrototypeModelName(Save_ModelName(szCategory));

					m_HumanRoot->Set_ModelName(szCategory);

					_uint iIndex = 0;
					if (strstr(szCategory,"Body"))
						iIndex = ENUM_CLASS(CRootModelPart::PARTSTYPE::BODY);
					else if (strstr(szCategory,"Head"))
						iIndex = ENUM_CLASS(CRootModelPart::PARTSTYPE::HEAD);
					else if (strstr(szCategory,"Hair"))
						iIndex = ENUM_CLASS(CRootModelPart::PARTSTYPE::HAIR);

					m_HumanRoot->Change_Model(iIndex);

					filesystem::path fullPath(szCategory);
					filesystem::path folder = fullPath.parent_path();
					_string baseName = fullPath.stem().string();

					_string defaultModel = fullPath.filename().string();

					for (auto& entry : filesystem::directory_iterator(folder))
					{
						if (!entry.is_regular_file())
							continue;

						auto file = entry.path().filename().string();
						auto stem = entry.path().stem().string();
						auto ext = entry.path().extension().string();

						if (ext != ".png")
							continue;

						if (strstr(stem.c_str(),"DAO_MSK"))
						{
							_string Name = folder.string() +"/" + file;

							const _wchar* Prototype = TEXT("DAO_MSK");

							if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, Prototype,
								CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, CMyTools::ToWstring(Name).c_str(), 0)))) {
								return;
							}

							m_HumanRoot->Set_Texture(iIndex, Prototype);
						}
						if (strstr(stem.c_str(), "THV_MSK"))
						{
							_string Name = folder.string() + "/" + file;

							const _wchar* Prototype = TEXT("THV_MSK");

							if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, Prototype,
								CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, CMyTools::ToWstring(Name).c_str(), 0)))) {
								return;
							}

							m_HumanRoot->Set_Texture(iIndex, Prototype);
						}
					}
				}
			}
		}
	}
}

void CLevel_ObjectViewer::Show_AnimList()
{
	GUI::Begin("Anim_List");
	if (GUI::BeginTabBar("Anim_List"))
	{
		if (GUI::BeginTabItem("Dummy_Anim"))
		{
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
			GUI::EndTabItem();
		}
		
		if (GUI::BeginTabItem("Parts_Anim"))
		{
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
			GUI::EndTabItem();
		}
	}
	GUI::EndTabBar();
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

void CLevel_ObjectViewer::Dummy_Object_Setting()
{
	GUI::Begin("Object_Info");
	if (!m_Objects.empty())
	{
		_float4 Pos;
		XMStoreFloat4(&Pos, m_Objects[m_iObjectIndex]->Get_Component<CTransform>()->Get_State(STATE::POSITION));
		if (GUI::DragFloat3("Pos", (_float*)&Pos))
		{
			m_Objects[m_iObjectIndex]->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMLoadFloat4(&Pos));
		}

		Show_ObjectList();

		CModel* pModel = m_Objects[m_iObjectIndex]->Get_Component<CModel>();

		if (pModel->Get_AnimSize() <= 0)
			return;
		GUI::Button(pModel->Get_AnimList(pModel->Get_AnimIndex()));

		GUI::SameLine();

		_char label[24];

		sprintf_s(label, "%s %d", "AnimIndex", pModel->Get_AnimIndex());

		GUI::Text(label);

		_float AnimTrack = pModel->Get_CurrentTrackPosition();
		if (GUI::DragFloat("AnimTrack", &AnimTrack))
		{
			if(AnimTrack>=0.f)
				pModel->Set_CurrentTrackPosition(AnimTrack);
		}

		_float AnimRatio = pModel->Get_CurrentTrackProgressRatio();
		GUI::DragFloat("AnimRatio", &AnimRatio);

		_float AnimSpeed = pModel->Get_AnimSpeed();
		GUI::DragFloat("AnimSpeed", &AnimSpeed, 0.1f);
		pModel->Set_AnimSpeed(AnimSpeed);

		_bool bPlayAnim = pModel->Get_PlayAnim();
		if (GUI::Checkbox("Play Anim", &bPlayAnim))
		{
			pModel->Set_PlayAnim(bPlayAnim);
		}

		if (GUI::Button("Load KeyFrame"))
		{
			_char szName[MAX_PATH] = {};
			_splitpath_s(m_DummyPath, nullptr, 0, nullptr, 0, szName, MAX_PATH, nullptr, 0);

			Load_KeyFrame(szName);
		}

		static _char EventName[64] = {};

		GUI::InputText("EventName", EventName, sizeof(EventName));
		GUI::SameLine();

		if (GUI::Button("Save KeyFrame"))
		{
			if (pModel)
			{
				_float fKeyFrame = pModel->Get_CurrentTrackPosition();
				m_KeyFrames.emplace(EventName, fKeyFrame);

				Save_KeyFrame();

				EventName[0] = '\0';
			}
		}


		for (auto& iter : m_KeyFrames)
		{
			string keyName = iter.first;

			_bool isSelected = (m_SelectedKey == keyName);

			if (GUI::Selectable(keyName.c_str(), isSelected))
			{
				m_SelectedKey = keyName;
			}

			if (isSelected)
			{
				GUI::Indent();

				GUI::DragFloat("KeyFrame", &iter.second, 0.01f);

				if (GUI::Button("Delete KeyFrame"))
				{
					m_KeyFrames.erase(keyName);
					m_SelectedKey.clear();   
					break;                     
				}

				GUI::Unindent();
			}
		}


		if (GUI::Button("Delete"))
		{
			m_pGameInstance->Get_Layer(NEXT_LEVEL, m_wszPreLayer)->Clear_Layer();
			m_Objects.pop_back();

			m_KeyFrames.clear();
		}
	}
	GUI::End();
}

void CLevel_ObjectViewer::Parts_Object_Setting()
{
	GUI::Begin("PartsObject_Info");
	if (m_HumanRoot)
	{
		CModel* pModel = m_HumanRoot->Get_MainModel();
		if (pModel)
		{
			_float3 Pos;
			XMStoreFloat3(&Pos, m_HumanRoot->Get_WorldPostion());

			float Pos3[3] = { Pos.x, Pos.y, Pos.z };
			GUI::DragFloat3("Pos", Pos3);

			Pos.x = Pos3[0];
			Pos.y = Pos3[1];
			Pos.z = Pos3[2];

			m_HumanRoot->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(Pos.x, Pos.y, Pos.z, 1.f));

			GUI::Button(pModel->Get_AnimList(pModel->Get_AnimIndex()));

			_float KeyFrame = pModel->Get_CurrentTrackPosition();
			GUI::DragFloat("KeyFrame", &KeyFrame);

			_float AnimSpeed = pModel->Get_AnimSpeed();
			GUI::DragFloat("AnimSpeed", &AnimSpeed, 0.1f);
			pModel->Set_AnimSpeed(AnimSpeed);

			_bool bPlayAnim = pModel->Get_PlayAnim();
			if (GUI::Checkbox("Play Anim", &bPlayAnim))
			{
				pModel->Set_PlayAnim(bPlayAnim);
			}

			if (GUI::Button("Load KeyFrame"))
			{
				Load_KeyFrame("Human");
			}

			static _char EventName[64] = {};

			GUI::InputText("EventName", EventName, sizeof(EventName));
			GUI::SameLine();

			if (GUI::Button("Save KeyFrame"))
			{
				if (m_HumanRoot && m_HumanRoot->Get_MainModel())
				{
					_float fKeyFrame = m_HumanRoot->Get_MainModel()->Get_CurrentTrackPosition();
					m_KeyFrames.emplace(EventName, fKeyFrame);

					FILE* fp = nullptr;
					fopen_s(&fp, "../Bin/Resources/Models/Human/KeyFrame.bin", "wb");
					if (!fp) {
						return;
					}

					_uint KeyFrameSize = (_uint)m_KeyFrames.size();

					fwrite(&KeyFrameSize, sizeof(_uint), 1, fp);

					for (auto& iter : m_KeyFrames)
					{
						_uint EventSize = (_uint)iter.first.size();
						fwrite(&iter.second, sizeof(_float), 1, fp);
						fwrite(&EventSize, sizeof(_uint), 1, fp);
						fwrite(iter.first.c_str(), sizeof(_char), EventSize, fp);
					}


					fclose(fp);

					EventName[0] = '\0';
				}
			}
			for (auto& iter : m_KeyFrames)
			{
				ImDrawList* draw = GUI::GetForegroundDrawList();
				ImVec2 center = GUI::GetMainViewport()->GetCenter();
				float scale = 50.f;

				GUI::DragFloat(iter.first.c_str(), &iter.second);
			}


			if (GUI::Button("Delete"))
			{
				m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_Root"))->Clear_Layer();
				m_HumanRoot = nullptr;
				m_KeyFrames.clear();
			}
		}
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

void CLevel_ObjectViewer::Load_KeyFrame(const _char* Name)
{
	FILE* fp = nullptr;
	_string Path = {};
	if (Name == _string("Human"))
	{
		Path = "../Bin/Resources/Models/Human/KeyFrame.bin";
	}
	else
	{
		_char szDir[MAX_PATH] = {};
		_splitpath_s(m_DummyPath, nullptr, 0, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
		Path = _string(szDir) + "KeyFrame.bin";
	}

	fopen_s(&fp, Path.c_str(), "rb");
	if (!fp) {
		return;
	}
	_uint KeyFrameSize = 0;
	fread(&KeyFrameSize, sizeof(_uint), 1, fp);

	for (size_t i = 0; i < KeyFrameSize; i++)
	{
		float fKey = 0.f;
		fread(&fKey, sizeof(float), 1, fp);

		_uint EventLen = 0;
		fread(&EventLen, sizeof(_uint), 1, fp);

		_string str;
		str.resize(EventLen);

		fread(str.data(), sizeof(_char), EventLen, fp);
		m_KeyFrames.emplace(str, fKey);
	}

	fclose(fp);
}

void CLevel_ObjectViewer::Find_Anim()
{
	if (!m_Objects.empty())
	{
		CModel* pModel = m_Objects[m_iObjectIndex]->Get_Component<CModel>();
		if (pModel->Get_AnimSize() <= 0)
			return;
		GUI::InputInt("FindAnimIndex", &m_iAnimIndex);



		if (m_iAnimIndex <= pModel->Get_AnimSize())
		{
			if (GUI::Button(pModel->Get_AnimList(m_iAnimIndex)))
			{
				pModel->Set_AnimationIndex(m_iAnimIndex);
			}
		}


		GUI::InputText("FindAnim", m_FindAnimName, sizeof(m_FindAnimName));


		for (_uint i = 0; i < pModel->Get_AnimSize(); i++)
		{
			if (strstr(pModel->Get_AnimList(i), m_FindAnimName))
			{
				if (GUI::Button(pModel->Get_AnimList(i)))
				{
					pModel->Set_AnimationIndex(i);
				}
			}
		}
	}

}

void CLevel_ObjectViewer::Save_KeyFrame()
{
	_char szDir[MAX_PATH] = {};

	_splitpath_s(m_DummyPath, nullptr, 0, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

	_string szFile = "KeyFrame.bin";
	_string Path = szDir + szFile;

	FILE* fp = nullptr;
	fopen_s(&fp, Path.c_str(), "wb");
	if (!fp) {
		return;
	}

	_uint KeyFrameSize = (_uint)m_KeyFrames.size();

	fwrite(&KeyFrameSize, sizeof(_uint), 1, fp);

	for (auto& iter : m_KeyFrames)
	{
		_uint EventSize = (_uint)iter.first.size();
		fwrite(&iter.second, sizeof(_float), 1, fp);
		fwrite(&EventSize, sizeof(_uint), 1, fp);
		fwrite(iter.first.c_str(), sizeof(_char), EventSize, fp);
	}


	fclose(fp);
}


HRESULT CLevel_ObjectViewer::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Debug::CAMERA_DEBUG_DESC            Camera_Desc{};
	Camera_Desc.fFovy = XMConvertToRadians(60.0f);
	Camera_Desc.fNear = 0.1f;
	Camera_Desc.fFar = 200.f;
	Camera_Desc.vEye = _float3(0.f, 10.f, -10.f);
	Camera_Desc.vAt = _float3(0.f, 0.f, 0.f);
	Camera_Desc.fSpeedPerSec = 5.f;
	Camera_Desc.pCameraKey = CAMERA_DEBUG;
	Camera_Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Camera_Desc.fMouseSensor = 0.1f;
	Camera_Desc.iPriority = 70;
	Camera_Desc.pFollowTarget = { nullptr };
	Camera_Desc.pLookTarget = { nullptr };

	CCamera_Debug* pCamera = { nullptr };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCamera_Debug>(g_iStaticLevel, NEXT_LEVEL, LAYER_CAMERA, &Camera_Desc, nullptr, &pCamera))) {
		return E_FAIL;
	}
	m_pGameInstance->Add_Camera(g_iStaticLevel, pCamera, CAMERA_DEBUG);
	if (FAILED(m_pGameInstance->Bind_Camera(g_iStaticLevel, CAMERA_DEBUG, true))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_ObjectViewer::Ready_Layer_Light()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMainLight>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_LIGHT)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_ObjectViewer::Ready_Layer_UI(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_ObjectViewer::Ready_Layer_Dummy(const _wstring& strLayerTag)
{


	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEffectPool>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_EffectPool")))) //플레이어보다 먼저 생성해야함!
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CPlayer>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, nullptr, nullptr, &m_Test)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummySkyBox>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE)))
		return E_FAIL;



	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTerrain>(g_iStaticLevel, NEXT_LEVEL, strLayerTag)))
		return E_FAIL;*/



	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CGoblin>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, m_Test)))
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
