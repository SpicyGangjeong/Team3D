#include "pch.h"
#include "Camera_Debug.h"
#include "Client_Struct.h"
#include "GameInstance.h"
#include "Light_Main.h"
#include "Loader.h"
#include "MainApp.h"
#include "Model.h"

#include "Broom.h"
#include "Camera_Gaze.h"
#include "CamPosition_Arm.h"
#include "CamPosition_Shoulder.h"
#include "CamPosition_Socket.h"
#include "CamPosition_Target.h"
#include "SkyBox.h"
#include "Troll_Weapon.h"
#include "Troll_Rock.h"
#include "Goblin_Dagger.h"
#include "Wand.h"

#pragma region ACTOR

#include "Player.h"
#include "Goblin.h"
#include "Troll.h"

#pragma endregion


#pragma region UI

#include "Skill_Data.h"
#include "UI_Manager.h"

#include "GamePlay_Canvas.h"

#include "Mission_Panel.h"
//#include "MissionBanner_Border.h"
//#include "MissionBanner_Key.h"
//#include "Mission_Icon.h"
//#include "Mouse_Cursor.h"
#include "MiniMap_TrimBorder.h"

#include "MiniMap_Panel.h"
#include "NoMountIcon.h"

#include "Loading_Panel.h"
#include "LoadingWidget.h"
#include "LoadingWidget_Flame.h"

#include "Action_Panel.h"
#include "Spell_Slot.h"
#include "Spell_Overlay.h"
#include "Slot_Number.h"
#include "HpBarBG.h"
#include "Potion.h"
#include "Magic_Meter.h"
#include "Magic_Icon.h"
#include "Spell_UI.h"
#include "Magic_Item.h"

#include "Spell_Canvas.h"
#include "Spell_Panel.h"
#include "Current_Spell_Slot.h"
#include "Spell_List.h"
#include "Eessential_Spell_Slot.h"
#include "Eessential_Spell.h"
#include "Spell_List_Image.h"
#include "Spell_State.h"
#include "Spell_Hover.h"
#include "Spell_Hover_Effect.h"
#include "Spell_Preview.h"
#include "Spell_Header.h"
#include "Spell_Header_Line.h"
#include "Spell_Vidio_Border.h"
#include "Spell_Anim.h"
#include "Current_Slot_Number.h"
#include "Spell_Drag.h"


#pragma endregion

#pragma region PHYSX

#include "Dummy_PhysXWall.h"
#include "Dummy_PhysXPlayable.h"
#include "PhysXEffectHitBox.h"

#pragma endregion

#pragma region EFFECT

#include "EffectParts.h"
#include "Bombard.h"
#include "Decendo.h"
#include "NomalJap.h"
#include "Protego.h"
#include "Revelio.h"
#include "NomalJapSide.h"
#include "DecendoSide.h"
#include "BombardSide.h"
#include "LeviosoSide.h"

#include "TrailObject.h"
#include "Instance_Model.h"
#include "Trail.h"
#include "EffectPool.h"
#include "Levioso.h"
#include "Lumos.h"



#pragma endregion

#pragma region MAP

#include "Terrain.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Model_Instance.h"
#include "BuildingContainer.h"
#include "StreetContainer.h"
#include "MapObject_Render.h"
#include "MapObject_Collision.h"
#include "InstancedProp.h"
#include "MapElement_Light.h"
#include "MapElement_Interactable.h"

#pragma endregion


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
}

unsigned int APIENTRY LoadingMain(void* pArg)
{
	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading())) {
		assert(false);
		int a = 0;
		return 1;
	}

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	m_pGameInstance->EnqueueJob(&LoadingMain, this);

	return S_OK;
}

HRESULT CLoader::Loading()
{

	if (FAILED(CoInitializeEx(nullptr, 0))) {
		return E_FAIL;
	}

	HRESULT		hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Loading_For_Logo();
		break;
	case LEVEL::GAMEPLAY:
		hr = Loading_For_GamePlay();
		break;
	case LEVEL::FIELD:
		hr = Loading_For_Field();
		break;
	case LEVEL::RESTART:
		hr = Loading_For_Restart();
		break;
	default:
		break;
	}

	CoUninitialize();

	if (FAILED(hr)){
		return E_FAIL;
	}

	return S_OK;
}

void CLoader::Output()
{
	SetWindowText(g_hWnd, m_strMessage.c_str());
}

HRESULT CLoader::Loading_For_Logo()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("사운드를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	if (FAILED(m_pGameInstance->Add_Prototype<CCamera_Debug>(g_iStaticLevel, CCamera_Debug::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLight_Main>(g_iStaticLevel, CLight_Main::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

#pragma region MAP_MODELS
	vector<future<vector<FOLDER_LOAD*>*>> jobMapModels;
	{
		{ /* Terrain */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Collision/Terrain",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/Terrain",
				".bin", false
			));
		}

		{ /* Ollivanders*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Ollivanders/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Ollivanders/Collision",
				".bin", false
			));
		}

		{ /* Gatehouse*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Gatehouse/Meshes",
				".bin", false
			));
		}

		{ /* TScrolls */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_TScrolls/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_TScrolls/Collisions",
				".bin", false
			));
		}
		{ /* 3BroomStick*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_ThreeBroomsticks/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_ThreeBroomsticks/Collision",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_ThreeBroomsticks/Meshes/3Broom_Kit",
				".bin", false
			));
		}


		{ /* BLDG_QuidditchShop */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_QuidditchShop/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_QuidditchShop/Collision",
				".bin", false
			));
		}
		{ /* BLDG_HogsheadInn */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_HogsheadInn/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_HogsheadInn/Collision",
				".bin", false
			));
		}
		{ /* BLDG_Honeydukes */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Honeydukes/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Honeydukes/Collision",
				".bin", false
			));
		}
		{ /* BLDG_OwlPost */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_OwlPost/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_OwlPost/Collision",
				".bin", false
			));
		}
		{ /* BLDG_TeaShop */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_TeaShop/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_TeaShop/Collision",
				".bin", false
			));
		}
		{ /* BLDG_Zonkos */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Zonkos/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Zonkos/Collision",
				".bin", false
			));
		}
		{ /* BLDG_DB_GR */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_DB_GR/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_DB_GR/Collision",
				".bin", false
			));
		}
		{ /* BLDG_Potions */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Potions/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Potions/Collisions",
				".bin", false
			));
		}
		{ /* BLDG_Salon */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Salon/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Salon/Collision",
				".bin", false
			));
		}
		{ /* Hengist_Tree */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/Vegetation/Hengist_Tree",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Collision/Vegetation",
				".bin", false
			));
		}
		{ /* GEN A*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_A/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_A/Collision",
				".bin", false
			));
		}
		{ /* GEN B*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_B/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_B/Collision",
				".bin", false
			));
		}
		{ /* GEN C*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_C/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_C/Collision",
				".bin", false
			));
		}
		{ /* GEN E*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_E/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_E/Collision",
				".bin", false
			));
		}
		{ /* GEN F*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_F/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_F/Collision",
				".bin", false
			));
		}
		{ /* GEN G*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_G/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_G/Collision",
				".bin", false
			));
		}
		{ /* GEN H*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_H/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_H/Collision",
				".bin", false
			));
		}
		{ /* GEN J*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_J/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_J/Collision",
				".bin", false
			));
		}
		{/* Light Objects */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/LightPosts",
				".bin", false
			));
		}
		{/* Doors */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/Doors",
				".bin", false
			));
		}
		{/* Step and Stair */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/GroundSurfaces",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Collision/GroundSurfaces",
				".bin", false
			));
		}
		{/* Barrel */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Objects/Meshes",
				".bin", false
			));
			/* Box */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/MiscProps",
				".bin", false
			));
			/* TeaShop Table */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/Tables",
				".bin", false
			));
			/* TeaShop Chair*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/Chairs",
				".bin", false
			));
		}
	}

#pragma endregion


#pragma region UI

	Asset_FileLoad("../Bin/Resources/Textures/Keyboard", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Action", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Cursor"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Cursor/UI_T_CursorRings.dds"), 0)))) {
		return E_FAIL;
	}

	Asset_FileLoad("../Bin/Resources/Textures/Mission", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/SpellWidget", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Mission_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Mission/Mission_Icon_%d.png"), 2)))) {
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("ActiveMission_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Mission/ActiveMission_Icon_%d.png"), 2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Megic_Metar"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Action/Megic_Meter%d.png"), 3)))) {
		return E_FAIL;
	}


	Asset_FileLoad("../Bin/Resources/Textures/MiniMap", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Loading", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Spells", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Spells/SpellMeters", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Potions", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/GadgetWheel", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/GadgetWheel", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Item"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/GadgetWheel/Item%d.png"), 8)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Accio"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Accio\\Accio%d.png"), 151))))
	{
		return E_FAIL;
	}
#pragma region UI_ANI
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Altering_Spell"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Altering_Spell\\Altering_Spell%d.png"), 349))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Alohomora"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Alohomora\\Alohomora%d.png"), 394))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Ancient_Magic"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Ancient_Magic\\Ancient_Magic%d.png"), 152))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Ancient_Magic_Throw"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Ancient_Magic_Throw\\Ancient_Magic_Throw%d.png"), 89))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Arresto_Momentum"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Arresto_Momentum\\Arresto_Momentum%d.png"), 120))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Avada_Kedavra"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Avada_Kedavra\\Avada_Kedavra%d.png"), 208))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Basic_Cast"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Basic_Cast\\Basic_Cast%d.png"), 308))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Beast_Feed"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Beast_Feed\\Beast_Feed%d.png"), 378))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Beast_Petting_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Beast_Petting_Brush\\Beast_Petting_Brush%d.png"), 327))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Bombarda"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Bombarda\\Bombarda%d.png"), 262))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Confringo"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Confringo\\Confringo%d.png"), 157))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Conjuring_Spell"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Conjuring_Spell\\Conjuring_Spell%d.png"), 115))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Descendo"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Descendo\\Descendo%d.png"), 155))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Depulso"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Depulso\\Depulso%d.png"), 152))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Diffindo"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Diffindo\\Diffindo%d.png"), 150))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Disillusionment"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Disillusionment\\Disillusionment%d.png"), 260))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Evanesco"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Evanesco\\Evanesco%d.png"), 136))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Expelliarmus"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Expelliarmus\\Expelliarmus%d.png"), 146))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Flipendo"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Flipendo\\Flipendo%d.png"), 188))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Glacius"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Glacius\\Glacius%d.png"), 156))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Imperio"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Imperio\\Imperio%d.png"), 462))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Incendio"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Incendio\\Incendio%d.png"), 213))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Levioso"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Levioso\\Levioso%d.png"), 180))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Lumos"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Lumos\\Lumos%d.png"), 365))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Nab_Sack"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Nab_Sack\\Nab_Sack%d.png"), 235))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Petrificus_Totalus"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Petrificus_Totalus\\Petrificus_Totalus%d.png"), 275))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Protego"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Protego\\Protego%d.png"), 285))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Reparo"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Reparo\\Reparo%d.png"), 385))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Revelio"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Revelio\\Revelio%d.png"), 227))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Stupefy"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Stupefy\\Stupefy%d.png"), 222))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Transformation"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Transformation\\Transformation%d.png"), 146))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Wingardium_Leviosa"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Wingardium_Leviosa\\Wingardium_Leviosa%d.png"), 273))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Crucio"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Crucio\\Crucio%d.png"), 216))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region TERRAIN_TEXTURE
	/* Terrain_Diffuse */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Terrain_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Terrain/Terrain_D_%d.dds"), 4)))) {
		return E_FAIL;
	}
	/* Terrain_Normal */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Terrain_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Terrain/Terrain_N_%d.dds"), 4)))) {
		return E_FAIL;
	}
	/* Terrain_MRO */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Terrain_MRO"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Terrain/Terrain_MRO_%d.dds"), 4)))) {
		return E_FAIL;
	}
	/* Terrain_AlphaMap */
	if(FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("HogsmeadeAlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Terrain/Hogsmeade_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}

	/* For.Prototype_Component_LightPost_Emissive */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("LightPost_Emissive"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("C:/MeshTable/Game/Environment/Hogsmeade/Common/Textures/LightPosts/T_HM_LampPost_Glass_E.png"), 0)))) {
		return E_FAIL;
	}

	/* For.Prototype_Component_LightPost_Mask */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("LightPost_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("C:/MeshTable/Game/Environment/Hogsmeade/Common/Textures/LightPosts/T_HM_LampPost_Glass_D.png"), 0)))) {
		return E_FAIL;
	}

	/* For.Prototype_Component_LightPost_Base */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("LightPost_Base"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("C:/MeshTable/Game/Environment/Hogsmeade/Common/Textures/LightPosts/T_HM_LampPost_Glass_Dark_D.png"), 0)))) {
		return E_FAIL;
	}
#pragma endregion

#pragma region EFFECT

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Trails", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;


		});



	Asset_FileLoad("../Bin/Resources/Textures/Effect/Noises", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Diffuse", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Flipbooks", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Gradients", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

#pragma endregion

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_UIEDITOR,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_UIEditor.hlsl"),
			VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_UIINSTANCE,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_UI_Instance.hlsl"),
			VTX_POSTEX_INSTANCE_UI::Elements, VTX_POSTEX_INSTANCE_UI::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_INSTANCE_MODEL,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxModelInstance.hlsl"),
			VTX_MODEL_INSTANCE_PARTICLE::Elements, VTX_MODEL_INSTANCE_PARTICLE::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_INSTANCE_PROP_MODEL,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxWorldModelInstance.hlsl"),
			VTX_MODEL_INSTANCE_MODEL::Elements, VTX_MODEL_INSTANCE_MODEL::iNumElements)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_VTXPOS,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxPos.hlsl"),
			VTXPOS::Elements, VTXPOS::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_MESH,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxMesh.hlsl"),
			VTXMESH::Elements, VTXMESH::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_ANIMMESH,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxAnimMesh.hlsl"),
			VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_POSTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxPosTex.hlsl"),
			VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_NORTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxNorTex.hlsl"),
			VTXNORTEX::Elements, VTXNORTEX::iNumElements)))) {
		return E_FAIL;
	}

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC SpellSlotUIDesc{};

	SpellSlotUIDesc.iNum = 4;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Spell_Slot"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &SpellSlotUIDesc)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC CurrentSpellSlotUIDesc{};

	CurrentSpellSlotUIDesc.iNum = 16;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Current_Spell_Slot"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &CurrentSpellSlotUIDesc)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC Spell_List{};

	Spell_List.iNum = 26;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Spell_List"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &Spell_List)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC Eessential_Spell_Slot{};

	Eessential_Spell_Slot.iNum = 8;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Eessential_Spell_Slot"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &Eessential_Spell_Slot)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC Eessential_Spell{};

	Eessential_Spell.iNum = 8;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Eessential_Spell"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &Eessential_Spell)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC Spell_List_Image{};

	Spell_List_Image.iNum = 26;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Spell_List_Image"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &Spell_List_Image)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC SlotNumberUIDesc{};

	SlotNumberUIDesc.iNum = 4;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Slot_Number_UI_Instance"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &SlotNumberUIDesc)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC Magic_MeterDesc{};

	Magic_MeterDesc.iNum = 5;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Magic_Meter_UI_Instance"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &Magic_MeterDesc)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC Spell_Slot_PreviewDesc{};

	Spell_Slot_PreviewDesc.iNum = 4;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Spell_Slot_Previe_UI_Instance"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &Spell_Slot_PreviewDesc)))) {
		return E_FAIL;
	}


	// Heavy Wall
	CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
	{
		Desc.eType = ACTOR::BOX;
		Desc.ePxRigidBodyFlags = { /*PSX::PxRigidBodyFlag::eKINEMATIC*/ };
		Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
		Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
		Desc.vMatInfo = { 0.5f, 0.5f, 0.6f };
		Desc.fContactOffset = { 0.05f };
		Desc.vhalfGeometryInfo = { 0.5f, 0.5f, 0.5f };
		Desc.fDensity = 1.f;
		Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
		Desc.eLockFlag = {};
		Desc.vAutoDamping = { 100.f, 100.f };
		Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
		Desc.vLocalTranslation = { 0.f, 0.f, 0.f };
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_HEAVY_WALL"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), CCharacter_Controller::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	Desc.ePxRigidBodyFlags = { PSX::PxRigidBodyFlag::eKINEMATIC };
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX_KIN"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Box"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Box/Box.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixIdentity())))) {
		return E_FAIL;
	}

#pragma region ACTOR

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Npc_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Human/Npc/Npc.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity())))){
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Goblin_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Goblin/Goblin.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity())))){
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Troll_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::PBR_ANIM, "../Bin/Resources/Models/Monster/SubTroll/troll.fbx", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity())))){
		return E_FAIL;
	}

#pragma endregion

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Wand_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Object/Wand/Wand.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Broom_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Object/Broom/Broom.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Troll_Weapon_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Object/Troll_Weapon/Troll_Weapon.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Troll_Rock_Big_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Object/Troll_Rock/Troll_Rock_Big.bin", XMMatrixScaling(0.00004f, 0.00004f, 0.00004f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Goblin_Dagger_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Object/Goblin_Dagger/Goblin_Dagger.bin", XMMatrixScaling(0.0002f, 0.0002f, 0.0002f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixIdentity()))))
		return E_FAIL;



	/* For.Prototype_Component_SkyboxModel */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_SkyboxModel"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/SkyBox/SkyBox.bin", XMMatrixIdentity()))))
		return E_FAIL;


	m_strMessage = TEXT("이펙트를(을) 로딩 중 입니다.");

	if (FAILED(m_pGameInstance->Add_Prototype<CTrail>(g_iStaticLevel, CTrail::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CEffectParts>(g_iStaticLevel, CEffectParts::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CTrailObject>(g_iStaticLevel, CTrailObject::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CBombard>(NEXT_LEVEL, CBombard::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDecendo>(NEXT_LEVEL, CDecendo::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CNomalJap>(NEXT_LEVEL, CNomalJap::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CProtego>(NEXT_LEVEL, CProtego::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CRevelio>(NEXT_LEVEL, CRevelio::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CNomalJapSide>(NEXT_LEVEL, CNomalJapSide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLevioso>(NEXT_LEVEL, CLevioso::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLumos>(NEXT_LEVEL, CLumos::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLeviosoSide>(NEXT_LEVEL, CLeviosoSide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CBombardSide>(NEXT_LEVEL, CBombardSide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CDecendoSide>(NEXT_LEVEL, CDecendoSide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}



	if (FAILED(m_pGameInstance->Add_Prototype<CEffectPool>(g_iStaticLevel, CEffectPool::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}


	m_strMessage = TEXT("Model Loading..");

	Asset_FileLoad("../Bin/Resources/Models/Effect/ParticleMesh", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Models/Effect/Goo", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Models/Effect/NomalMesh", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	/* For.Prototype_GameObject_UI_Manager*/
	if (FAILED(m_pGameInstance->Add_Prototype<CUI_Manager>(g_iStaticLevel, CUI_Manager::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_UI_Manager*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSkill_Data>(g_iStaticLevel, CSkill_Data::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_GamePlay_Canvas*/
	if (FAILED(m_pGameInstance->Add_Prototype<CGamePlay_Canvas>(g_iStaticLevel, CGamePlay_Canvas::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Action_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype<CAction_Panel>(g_iStaticLevel, CAction_Panel::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_Slot*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Slot>(g_iStaticLevel, CSpell_Slot::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_Overlay*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Overlay>(g_iStaticLevel, CSpell_Overlay::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Slot_Number*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSlot_Number>(g_iStaticLevel, CSlot_Number::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_HpBarBG*/
	if (FAILED(m_pGameInstance->Add_Prototype<CHpBarBG>(g_iStaticLevel, CHpBarBG::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Potion*/
	if (FAILED(m_pGameInstance->Add_Prototype<CPotion>(g_iStaticLevel, CPotion::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Magic_Meter*/
	if (FAILED(m_pGameInstance->Add_Prototype<CMagic_Meter>(g_iStaticLevel, CMagic_Meter::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Magic_Icon*/
	if (FAILED(m_pGameInstance->Add_Prototype<CMagic_Icon>(g_iStaticLevel, CMagic_Icon::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_UI>(g_iStaticLevel, CSpell_UI::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Magic_Item*/
	if (FAILED(m_pGameInstance->Add_Prototype<CMagic_Item>(g_iStaticLevel, CMagic_Item::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Loading_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype<CLoading_Panel>(g_iStaticLevel, CLoading_Panel::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	/* For.Prototype_GameObject_LoadingWidget*/
	if (FAILED(m_pGameInstance->Add_Prototype<CLoadingWidget>(g_iStaticLevel, CLoadingWidget::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	/* For.Prototype_GameObject_LoadingWidget_Flame*/
	if (FAILED(m_pGameInstance->Add_Prototype<CLoadingWidget_Flame>(g_iStaticLevel, CLoadingWidget_Flame::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Mission_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype<CMission_Panel>(g_iStaticLevel, CMission_Panel::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Minimap_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype<CMiniMap_Panel>(g_iStaticLevel, CMiniMap_Panel::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	/* For.Prototype_GameObject_MiniMap_TrimBorder*/
	if (FAILED(m_pGameInstance->Add_Prototype<CMiniMap_TrimBorder>(g_iStaticLevel, CMiniMap_TrimBorder::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	/* For.Prototype_GameObject_NoMountIcon*/
	if (FAILED(m_pGameInstance->Add_Prototype<CNoMountIcon>(g_iStaticLevel, CNoMountIcon::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Canvas>(g_iStaticLevel, CSpell_Canvas::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Panel>(g_iStaticLevel, CSpell_Panel::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CCurrent_Spell_Slot>(g_iStaticLevel, CCurrent_Spell_Slot::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_List>(g_iStaticLevel, CSpell_List::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CEessential_Spell_Slot>(g_iStaticLevel, CEessential_Spell_Slot::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CEessential_Spell>(g_iStaticLevel, CEessential_Spell::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_List_Image>(g_iStaticLevel, CSpell_List_Image::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_State>(g_iStaticLevel, CSpell_State::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Hover>(g_iStaticLevel, CSpell_Hover::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Hover_Effect>(g_iStaticLevel, CSpell_Hover_Effect::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Preview>(g_iStaticLevel, CSpell_Preview::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Header>(g_iStaticLevel, CSpell_Header::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Header_Line>(g_iStaticLevel, CSpell_Header_Line::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Vidio_Border>(g_iStaticLevel, CSpell_Vidio_Border::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Anim>(g_iStaticLevel, CSpell_Anim::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CCurrent_Slot_Number>(g_iStaticLevel, CCurrent_Slot_Number::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Drag>(g_iStaticLevel, CSpell_Drag::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* For.Prototype_GameObject_SkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype<CSkyBox>(g_iStaticLevel, CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region MAP PROTOTYPE
	/* For.Prototype_Component_VIBuffer_Terrain */
	{
		CVIBuffer_Terrain* pTerrain = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, "Hogsmeade_HeightMap.bin", 512, 512);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Terrain"), pTerrain))) {
			return E_FAIL;
		}
		pTerrain->ConvertToHeightField(TEXT("Hogsmeade_HeightMap"));

		CRigidBody_Static::RIGIDBODY_STATIC_PROTOTYPEDESC Desc{}; // 터레인 리지드 지형 추가
		{
			Desc.eType = ACTOR::HEIGHTFIELD;
			Desc.ePxRigidBodyFlags = {};
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = PXMATERIAL::DEFAULT;
			Desc.fContactOffset = 0.f;
			Desc.vMatInfo = { 1.2f, 1.0f, 0.0f };
		}
		CRigidBody_Static* pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_RigidBody_Static_Terrain_Hogsmeade"), pRigid))) {
			return E_FAIL;
		}
	}


#pragma region MAP_INSTANCE
	/* For.Prototype_Component_VIBuffer_Model_Instancel_SM_OakTree_MedA */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_SM_OakTree_MedA"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_OakTree_MedA.bin",
			"../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_SM_BearBerry_A */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_SM_BearBerry_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_BearBerry_A.bin",
			"../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_SM_HM_OwlPost_Window_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_SM_HM_OwlPost_Window_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_OwlPost_Window_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_WA_Rectangle_Double_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_WA_Rectangle_Double_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_WindowsStyleA_L_Rectangle_Double_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_WC_Retangle_Double_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_WC_Retangle_Double_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_WindowsStyleC_Retangle_Double_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_WA_Square_Double_C*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_WA_Square_Double_C"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_WindowsStyleA_L_Square_Double_C.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_Quid_Window_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_Quid_Window_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_Quid_Window_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_Ollivanders_Box_Window*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_Ollivanders_Box_Window"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_Ollivanders_Box_Window.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_WC_L_DoubleS_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_WC_L_DoubleS_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_WindowsStyleC_L_Double_Single_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_WC_Round_Double_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_WC_Round_Double_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_WindowsStyleC_Round_Double_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_SM_HM_Door1a*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_SM_HM_Door1a"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_Door1a.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_SM_HM_Door2b*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_SM_HM_Door2b"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_Door2b.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;
#pragma endregion


	/* For.Prototype_Component_VIBuffer_Box */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Box"),
		CVIBuffer_Box::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_OcclusionQuery */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_OcclusionQuery"),
		COcclusionQuery::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype<CTerrain>(g_iStaticLevel, CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CBuildingContainer */
	if (FAILED(m_pGameInstance->Add_Prototype<CBuildingContainer>(g_iStaticLevel, CBuildingContainer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CStreetContainer */
	if (FAILED(m_pGameInstance->Add_Prototype<CStreetContainer>(g_iStaticLevel, CStreetContainer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CMapObject_Render */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapObject_Render>(g_iStaticLevel, CMapObject_Render::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CMapObject_Collision */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapObject_Collision>(g_iStaticLevel, CMapObject_Collision::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CInstancedProp */
	if (FAILED(m_pGameInstance->Add_Prototype<CInstancedProp>(g_iStaticLevel, CInstancedProp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Light */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Light>(g_iStaticLevel, CMapElement_Light::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Interactable */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Interactable>(g_iStaticLevel, CMapElement_Interactable::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion // MAP PROTOTYPE

#pragma region ACTOR PROTOTYPE

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype<CPlayer>(g_iStaticLevel, CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Goblin */
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin>(g_iStaticLevel, CGoblin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Troll */
	if (FAILED(m_pGameInstance->Add_Prototype<CTroll>(g_iStaticLevel, CTroll::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion
#pragma region STAT PROTOTYPE
	if (FAILED(Stat_FileLoad("../Bin/Resources/Data/Stat/Stat.xml"))) {
		return E_FAIL;
	}
#pragma endregion


	/* For.Prototype_Component_FSM */
	if (FAILED(m_pGameInstance->Add_Prototype<CFSM>(g_iStaticLevel, CFSM::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Gaze */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamera_Gaze>(g_iStaticLevel, CCamera_Gaze::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CamPosition_Player */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamPosition_Socket>(g_iStaticLevel, CCamPosition_Socket::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CamPosition_Shoulder */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamPosition_Shoulder>(g_iStaticLevel, CCamPosition_Shoulder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CamPosition_Target */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamPosition_Target>(g_iStaticLevel, CCamPosition_Target::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CamPosition_Arm */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamPosition_Arm>(g_iStaticLevel, CCamPosition_Arm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Wand */
	if (FAILED(m_pGameInstance->Add_Prototype<CWand>(g_iStaticLevel, CWand::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Troll_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype<CTroll_Weapon>(g_iStaticLevel, CTroll_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Troll_Rock */
	if (FAILED(m_pGameInstance->Add_Prototype<CTroll_Rock>(g_iStaticLevel, CTroll_Rock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Goblin_Dagger */
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_Dagger>(g_iStaticLevel, CGoblin_Dagger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Broom */
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom>(g_iStaticLevel, CBroom::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXWall>(g_iStaticLevel, CDummy_PhysXWall::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXPlayable>(g_iStaticLevel, CDummy_PhysXPlayable::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CPhysXEffectHitBox>(g_iStaticLevel, CPhysXEffectHitBox::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}


#pragma region RECEIVE_THREAD
	{ // MapModels
		_uint iIndex = 0;
		for (auto& JobMapModels : jobMapModels)
		{
			vector<FOLDER_LOAD*>* pOut = JobMapModels.get();
			for (_uint i = 0; i < pOut->size(); ++i) {
				FOLDER_LOAD* pContents = (*pOut)[i];
				if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, pContents->pModelTag, pContents->pLoadedModel))) {
					return E_FAIL;
				}

				for (_uint j = 0; j < pContents->pRigidBodyTags.size(); ++j) {
					if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, pContents->pRigidBodyTags[j], pContents->LoadedRigidBody[j]))) {
						return E_FAIL;
					}
				}
				Safe_Delete(pContents);
			}
			iIndex++;
			Safe_Delete(pOut);
		}
	}
#pragma endregion
	m_strMessage = TEXT("정보를 불러오는 중입니다.");

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Field()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("이펙트를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("정보를 불러오는 중입니다.");

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Restart()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("이펙트를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("정보를 불러오는 중입니다.");

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Asset_FileLoad(const _char* pDirectoryPath, const _tchar* pPreName, function<HRESULT(_wstring, const _char*)> AddPrototypeEvent)
{
	for (const auto& file : filesystem::directory_iterator(pDirectoryPath))
	{
		if (file.is_directory())
			continue;

		string ext = file.path().extension().string();

		if (!strcmp(ext.c_str(), ".txt") || !strcmp(ext.c_str(), ".fbx")){
			continue;
		}

		_char szFilePath[MAX_PATH] = {};

		strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());

		wstring wstrFileName = pPreName + file.path().stem().wstring();

		AddPrototypeEvent(wstrFileName, szFilePath);

	}

	return S_OK;
}

HRESULT CLoader::Stat_FileLoad(const _char* pDirectoryPath)
{
	filesystem::path pathStatFile = pDirectoryPath;

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError Error = doc.LoadFile(pathStatFile.string().c_str());
	if (Error != tinyxml2::XML_SUCCESS) {
		return E_FAIL;
	}

	tinyxml2::XMLElement* pStatInfo = doc.FirstChildElement("StatInfo");
	if (!pStatInfo) {
		return E_FAIL;
	}

	_uint iNumChild = pStatInfo->ChildElementCount();
	tinyxml2::XMLNode* pChild = pStatInfo->FirstChildElement();
	for (_uint i = 0; i < iNumChild; ++i) {
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, CMyTools::ToWstring(pChild->Value()),
			CStat::Create(m_pDevice, m_pContext, pChild)))) {
			return E_FAIL;
		}
		pChild = pChild->NextSiblingElement();
	}

	return S_OK;
}

vector<FOLDER_LOAD*>* APIENTRY Deferred_FolderLoad_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pDirectoryPath, const _char* pFileExt, _bool bUseTag)
{
	if (FAILED(CoInitializeEx(nullptr, 0))) {
		return nullptr;
	}

	if (!filesystem::exists(pDirectoryPath)) {
		MSG_BOX(" 경로 이상해 ");
		return nullptr;
	}

	vector<FOLDER_LOAD*>* pOut = new vector<FOLDER_LOAD*>;
	for (const auto& file : filesystem::directory_iterator(pDirectoryPath))
	{
		if (file.is_directory()) { continue; }
		if (strcmp(file.path().extension().string().c_str(), pFileExt)) { continue; }

		FOLDER_LOAD* pContents = new FOLDER_LOAD;
	
		{ // FOLDER_LOAD
			_char szFilePath[MAX_PATH] = {};
			strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());

			_wstring wstrFileName = L"Prototype_GameObject_" + file.path().stem().wstring();

			CModel* pModel = CModel::Create(pDevice, pContext, MODEL::ENVIROMENT, szFilePath);
			
			_uint iNumMesh = pModel->Get_NumMeshes();

			{
				pContents->pModelTag = wstrFileName;
				pContents->pathModel = file.path();

				pContents->pLoadedModel = pModel;

				pContents->pRigidBodyTags.reserve(iNumMesh);
				pContents->LoadedRigidBody.reserve(iNumMesh);
			}

			CRigidBody_Static::RIGIDBODY_STATIC_PROTOTYPEDESC Desc{};
			for (_uint i = 0; i < iNumMesh; ++i) {
				{
					Desc.eType = ACTOR::TRIANGLEMESH;
					Desc.ePxRigidBodyFlags = {};
					Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
					Desc.ePxMaterialTypes = PXMATERIAL::DEFAULT;
					Desc.vMatInfo = _float3(0.5f, 0.5f, 0.6f);
					Desc.fContactOffset = 0.f;
				}

				pContents->pRigidBodyTags.emplace_back(CMyTools::ToWstring(pModel->Get_MeshName(i) + to_string(i)).c_str());
				pContents->LoadedRigidBody.emplace_back(CRigidBody_Static::Create(pDevice, pContext, Desc));
			}
		}
		(*pOut).emplace_back(pContents);
	}
	(*pOut).shrink_to_fit();

	CoUninitialize();
	return pOut;
}

future<vector<FOLDER_LOAD*>*> CLoader::Deferred_FolderLoad(const _char* pDirectoryPath, const _char* pFileExt, _bool bUseTag)
{
	return m_pGameInstance->EnqueueJob(&Deferred_FolderLoad_Main, m_pDevice, m_pContext, pDirectoryPath, pFileExt, bUseTag);
}

pair<_wstring, CModel*>* APIENTRY Deferred_ModelLoad_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pDirectoryPath, const _tchar* pPrototypeTag, _fmatrix PreTransform)
{
	if (FAILED(CoInitializeEx(nullptr, 0))) {
		assert(false);
		return nullptr;
	}

	CModel* pModel = CModel::Create(pDevice, pContext, eType, pDirectoryPath, PreTransform);

	CoUninitialize();
	pair<_wstring, CModel*>* Out = new pair<_wstring, CModel*>(pPrototypeTag, pModel);
	return Out;
}

future<pair<_wstring, CModel*>*> CLoader::Deferred_ModelLoad(MODEL eType, const _char* pDirectoryPath, _fmatrix PreTransform, const _tchar* pPrototypeTag) {
	return m_pGameInstance->EnqueueJob(&Deferred_ModelLoad_Main, m_pDevice, m_pContext, eType, pDirectoryPath, pPrototypeTag, PreTransform);
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	__super::Free();


	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
