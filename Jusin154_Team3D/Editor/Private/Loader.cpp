#include "pch.h"
#include "Loader.h"
#include "GameInstance.h"

#pragma region DEFAULT_HEADER

#include "Camera_Debug.h"
#include "DummyRect.h"
#include "MainApp.h"
#include "Dummy_Cube.h"
#include "MainLight.h"

#pragma endregion

#pragma region OBJECT_HEADER

#include "RootModelPart.h"
#include "Head.h"
#include "Body.h"
#include "Hair.h"
#include "DummySkyBox.h"
#include "DummyObject.h"
#include "Player.h"
#include "Goblin.h"
#include "Broom.h"
#include <filesystem>
#include "Camera_Gaze.h"
#include "CamPosition_Socket.h"
#include "CamPosition_Target.h"
#include "CamPosition_Shoulder.h"
#include "CamPosition_Arm.h"
#include "Wand.h"


#pragma endregion

#pragma region UI

#include "Logo.h"
#include "Logo_Text.h"
#include "Logo_Glow.h"

#include "Spell_Data.h"
#include "UI_Manager.h"

#include "Mouse_Cursor.h"
#include "CameraLockOn.h"

#include "Loding_Canvas.h"
#include "Loding_Panel.h"

#include "GamePlay_Canvas.h"

#include "Enemy_Panel.h"
#include "Enemy_HpBar.h"
#include "Enemy_Info.h"
#include "Boss_HpBer.h"
#include "Enemy_Detection.h"

#include "Mission_Panel.h"
#include "MissionBanner_Border.h"
#include "MissionBanner_Key.h"
#include "Mission_Icon.h"
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

#include "IMGUIUI.h"

#pragma endregion

#pragma region MAP_HEADER
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Box.h"
#include "OcclusionQuery.h"
#include "Terrain.h"
#include "BuildingContainer.h"
#include "MapObject_Collision.h"
#include "MapObject_LOD.h"
#include "MapObject_Manager.h"
#include "MapElement_Interactable.h"
#include "MapElement_Static.h"
#include "MapElement_Light.h"
#include "MapElement_Door.h"
#include "Land.h"
#include "Collider.h"
#include "VIBuffer_Model_Instance.h"
#include "Instance_Model.h"
#include "InstancedProp.h"
#include "Unified.h"
#include "MapElement_Lake.h"
#include "MapElement_Chest.h"
#include "MapElement_Chest_Lid.h"
#pragma endregion


#pragma region EFFECT_HEADER

#include "TestEffect.h"
#include "EditEffect.h"
#include "Effect_Editor.h"
#include "Dummy_Plane.h"
#include "TrailObject.h"
#include "NomalJap.h"
#include "Bombard.h"
#include "Decendo.h"
#include "Protego.h"
#include "EffectPool.h"
#include "Revelio.h"
#include "Levioso.h"

#include "BombardSide.h"
#include "LeviosoSide.h"
#include "DecendoSide.h"
#include "NomalJapSide.h"

#include "TrollSwing.h"
#include "Troll_Nomal_Smoke.h"
#include "Troll_Rush_Hit.h"
#include "Goblin_Protego.h"

#include "Troll.h"
#include "Troll_Rock.h"
#include "Troll_Weapon.h"
#include "Goblin_Dagger.h"
#include "Goblin_BattleAxe.h"
#include "Goblin_Spector.h"
#include "StunEffect.h"
#pragma endregion

#pragma region PHYSX_HEADER

#include "Dummy_PhysXBox.h"
#include "Dummy_PhysXPlayable.h"
#include "Dummy_PhysXMonster.h"
#include "Dummy_PhysXMesh.h"
#include "Dummy_PhysXPlatform.h"
#include "Dummy_PhysXFreeDoor.h"
#include "Dummy_PhysXWall.h"
#include "Dummy_PhysXEffectHitBox.h"
//#include "Dummy_PhysXDoorFrame.h"
//#include "Dummy_PhysXFixedDoor.h"
//#include "Dummy_PhysXDoorSet.h"

#pragma endregion 

#pragma region BLOOM_HEADER

#include "Dummy_Globe.h"

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

_uint APIENTRY LoadingMain(void* pArg)
{
	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading())) {
		assert(false);
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
	case LEVEL::UI:
		hr = Loading_For_UI();
		break;
	case LEVEL::MAP:
		hr = Loading_For_MapViewer();
		break;
	case LEVEL::OBJECT:
		hr = Loading_For_ObjectViewer();
		break;
		//case LEVEL::COMBINED:
		//	hr = Loading_For_CombinedViewer();
		//	break;
	case LEVEL::EFFECT:
		hr = Loading_For_Effect();
		break;
	case LEVEL::PHYSX:
		hr = Loading_For_PhysXLevel();
		break;
	case LEVEL::BLOOM:
		hr = Loading_For_Bloom();
		break;
	default:
		assert(false);
		break;
	}


	CoUninitialize();

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

void CLoader::Output()
{
	SetWindowText(g_hWnd, m_strMessage.c_str());
}

HRESULT CLoader::Loading_For_Logo()
{

	m_strMessage = TEXT("Texture Loading..");


	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Dororong"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/DororongDoro.png"), 0)))) {
		return E_FAIL;
	}

	Asset_FileLoad("../Bin/Resources/Textures/Logo", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::LOGO), wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	m_strMessage = TEXT("Model Loading..");


	m_strMessage = TEXT("Shader Loading..");

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

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_CELL,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_Cell.hlsl"),
			VTXPOS::Elements, VTXPOS::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_PARTICLE,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxPointParticle.hlsl"),
			VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements)))) {
		return E_FAIL;
	}

	m_strMessage = TEXT("Prototype Loading..");

	if (FAILED(m_pGameInstance->Add_Prototype<CDummyRect>(g_iStaticLevel, CDummyRect::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CCamera_Debug>(g_iStaticLevel, CCamera_Debug::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CMainLight>(g_iStaticLevel, CMainLight::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLogo>(g_iStaticLevel, CLogo::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLogo_Text>(g_iStaticLevel, CLogo_Text::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLogo_Glow>(g_iStaticLevel, CLogo_Glow::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	m_strMessage = TEXT("Loading Success!");

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_For_UI()
{


	m_strMessage = TEXT("Texture Loading..");

	Asset_FileLoad("../Bin/Resources/Textures/Keyboard", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Action", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Mission", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/SpellWidget", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Mission_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Mission/Mission_Icon_%d.png"), 2)))) {
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("ActiveMission_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Mission/ActiveMission_Icon_%d.png"), 2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Megic_Metar"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Action/Megic_Meter%d.png"), 3)))) {
		return E_FAIL;
	}


	Asset_FileLoad("../Bin/Resources/Textures/MiniMap", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Loading", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Spells", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Spells/SpellMeters", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Potions", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/GadgetWheel", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Cursor", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Enemy", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Item"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/GadgetWheel/Item%d.png"), 8)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Accio"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Accio\\Accio%d.png"), 151))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Altering_Spell"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Altering_Spell\\Altering_Spell%d.png"), 349))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Alohomora"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Alohomora\\Alohomora%d.png"), 394))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Ancient_Magic"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Ancient_Magic\\Ancient_Magic%d.png"), 152))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Ancient_Magic_Throw"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Ancient_Magic_Throw\\Ancient_Magic_Throw%d.png"), 89))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Arresto_Momentum"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Arresto_Momentum\\Arresto_Momentum%d.png"), 120))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Avada_Kedavra"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Avada_Kedavra\\Avada_Kedavra%d.png"), 208))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Basic_Cast"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Basic_Cast\\Basic_Cast%d.png"), 308))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Beast_Feed"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Beast_Feed\\Beast_Feed%d.png"), 378))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Beast_Petting_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Beast_Petting_Brush\\Beast_Petting_Brush%d.png"), 327))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Bombarda"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Bombarda\\Bombarda%d.png"), 262))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Confringo"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Confringo\\Confringo%d.png"), 157))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Conjuring_Spell"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Conjuring_Spell\\Conjuring_Spell%d.png"), 115))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Descendo"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Descendo\\Descendo%d.png"), 155))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Depulso"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Depulso\\Depulso%d.png"), 152))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Diffindo"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Diffindo\\Diffindo%d.png"), 150))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Disillusionment"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Disillusionment\\Disillusionment%d.png"), 260))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Evanesco"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Evanesco\\Evanesco%d.png"), 136))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Expelliarmus"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Expelliarmus\\Expelliarmus%d.png"), 146))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Flipendo"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Flipendo\\Flipendo%d.png"), 188))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Glacius"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Glacius\\Glacius%d.png"), 156))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Imperio"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Imperio\\Imperio%d.png"), 462))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Incendio"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Incendio\\Incendio%d.png"), 213))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Levioso"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Levioso\\Levioso%d.png"), 180))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Lumos"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Lumos\\Lumos%d.png"), 365))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Nab_Sack"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Nab_Sack\\Nab_Sack%d.png"), 235))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Petrificus_Totalus"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Petrificus_Totalus\\Petrificus_Totalus%d.png"), 275))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Protego"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Protego\\Protego%d.png"), 285))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Reparo"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Reparo\\Reparo%d.png"), 385))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Revelio"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Revelio\\Revelio%d.png"), 227))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Stupefy"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Stupefy\\Stupefy%d.png"), 222))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Transformation"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Transformation\\Transformation%d.png"), 146))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Wingardium_Leviosa"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Wingardium_Leviosa\\Wingardium_Leviosa%d.png"), 273))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Wingardium_Leviosa"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Wingardium_Leviosa\\Wingardium_Leviosa%d.png"), 273))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Crucio"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Crucio\\Crucio%d.png"), 216))))
	{
		return E_FAIL;
	}

	m_strMessage = TEXT("Model Loading..");

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

	m_strMessage = TEXT("Shader Loading..");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_UIINSTANCE,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_UI_Instance.hlsl"),
			VTX_POSTEX_INSTANCE_UI::Elements, VTX_POSTEX_INSTANCE_UI::iNumElements)))) {
		return E_FAIL;
	}

	m_strMessage = TEXT("Prototype Loading..");


	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Data>(g_iStaticLevel, CSpell_Data::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CUI_Manager>(g_iStaticLevel, CUI_Manager::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CMouse_Cursor>(g_iStaticLevel, CMouse_Cursor::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CCameraLockOn>(g_iStaticLevel, CCameraLockOn::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLoding_Canvas>(g_iStaticLevel, CLoding_Canvas::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CGamePlay_Canvas>(g_iStaticLevel, CGamePlay_Canvas::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CEnemy_Panel>(g_iStaticLevel, CEnemy_Panel::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CEnemy_HpBar>(g_iStaticLevel, CEnemy_HpBar::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CEnemy_Info>(g_iStaticLevel, CEnemy_Info::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CBoss_HpBar>(g_iStaticLevel, CBoss_HpBar::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CEnemy_Detection>(g_iStaticLevel, CEnemy_Detection::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLoading_Panel>(g_iStaticLevel, CLoading_Panel::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CLoadingWidget>(g_iStaticLevel, CLoadingWidget::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CLoadingWidget_Flame>(g_iStaticLevel, CLoadingWidget_Flame::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CMission_Panel>(g_iStaticLevel, CMission_Panel::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMissionBanner_Border>(g_iStaticLevel, CMissionBanner_Border::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMissionBanner_Key>(g_iStaticLevel, CMissionBanner_Key::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMission_Icon>(g_iStaticLevel, CMission_Icon::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CMiniMap_Panel>(g_iStaticLevel, CMiniMap_Panel::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMiniMap_TrimBorder>(g_iStaticLevel, CMiniMap_TrimBorder::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CNoMountIcon>(g_iStaticLevel, CNoMountIcon::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CAction_Panel>(g_iStaticLevel, CAction_Panel::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Slot>(g_iStaticLevel, CSpell_Slot::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Overlay>(g_iStaticLevel, CSpell_Overlay::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSlot_Number>(g_iStaticLevel, CSlot_Number::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CHpBarBG>(g_iStaticLevel, CHpBarBG::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CPotion>(g_iStaticLevel, CPotion::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMagic_Meter>(g_iStaticLevel, CMagic_Meter::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMagic_Icon>(g_iStaticLevel, CMagic_Icon::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_UI>(g_iStaticLevel, CSpell_UI::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMagic_Item>(g_iStaticLevel, CMagic_Item::Create(m_pDevice, m_pContext))))
	{
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

	if (FAILED(m_pGameInstance->Add_Prototype<CIMGUIUI>(g_iStaticLevel, CIMGUIUI::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}


	m_strMessage = TEXT("Loading Success!");


	m_isFinished = true;

	return S_OK;

}
HRESULT CLoader::Loading_For_Effect()
{

	m_strMessage = TEXT("PhysX Meshes Loading..");
	{

		// Dumping Box
		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
		{
			Desc.eType = ACTOR::BOX;
			Desc.ePxRigidBodyFlags = { /*PSX::PxRigidBodyFlag::eKINEMATIC*/ };
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
			Desc.vMatInfo = { 0.5f, 0.5f, 0.6f };
			Desc.fContactOffset = { 0.05f };
			Desc.vhalfGeometryInfo = { 1.f, 1.f, 1.f };
			Desc.fDensity = 10.f;
			Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
			Desc.eLockFlag = {};
			Desc.vAutoDamping = { };
			Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
			Desc.vLocalTranslation = { 0.f, 0.f, 0.f };
		}
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
			return E_FAIL;
		}
	} { // Heavy Wall
		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
		{
			Desc.eType = ACTOR::BOX;
			Desc.ePxRigidBodyFlags = { /*PSX::PxRigidBodyFlag::eKINEMATIC*/ };
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
			Desc.vMatInfo = { 0.5f, 0.5f, 0.6f };
			Desc.fContactOffset = { 0.05f };
			Desc.vhalfGeometryInfo = { 2.5f, 4.5f, 3.5f };
			Desc.fDensity = 10.f;
			Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
			Desc.eLockFlag = {};
			Desc.vAutoDamping = { 100.f, 100.f };
			Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
			Desc.vLocalTranslation = { 0.f, 0.f, 0.f };
		}
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_HEAVY_WALL"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
			return E_FAIL;
		}
	}

	{
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), CCharacter_Controller::Create(m_pDevice, m_pContext)))) {
			return E_FAIL;
		}
	}
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Box"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Box/Box.fbx", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixIdentity())))) {
		return E_FAIL;
	}

	m_strMessage = TEXT("Texture Loading..");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("TerrainTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures//T_LandscapeStreamingProxy_0_LOD1_Summer_D.png"), 0)))) {
		return E_FAIL;
	}

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

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Noises", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Diffuse", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Trails", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;


	});

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Flipbooks", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Gradients", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

	m_strMessage = TEXT("Model Loading..");


	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_SkyboxModel"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/SkyBox/SkyBox.fbx", XMMatrixIdentity()))))
		return E_FAIL;


	Asset_FileLoad("../Bin/Resources/Models/Effect/Box", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Models/Effect/Rock", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});


	Asset_FileLoad("../Bin/Resources/Models/Effect/ParticleMesh", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Models/Effect/Goo", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Models/Effect/NomalMesh", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Desc_Box"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Box/Box.bin", XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Model_DummyPlane"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Effect/DummyPlane/DummyPlane.fbx", XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Wand_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Object/Wand/Wand.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Goblin_Dagger_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Object/Goblin_Dagger/SK_WPN_GOB_SmallSword.bin", XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Goblin_BattleAxe_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Object/Goblin_BattleAxe/SK_WPN_GOB_BattleAxe01.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Goblin_Spector_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/GoblinSpector/GoblinSpector.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity()))))
		return E_FAIL;

	vector<future<pair<_wstring, CModel*>*>> futures = {};

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Monster/Goblin/Goblin.bin", XMMatrixRotationY(XMConvertToRadians(180.f))* XMMatrixIdentity(),
		TEXT("Prototype_Component_Goblin_Model")

	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/SkyBox/SkyBox.fbx", XMMatrixIdentity(),
		TEXT("Prototype_Component_SkyboxModel")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Npc/Npc.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Npc_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Wand/Wand.fbx", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Wand_Model")
	));


	for (auto& job : futures) {

		pair<_wstring, CModel*>* pResult = job.get();
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, pResult->first, pResult->second))) {
			assert(false);
			return E_FAIL;
		}
		Safe_Delete(pResult);
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Troll_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::PBR_ANIM, "../Bin/Resources/Models/Monster/SubTroll/troll.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity())))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Troll_Weapon_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::PBR_ANIM, "../Bin/Resources/Models/Object/SubTroll_Weapon/SK_WPN_Troll_Club07.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Troll_Rock_Big_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Object/Troll_Rock/Troll_Rock_Big.bin", XMMatrixScaling(0.00004f, 0.00004f, 0.00004f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity()))))
		return E_FAIL;

	{
		CVIBuffer_Terrain* pTerrain = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, "Hogsmeade_HeightMap.bin", 512, 512);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Terrain_Hogsmeade"), pTerrain))) {
			return E_FAIL;
		}
		pTerrain->ConvertToHeightField(TEXT("Hogsmeade_HeightMap"));

		CRigidBody_Static::RIGIDBODY_STATIC_PROTOTYPEDESC Desc{}; // 터레인 리지드 지형 추가
		{
			Desc.eType = ACTOR::HEIGHTFIELD;
			Desc.ePxRigidBodyFlags = {};
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = PXMATERIAL::DEFAULT;
			Desc.vMatInfo = _float3(0.5f, 0.5f, 0.6f);
			Desc.fContactOffset = 0.f;
		}
		CRigidBody_Static* pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_RigidBody_Static_Terrain_Hogsmeade"), pRigid))) {
			return E_FAIL;
		}
	}



	m_strMessage = TEXT("Shader Loading..");


	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_INSTANCE_MODEL,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxModelInstance.hlsl"),
			VTX_MODEL_INSTANCE_PARTICLE::Elements, VTX_MODEL_INSTANCE_PARTICLE::iNumElements)))) {
		return E_FAIL;
	}


	m_strMessage = TEXT("Prototype Loading..");



	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXBox>(g_iStaticLevel, CDummy_PhysXBox::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXEffectHitBox>(g_iStaticLevel, CDummy_PhysXEffectHitBox::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXPlayable>(g_iStaticLevel, CDummy_PhysXPlayable::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXMonster>(g_iStaticLevel, CDummy_PhysXMonster::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXWall>(g_iStaticLevel, CDummy_PhysXWall::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CEditEffect>(ENUM_CLASS(LEVEL::EFFECT), CEditEffect::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CTestEffect>(ENUM_CLASS(LEVEL::EFFECT), CTestEffect::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CEffect_Editor>(ENUM_CLASS(LEVEL::EFFECT), CEffect_Editor::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_Plane>(ENUM_CLASS(LEVEL::EFFECT), CDummy_Plane::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CTrail>(ENUM_CLASS(LEVEL::EFFECT), CTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CTrailObject>(ENUM_CLASS(LEVEL::EFFECT), CTrailObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CNomalJap>(ENUM_CLASS(LEVEL::EFFECT), CNomalJap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin>(g_iStaticLevel, CGoblin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CTroll>(g_iStaticLevel, CTroll::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype<CTroll_Rock>(g_iStaticLevel, CTroll_Rock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CTroll_Weapon>(g_iStaticLevel, CTroll_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CStunEffect>(g_iStaticLevel, CStunEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	
	/* For.Prototype_GameObject_DummySkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype<CDummySkyBox>(g_iStaticLevel, CDummySkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_Cube>(ENUM_CLASS(LEVEL::EFFECT), CDummy_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CBombard>(ENUM_CLASS(LEVEL::EFFECT), CBombard::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CDecendo>(ENUM_CLASS(LEVEL::EFFECT), CDecendo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CProtego>(ENUM_CLASS(LEVEL::EFFECT), CProtego::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CRevelio>(ENUM_CLASS(LEVEL::EFFECT), CRevelio::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CLevioso>(ENUM_CLASS(LEVEL::EFFECT), CLevioso::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CNomalJapSide>(NEXT_LEVEL, CNomalJapSide::Create(m_pDevice, m_pContext)))) {
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

	if (FAILED(m_pGameInstance->Add_Prototype<CTrollSwing>(NEXT_LEVEL, CTrollSwing::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CTroll_Nomal_Smoke>(NEXT_LEVEL, CTroll_Nomal_Smoke::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CTroll_Rush_Hit>(NEXT_LEVEL, CTroll_Rush_Hit::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_Protego>(NEXT_LEVEL, CGoblin_Protego::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_Dagger>(g_iStaticLevel, CGoblin_Dagger::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_BattleAxe>(g_iStaticLevel, CGoblin_BattleAxe::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_Spector>(g_iStaticLevel, CGoblin_Spector::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}


	/* For.Prototype_GameObject_Wand */
	if (FAILED(m_pGameInstance->Add_Prototype<CWand>(g_iStaticLevel, CWand::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_FSM */
	if (FAILED(m_pGameInstance->Add_Prototype<CFSM>(g_iStaticLevel, CFSM::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CamPosition_Arm */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamPosition_Arm>(g_iStaticLevel, CCamPosition_Arm::Create(m_pDevice, m_pContext))))
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

	if (FAILED(m_pGameInstance->Add_Prototype<CEffectPool>(g_iStaticLevel, CEffectPool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype<CPlayer>(g_iStaticLevel, CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype<CTerrain>(g_iStaticLevel, CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	m_strMessage = TEXT("Loading Success!");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_PhysXLevel()
{
	m_strMessage = TEXT("PhysX Bodies Loading..");

	{ // Dumping Box
		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
		{
			Desc.eType = ACTOR::BOX;
			Desc.ePxRigidBodyFlags = { /*PSX::PxRigidBodyFlag::eKINEMATIC*/ };
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
			Desc.vMatInfo = { 0.5f, 0.5f, 0.6f };
			Desc.fContactOffset = { 0.05f };
			Desc.vhalfGeometryInfo = { 1.f, 1.f, 1.f };
			Desc.fDensity = 10.f;
			Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
			Desc.eLockFlag = {};
			Desc.vAutoDamping = { };
			Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
			Desc.vLocalTranslation = { 0.f, 0.f, 0.f };
		}
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
			return E_FAIL;
		}
	}
	{ // Heavy Wall
		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
		{
			Desc.eType = ACTOR::BOX;
			Desc.ePxRigidBodyFlags = { /*PSX::PxRigidBodyFlag::eKINEMATIC*/ };
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
			Desc.vMatInfo = { 0.5f, 0.5f, 0.6f };
			Desc.fContactOffset = { 0.05f };
			Desc.vhalfGeometryInfo = { 2.5f, 4.5f, 3.5f };
			Desc.fDensity = 10.f;
			Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
			Desc.eLockFlag = {};
			Desc.vAutoDamping = { 100.f, 100.f };
			Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
			Desc.vLocalTranslation = { 0.f, 0.f, 0.f };
		}
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_HEAVY_WALL"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
			return E_FAIL;
		}
	}
	{ // Platform
		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
		{
			Desc.eType = ACTOR::BOX;
			Desc.ePxRigidBodyFlags = { /*PSX::PxRigidBodyFlag::eKINEMATIC*/ };
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
			Desc.vMatInfo = { 0.5f, 0.5f, 0.6f };
			Desc.fContactOffset = { 0.05f };
			Desc.vhalfGeometryInfo = { 1.5f, 0.5f, 1.5f };
			Desc.fDensity = 1000.f;
			Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
			Desc.eLockFlag = { PSX::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PSX::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z | PSX::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y };
			Desc.vAutoDamping = { 10.f, 10.f };
			Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
			Desc.vLocalTranslation = { 0.f, 0.f, 0.f };
		}
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_PLATFORM"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
			return E_FAIL;
		}
	}
	{ // Door
		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
		{
			Desc.eType = ACTOR::BOX;
			Desc.ePxRigidBodyFlags = { /*PSX::PxRigidBodyFlag::eKINEMATIC*/ };
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
			Desc.vMatInfo = { 0.5f, 0.5f, 0.6f };
			Desc.fContactOffset = { 0.05f };
			Desc.vhalfGeometryInfo = { 3.5f, 1.5f, 0.25f };
			Desc.fDensity = 10.f;
			PSX::PxTransform pxPivotTransform = PSX::PxTransform(PSX::PxVec3(3.5f, 1.5f, 0.f));
			Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
			Desc.vLocalTranslation = { 0.f, 0.f, 0.f };

			Desc.pxMassCenter = pxPivotTransform;
			Desc.eLockFlag = {
				PSX::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
				//PSX::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
				PSX::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z |
				PSX::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y |
				PSX::PxRigidDynamicLockFlag::eLOCK_LINEAR_X |
				PSX::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z
			};
			Desc.vAutoDamping = { 10.f, 10.f };
		}
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_DOOR"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
			return E_FAIL;
		}
	}
	{ // FixedDoor
		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
		{
			Desc.eType = ACTOR::BOX;
			Desc.ePxRigidBodyFlags = { /*PSX::PxRigidBodyFlag::eKINEMATIC*/ };
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
			Desc.vMatInfo = { 0.5f, 0.5f, 0.6f };
			Desc.fContactOffset = { 0.05f };
			Desc.vhalfGeometryInfo = { 1.5f, 1.5f, 0.25f };
			Desc.fDensity = 10.f;
			PSX::PxTransform pxPivotTransform = PSX::PxTransform(PSX::PxVec3(1.5f, 1.5f, 0.f));

			Desc.pxMassCenter = pxPivotTransform;
			Desc.eLockFlag = { };
			Desc.vAutoDamping = { 10.f, 10.f };
			Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
			Desc.vLocalTranslation = { 0.f, 0.f, 0.f };
		}
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_DOOR"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
			return E_FAIL;
		}
	}

	{
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), CCharacter_Controller::Create(m_pDevice, m_pContext)))) {
			return E_FAIL;
		}
	}

	m_strMessage = TEXT("Texture Loading..");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("TerrainTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Cursor/UI_T_CursorRings.dds"), 0)))) {
		return E_FAIL;
	}
	m_strMessage = TEXT("Model Loading..");


	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Box"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Box/Box.fbx", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixIdentity())))) {
		return E_FAIL;
	}

#ifdef 기무리
	vector<_wstring> ModelPrototypeTags = {};
	vector<filesystem::path> ModelPrototypePath = {};

	//for (const auto& file : filesystem::directory_iterator("..\\Bin\\Resources\\Models\\MapMesh"))
	for (const auto& file : filesystem::directory_iterator("C:\\Users\\kimnuri\\Desktop\\MeshTable\\Game\\Environment\\Hogsmeade\\Common\\Meshes\\Terrain"))
	{
		if (file.is_directory()) {
			continue;
		}

		string ext = file.path().extension().string();

		//if (strcmp(ext.c_str(), ".bin"))
		//	continue;
		if (strcmp(ext.c_str(), ".fbx"))
			continue;

		_char szFilePath[MAX_PATH] = {};

		strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());

		_wstring strFileName = L"Prototype_GameObject_" + file.path().stem().wstring();

		CModel* pModel = CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, szFilePath);

		/*For Prototype_Component_Model_*/
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, strFileName, pModel))) {
			return E_FAIL;
		}

		ModelPrototypeTags.push_back(strFileName);

		{ /* cache rigidbody */ // More Info In 리지드바디, CCT, 엔진이넘
			ModelPrototypePath.push_back(file.path());

			_uint iNumMesh = pModel->Get_NumMeshes();

			CRigidBody_Static::RIGIDBODY_STATIC_PROTOTYPEDESC Desc{};
			for (_uint i = 0; i < iNumMesh; ++i) {
				_string strDestName = pModel->Get_MeshName(i) + to_string(i);
				{ // basic static RIGIDBODY
					Desc.eType = ACTOR::TRIANGLEMESH;
					Desc.ePxRigidBodyFlags = {};
					Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
					Desc.ePxMaterialTypes = PXMATERIAL::DEFAULT;
					Desc.vMatInfo = _float3(0.5f, 0.5f, 0.6f);
					Desc.fContactOffset = 0.001f;
				}
				if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, CMyTools::ToWstring(strDestName).c_str(), CRigidBody_Static::Create(m_pDevice, m_pContext, Desc)))) {
					return E_FAIL;
				}
			}
		}

	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMapObject_Manager>(g_iStaticLevel, CMapObject_Manager::Create(m_pDevice, m_pContext, ModelPrototypeTags, ModelPrototypePath)))) {
		return E_FAIL;
	}

#endif // 기무리

	m_strMessage = TEXT("Shader Loading..");

	m_strMessage = TEXT("Prototype Loading..");

	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXBox>(g_iStaticLevel, CDummy_PhysXBox::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXFreeDoor>(g_iStaticLevel, CDummy_PhysXFreeDoor::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	//if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXDoorSet>(g_iStaticLevel, CDummy_PhysXDoorSet::Create(m_pDevice, m_pContext)))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXDoorFrame>(g_iStaticLevel, CDummy_PhysXDoorFrame::Create(m_pDevice, m_pContext)))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXFixedDoor>(g_iStaticLevel, CDummy_PhysXFixedDoor::Create(m_pDevice, m_pContext)))) {
	//	return E_FAIL;
	//}
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXPlayable>(g_iStaticLevel, CDummy_PhysXPlayable::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXMonster>(g_iStaticLevel, CDummy_PhysXMonster::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXMesh>(g_iStaticLevel, CDummy_PhysXMesh::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXWall>(g_iStaticLevel, CDummy_PhysXWall::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXPlatform>(g_iStaticLevel, CDummy_PhysXPlatform::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMapObject_Collision>(g_iStaticLevel, CMapObject_Collision::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMapObject_LOD>(g_iStaticLevel, CMapObject_LOD::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	m_strMessage = TEXT("Loading Success!");

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_For_Bloom()
{

	vector<future<pair<_wstring, CModel*>*>> futures = {};

#pragma region HAIR

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Monster/SubTroll/troll.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_SubTroll_Model")
	));


#pragma endregion
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Desc_Box"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Box/Box.bin", XMMatrixScaling(10.f, 10.f, 10.f) * XMMatrixIdentity())))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Model_DummyPlane"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Effect/DummyPlane/DummyPlane.fbx", XMMatrixTranslation(0.f, -10.f, 0.f) * XMMatrixIdentity())))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Desc_Globe"), // RG DDS NormalMap
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Object/DragonGlobe/DragonGlobe.fbx", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixIdentity())))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Desc_Globe2"), // RGB DDS NormalMap
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Object/DragonGlobe2/DragonGlobe.fbx", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixIdentity())))) {
		return E_FAIL;
	}


	vector<_wstring> ModelPrototypeTags = {};
	vector<filesystem::path> ModelPrototypePath = {};

	/* Hog_Props */
	if (FAILED(MapFolderLoad("C:\\Users\\kimnuri\\Desktop\\MeshTable\\Game\\Environment\\Hogwarts\\Meshes\\Props",
		".fbx", true, ModelPrototypeTags, ModelPrototypePath))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CHair>(g_iStaticLevel, CHair::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_Globe>(g_iStaticLevel, CDummy_Globe::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_Cube>(g_iStaticLevel, CDummy_Cube::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_Plane>(g_iStaticLevel, CDummy_Plane::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	for (auto& job : futures) {
		pair<_wstring, CModel*>* pResult = job.get();
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, pResult->first, pResult->second))) {
			assert(false);
			return E_FAIL;
		}
		Safe_Delete(pResult);
	}

	m_strMessage = TEXT("Loading Success!");

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

		if (!strcmp(ext.c_str(), ".txt"))
			continue;

		_char szFilePath[MAX_PATH] = {};

		strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());

		wstring wstrFileName = pPreName + file.path().stem().wstring();

		AddPrototypeEvent(wstrFileName, szFilePath);

	}

	return S_OK;
}

HRESULT CLoader::MapFolderLoad(const _char* pDirectoryPath, const _char* pFileExt, _bool bUseTag, vector<_wstring>& ModelPrototypeTags, vector<filesystem::path>& ModelPrototypePath)
{
	for (const auto& file : filesystem::directory_iterator(pDirectoryPath))
	{
		if (file.is_directory())
			continue;

		string ext = file.path().extension().string();

		if (strcmp(ext.c_str(), pFileExt)) {
			continue;
		}

		_char szFilePath[MAX_PATH] = {};

		strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());

		_wstring strFileName = L"Prototype_GameObject_" + file.path().stem().wstring();

		CModel* pModel = CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, szFilePath);

		/*For Prototype_Component_Model_*/
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, strFileName, pModel)))
			return E_FAIL;

		if (bUseTag)
		{
			ModelPrototypeTags.push_back(strFileName);
			ModelPrototypePath.emplace_back(file.path());
		}

		_uint iNumMesh = pModel->Get_NumMeshes();


		CRigidBody_Static::RIGIDBODY_STATIC_PROTOTYPEDESC Desc{};
		for (_uint i = 0; i < iNumMesh; ++i) {
			_string strDestName = pModel->Get_MeshName(i) + to_string(i);
			{
				Desc.eType = ACTOR::TRIANGLEMESH;
				Desc.ePxRigidBodyFlags = {};
				Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
				Desc.ePxMaterialTypes = PXMATERIAL::DEFAULT;
				Desc.vMatInfo = _float3(0.5f, 0.5f, 0.6f);
				Desc.fContactOffset = 0.001f;
			}
			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, CMyTools::ToWstring(strDestName).c_str(), CRigidBody_Static::Create(m_pDevice, m_pContext, Desc)))) {
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

void APIENTRY Deferred_FolderLoad_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pDirectoryPath, const _char* pFileExt, _bool bUseTag, vector<FOLDER_LOAD*>** OutContents)
{
	if (FAILED(CoInitializeEx(nullptr, 0))) {
		return;
	}

	*OutContents = new vector<FOLDER_LOAD*>;
	for (const auto& file : filesystem::directory_iterator(pDirectoryPath))
	{
		if (file.is_directory()) { continue; }
		if (strcmp(file.path().extension().string().c_str(), pFileExt)) { continue; }

		FOLDER_LOAD* pContents = new FOLDER_LOAD;
		pContents->bLoadTags = bUseTag;
		{ // FOLDER_LOAD
			_char szFilePath[MAX_PATH] = {};
			strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());

			_wstring wstrFileName = L"Prototype_GameObject_" + file.path().stem().wstring();

			CModel* pModel = CModel::Create(pDevice, pContext, MODEL::ENVIRONMENT, szFilePath);

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
					Desc.fContactOffset = 0.001f;
				}
				pContents->pRigidBodyTags.emplace_back(CMyTools::ToWstring(pModel->Get_MeshName(i) + to_string(i)).c_str());
				pContents->LoadedRigidBody.emplace_back(CRigidBody_Static::Create(pDevice, pContext, Desc));
			}
		}
		(*OutContents)->emplace_back(pContents);
	}
	(*OutContents)->shrink_to_fit();

	CoUninitialize();
	return;
}

future<void> CLoader::Deferred_FolderLoad(const _char* pDirectoryPath, const _char* pFileExt, _bool bUseTag, vector<FOLDER_LOAD*>** OutContents)
{
	return m_pGameInstance->EnqueueJob(&Deferred_FolderLoad_Main, m_pDevice, m_pContext, pDirectoryPath, pFileExt, bUseTag, OutContents);
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

HRESULT CLoader::Loading_For_ObjectViewer()
{
	m_strMessage = TEXT("Texture Loading..");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("TerrainTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Cursor/UI_T_CursorRings.dds"), 0)))) {
		return E_FAIL;
	}

	m_strMessage = TEXT("Model Loading..");

	// Heavy Wall
	CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
	{
		Desc.eType = ACTOR::BOX;
		Desc.ePxRigidBodyFlags = { /*PSX::PxRigidBodyFlag::eKINEMATIC*/ };
		Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
		Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
		Desc.vMatInfo = { 0.5f, 0.5f, 0.6f };
		Desc.fContactOffset = { 0.05f };
		Desc.vhalfGeometryInfo = { 2.5f, 4.5f, 3.5f };
		Desc.fDensity = 10.f;
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
	vector<future<pair<_wstring, CModel*>*>> futures = {};


	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_SM_BRR_RaceRing_01_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM_LOCAL, "C:/MeshTable\\Game\\Environment\\BroomFlight\\Meshes\\SM_BRR_RaceRing_01.fbx", XMMatrixIdentity())))) {
		return E_FAIL;
	}

#pragma region BODY

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Body/Male/M_DADA/M_DADA.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_M_DADA_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Body/Male/M_DarkWizardSoldier/M_DarkWizardSoldier.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_M_DarkWizardSoldier_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Body/Male/M_DarkWizardGrunt/M_DarkWizardGrunt.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_M_DarkWizardGrunt_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Body/Male/M_DarkWizardTank/M_DarkWizardTank.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_M_DarkWizardTank_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Body/FeMale/F_DarkWizardMage/F_DarkWizardMage.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_F_DarkWizardMage_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Body/FeMale/F_DarkWizardSniper/F_DarkWizardSniper.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_F_DarkWizardSniper_Model")
	));

#pragma endregion

#pragma region HEAD

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Head/Male/Young_M_Head_Ivory.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Young_M_Head_Ivory_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Head/FeMale/T_NPC_F_Head_Eye_Honey.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_T_NPC_F_Head_Eye_Honey_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Head/FeMale/F_Head.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_F_Head_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Head/Male/M_Head.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_M_Head_Model")
	));

#pragma endregion

#pragma region HAIR

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Hair/Male/M_Hair1/M_Hair1.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_M_Hair1_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Hair/Male/M_Hair2/M_Hair2.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_M_Hair2_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Hair/Male/M_Hair3/M_Hair3.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_M_Hair3_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Hair/FeMale/F_Hair1/F_Hair1.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_F_Hair1_Model")
	));

#pragma endregion

#pragma region MONSTER

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Monster/Goblin/Goblin.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Goblin_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Monster/Goblin_Mage/GoblinMage.bin", XMMatrixRotationY(XMConvertToRadians(180.f))* XMMatrixIdentity(),
		TEXT("Prototype_Component_GoblinMage_Model")
	));

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_GoblinSpector_Anim_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/GoblinSpector/GoblinSpector_Anim.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity()))))
		return E_FAIL;

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Monster/TombProtector/TombProtector.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_TombProtector_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Monster/SubTroll/troll.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_troll_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Monster/Dragon/Dragon.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Dragon_Model")
	));

#pragma endregion

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Troll_Weapon/Troll_Weapon.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Troll_Weapon_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/Object/Troll_Rock/Troll_Rock.bin",XMMatrixIdentity(),
		TEXT("Prototype_Component_Troll_Rock_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/Object/Troll_Rock/Troll_Rock_Big.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f)* XMMatrixIdentity(),
		TEXT("Prototype_Component_Troll_Rock_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/Object/Goblin_Dagger/Goblin_Dagger.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f)* XMMatrixIdentity(),
		TEXT("Prototype_Component_Dagger_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Npc/Npc.bin", XMMatrixRotationY(XMConvertToRadians(180.f))* XMMatrixIdentity(),
		TEXT("Prototype_Component_Npc_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Wand/Wand.bin",XMMatrixIdentity(),
		TEXT("Prototype_Component_Wand_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Object/Broom/Broom.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Broom_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/SkyBox/SkyBox.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_SkyboxModel")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/Box/Box.bin", XMMatrixIdentity(),
		TEXT("Desc_Box")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/GerboldOllivander/GerboldOlivander.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_GerboldOlivander_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Npc/GerboldOllivander/GerboldOlivander_Anim.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_GerboldOlivander_Anim_Model")
	));


	for (auto& job : futures) {
		pair<_wstring, CModel*>* pResult = job.get();
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, pResult->first, pResult->second))) {
			assert(false);
			return E_FAIL;
		}
		Safe_Delete(pResult);
	}


	m_strMessage = TEXT("Shader Loading..");

	m_strMessage = TEXT("Prototype Loading..");

	///* For.Prototype_Component_VIBuffer_Terrain */
	//if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, nullptr, 100, 100))))
	//	return E_FAIL;

	/* For.Prototype_GameObject_RootModelPart */
	if (FAILED(m_pGameInstance->Add_Prototype<CRootModelPart>(g_iStaticLevel, CRootModelPart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body */
	if (FAILED(m_pGameInstance->Add_Prototype<CBody>(g_iStaticLevel, CBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Head */
	if (FAILED(m_pGameInstance->Add_Prototype<CHead>(g_iStaticLevel, CHead::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Hair */
	if (FAILED(m_pGameInstance->Add_Prototype<CHair>(g_iStaticLevel, CHair::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_DummyObject */
	if (FAILED(m_pGameInstance->Add_Prototype<CDummyObject>(g_iStaticLevel, CDummyObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_DummySkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype<CDummySkyBox>(g_iStaticLevel, CDummySkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	///* For.Prototype_GameObject_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype<CTerrain>(g_iStaticLevel, CTerrain::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/* For.Prototype_Component_FSM */
	if (FAILED(m_pGameInstance->Add_Prototype<CFSM>(g_iStaticLevel, CFSM::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype<CPlayer>(g_iStaticLevel, CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Goblin */
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin>(g_iStaticLevel, CGoblin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Broom */
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom>(g_iStaticLevel, CBroom::Create(m_pDevice, m_pContext))))
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

	if (FAILED(m_pGameInstance->Add_Prototype<CTrail>(g_iStaticLevel, CTrail::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CTrailObject>(g_iStaticLevel, CTrailObject::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CBombard>(g_iStaticLevel, CBombard::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CEffectPool>(g_iStaticLevel, CEffectPool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strMessage = TEXT("Loading Success!");

	m_isFinished = true;

	return S_OK;
}


HRESULT CLoader::Loading_For_MapViewer()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("TerrainTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures//T_LandscapeStreamingProxy_0_LOD1_Summer_D.png"), 0)))) {
		return E_FAIL;
	}

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

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("CollisionDebug"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Effect/Noises/VFX_T_noise6_D.png"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Levioso_Noise"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Effect/Noises/VFX_T_NoiseCaustics02_Color_D.png"), 0)))) {
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

	/* For.Prototype_Component_Water_Noise_D */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("T_Noises_D"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("../Bin/Resources/Models/Lake/T_Noises_D.dds"), 0)))) {
		return E_FAIL;
	}
	/* For.Prototype_Component_T_Water_Normal_Large_N */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("T_Water_Normal_Large_N"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("../Bin/Resources/Models/Lake/T_Water_Normal_Large_N.dds"), 0)))) {
		return E_FAIL;
	}
	/* For.Prototype_Component_T_Water_Normal_Subtle_N */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("T_Water_Normal_Subtle_N"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("../Bin/Resources/Models/Lake/T_Water_Normal_Subtle_N.dds"), 0)))) {
		return E_FAIL;
	}
	/* For.Prototype_Component_T_Caustics_D */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("T_Caustics_D"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("../Bin/Resources/Models/Lake/T_Caustics_D.dds"), 0)))) {
		return E_FAIL;
	}
	/* For.Prototype_Component_T_Water_N */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("T_Water_N"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("../Bin/Resources/Models/Lake/T_Water_N.dds"), 0)))) {
		return E_FAIL;
	}
	/* Lake_Cube_D */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Lake_Cube_D"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Models/Lake/SkyCube.dds"), 0)))) {
		return E_FAIL;
	}

	/* Lake_Cube_D */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Lake_Refraction"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Models/Lake/T_MudSmallRocks_A_D.dds"), 0)))) {
		return E_FAIL;
	}

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_SkyboxModel */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_SkyboxModel"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/SkyBox/SkyBox.bin", XMMatrixIdentity()))))
		return E_FAIL;

#pragma region MAP_LANDS
	/* For.Prototype_Component_Hogsmead_Land */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Hogsmead_Land"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Land/Hogsmead_Land.fbx", XMMatrixIdentity()))))
		return E_FAIL;

	/* For.Prototype_Component_South_Hogwart_Land_LOD1 */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_South_Hogwart_Land_LOD1"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "C:/MeshTable/Game/Levels/Overland/HOG/HN_BCLOD/SM_LandscapeStreamingProxy_0_LOD1.fbx"))))
		return E_FAIL;

	/* For.Prototype_Component_North_Hogwart_Land_LOD1 */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_North_Hogwart_Land_LOD1"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "C:/MeshTable/Game/Levels/Overland/HOG/HN_AVLOD/SM_LandscapeStreamingProxy_0_LOD1.fbx"))))
		return E_FAIL;

	/* For.Prototype_Component_North_Hogwart2_Land_LOD1 */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_North_Hogwart2_Land_LOD1"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "C:/MeshTable/Game/Levels/Overland/HOG/HN_AULOD/SM_LandscapeStreamingProxy_0_LOD1.fbx"))))
		return E_FAIL;

	/* For.Prototype_Component_West_Hogwart_Land_LOD1 */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_West_Hogwart_Land_LOD1"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "C:/MeshTable/Game/Levels/Overland/HOG/HN_AZLOD/SM_LandscapeStreamingProxy_0_LOD1.fbx"))))
		return E_FAIL;

	/* For.Prototype_Component_East_Hogwart_Land_LOD1 */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_East_Hogsmeade_Land_LOD1"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "C:/MeshTable/Game/Levels/Overland/HOG/HN_AYLOD/SM_LandscapeStreamingProxy_0_LOD1.fbx"))))
		return E_FAIL;

	/* For.Prototype_Component_Hogwart_Lake */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Hogwart_Lake"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Lake/SM_Lake_A_Collision_Deep.fbx", XMMatrixIdentity()))))
		return E_FAIL;

	/* For.Prototype_Component_Hogwart_LakeSurFace */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Hogwart_LakeSurFace"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Lake/SM_Lake_A_Collision_Shallow.fbx", XMMatrixIdentity()))))
		return E_FAIL;

	/* For.Prototype_Component_Hogsmead_NorthLake */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Hogsmead_NorthLake"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/River/NorthLake/HDA_SM_LakeA_Collision_Deep.fbx", XMMatrixIdentity()))))
		return E_FAIL;
	/* For.Prototype_Component_Hogsmead_NorthLakeShallow */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Hogsmead_NorthLakeShallow"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/River/NorthLake/HDA_SM_LakeA_Collision_Shallow.fbx", XMMatrixIdentity()))))
		return E_FAIL;
#pragma endregion

	

	vector<_wstring> ModelPrototypeTags = {};
	vector<filesystem::path> ModelPrototypePath = {};

#pragma region HOGSMEADE

	///* Terrain*/
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\Common\\Collision\\Terrain",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\Common\\Meshes\\Terrain",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* TScrolls*/
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_TScrolls\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_TScrolls\\Collisions",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* 3BroomStick*/
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_ThreeBroomsticks\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_ThreeBroomsticks\\Collision",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_ThreeBroomsticks\\Meshes\\3Broom_Kit",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* Ollivanders*/
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_Ollivanders\\Meshes",
	//	".fbx", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_Ollivanders\\Collision",
	//	".fbx", true, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* Gatehouse*/
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_Gatehouse\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* BLDG_QuidditchShop */
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_QuidditchShop\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_QuidditchShop\\Collision",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* BLDG_HogsheadInn */
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_HogsheadInn\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_HogsheadInn\\Collision",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* BLDG_Honeydukes */
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_Honeydukes\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_Honeydukes\\Collision",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* BLDG_OwlPost */
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_OwlPost\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_OwlPost\\Collision",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* BLDG_TeaShop */
	/*if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_TeaShop\\Meshes",
		".fbx", false, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;
	if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_TeaShop\\Collision",
		".bin", false, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;*/

	///* BLDG_Zonkos */
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_Zonkos\\Meshes",
	//	".fbx", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_Zonkos\\Collision",
	//	".fbx", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* BLDG_DB_GR */
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_DB_GR\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_DB_GR\\Collision",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* BLDG_Potions */
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_Potions\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_Potions\\Collisions",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* BLDG_Salon */
	/*if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_Salon\\Meshes",
		".fbx", false, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;
	if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_Salon\\Collision",
		".fbx", false, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;*/

	///* Hengist_Tree */
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\Common\\Meshes\\Vegetation\\Hengist_Tree",
	//	".fbx", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\Common\\Collision\\Vegetation",
	//	".bin", true, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* GEN A*/
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_A\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_A\\Collision",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* GEN B*/
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_B\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_B\\Collision",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* GEN C*/
	/*if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_C\\Meshes",
		".fbx", false, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;*/
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_C\\Collision",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* GEN E*/
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_E\\Meshes",
	//	".fbx", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_E\\Collision",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* GEN F*/
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_F\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_F\\Collision",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* GEN G*/
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_G\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_G\\Collision",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* GEN H*/
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_H\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_H\\Collision",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	///* GEN J*/
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_J\\Meshes",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;
	//if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\BLDG_GEN_J\\Collision",
	//	".bin", false, ModelPrototypeTags, ModelPrototypePath)))
	//	return E_FAIL;

	/* Fences */
	if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\Common\\Meshes\\fences",
		".fbx", true, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;

	/* Light */
	if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\Common\\Meshes\\LightPosts",
		".bin", false, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;

	/* Doors */
	if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\Common\\Meshes\\Doors",
		".bin", true, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;

	/* Step */
	if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\Common\\Meshes\\GroundSurfaces",
		".bin", false, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;
	if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\Common\\Collision\\GroundSurfaces",
		".bin", false, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;

	/* Barrel */
	if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Objects\\Meshes",
		".bin", false, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;

	/* Box */
	if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\Common\\Meshes\\MiscProps",
		".bin", false, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;

	/* TeaShop Table */
	if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\Common\\Meshes\\Tables",
		".bin", false, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;

	/* TeaShop Chair*/
	if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogsmeade\\Common\\Meshes\\Chairs",
		".bin", false, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;

	/* Hogwart LOD */
	if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Hogwarts\\HogwartsLOD",
		".fbx", false, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;

	/* Object Interactables */
	/* Chest */
	if (FAILED(MapFolderLoad("C:\\MeshTable\\Game\\Environment\\Objects\\Interactables",
		".fbx", false, ModelPrototypeTags, ModelPrototypePath)))
		return E_FAIL;

#pragma endregion

#ifdef gimch

vector<future<void>> jobFutures;

_uint iLoadCount = 46;
vector<vector<FOLDER_LOAD*>*> Contents(iLoadCount);

_bool isLoad_Map = { true };
if(isLoad_Map)
{
	{ /* Terrain */
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Collision/Terrain",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/Terrain",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}

	{ /* Ollivanders*/
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Ollivanders/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Ollivanders/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}

	{ /* Gatehouse*/
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Gatehouse/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}

	{ /* TScrolls */
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_TScrolls/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_TScrolls/Collisions",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* 3BroomStick*/
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_ThreeBroomsticks/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_ThreeBroomsticks/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_ThreeBroomsticks/Meshes/3Broom_Kit",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}


	{ /* BLDG_QuidditchShop */
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_QuidditchShop/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_QuidditchShop/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* BLDG_HogsheadInn */
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_HogsheadInn/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_HogsheadInn/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* BLDG_Honeydukes */
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Honeydukes/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Honeydukes/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* BLDG_OwlPost */
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_OwlPost/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_OwlPost/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* BLDG_TeaShop */
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_TeaShop/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_TeaShop/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* BLDG_Zonkos */
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Zonkos/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Zonkos/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* BLDG_DB_GR */
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_DB_GR/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_DB_GR/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* BLDG_Potions */
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Potions/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Potions/Collisions",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* BLDG_Salon */
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Salon/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Salon/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* Hengist_Tree */
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/Vegetation/Hengist_Tree",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Collision/Vegetation",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* GEN A*/
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_A/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_A/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* GEN B*/
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_B/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_B/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* GEN C*/
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_C/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_C/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* GEN E*/
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_E/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_E/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* GEN F*/
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_F/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_F/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* GEN G*/
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_G/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_G/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* GEN H*/
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_H/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_H/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
	{ /* GEN J*/
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_J/Meshes",
			".bin", false,
			&Contents[jobFutures.size()]
		));
		jobFutures.emplace_back(Deferred_FolderLoad(
			"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_J/Collision",
			".bin", false,
			&Contents[jobFutures.size()]
		));
	}
}

if(isLoad_Map)
{
	for (auto& jobFuture : jobFutures)
	{
		jobFuture.get();
	}

	for (_uint i = 0; i < Contents.size(); ++i) {
		for (_uint j = 0; j < (Contents[i])->size(); ++j) {
			FOLDER_LOAD* pContents = (*Contents[i])[j];
			if (true == pContents->bLoadTags) {
				ModelPrototypeTags.push_back(pContents->pModelTag);
				ModelPrototypePath.push_back(pContents->pathModel);
			}

			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, pContents->pModelTag, pContents->pLoadedModel))) {
				return E_FAIL;
			}

			for (_uint k = 0; k < pContents->pRigidBodyTags.size(); ++k) {
				if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, pContents->pRigidBodyTags[k], pContents->LoadedRigidBody[k]))) {
					return E_FAIL;
				}
			}

		}
	}
}
	
	m_strMessage = TEXT("쉐이더를(을) 로딩 중 입니다.");

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

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
	{
		Desc.eType = ACTOR::BOX;
		Desc.ePxRigidBodyFlags = { PSX::PxRigidBodyFlag::eKINEMATIC };
		Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
		Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
		Desc.vMatInfo = { 0.5f, 0.5f, 0.6f };
		Desc.fContactOffset = { 0.05f };
		Desc.vhalfGeometryInfo = { 0.5f, 0.5f, 0.5f };
		Desc.fDensity = 10.f;
		Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
		Desc.eLockFlag = {};
		Desc.vAutoDamping = { };
	}
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
		return E_FAIL;
	}

	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Collider"),
		CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	{
		CVIBuffer_Terrain* pTerrain = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, "Hogsmeade_HeightMap.bin", 512, 512);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Terrain_Hogsmeade"), pTerrain))) {
			return E_FAIL;
		}
		pTerrain->ConvertToHeightField(TEXT("Hogsmeade_HeightMap"));

		CRigidBody_Static::RIGIDBODY_STATIC_PROTOTYPEDESC Desc{}; // 터레인 리지드 지형 추가
		{
			Desc.eType = ACTOR::HEIGHTFIELD;
			Desc.ePxRigidBodyFlags = {};
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = PXMATERIAL::DEFAULT;
			Desc.vMatInfo = _float3(0.5f, 0.5f, 0.6f);
			Desc.fContactOffset = 0.001f;
		}
		CRigidBody_Static* pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_RigidBody_Static_Terrain_Hogsmeade"), pRigid))) {
			return E_FAIL;
		}
	}


	CVIBuffer_Terrain* pTerrain = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, "../Bin/Resources/Data/Map/Terrain/Hogwart_Height.png");
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Terrain_Hogwart"), pTerrain))) {
		return E_FAIL;
	}

	//pTerrain = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, "../Bin/Resources/Data/Map/Terrain/North_Hogwart_Height.png");
	//if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Terrain_North_Hogwart"), pTerrain))) {
	//	return E_FAIL;
	//}

	//pTerrain = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, "../Bin/Resources/Data/Map/Terrain/South_Hogwart_Height.png");
	//if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Terrain_South_Hogwart"), pTerrain))) {
	//	return E_FAIL;
	//}

#pragma region INSTANCE_MODEL
	CVIBuffer_Model_Instance::INSTANCE_DESC InstanceDesc = {};

	InstanceDesc.iNum = 100;

	///* For.Prototype_Component_VIBuffer_Model_Instancel */
	//if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel"),
	//	CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext, &InstanceDesc,
	//		"../Bin/Resources/Models/InstanceProp/SM_OakTree_MedA.fbx", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
	//	return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_SM_OakTree_MedA */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_SM_OakTree_MedA"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_OakTree_MedA.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_SM_BearBerry_A */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_SM_BearBerry_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext, 
			"../Bin/Resources/Models/InstanceProp/SM_BearBerry_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
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

	/* For.Prototype_Component_VIBuffer_Model_Instancel_OakTree_TallA*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_OakTree_TallA"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_OakTree_TallA.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_Shrub_B*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_Shrub_B"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_GenericShrub_B.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_BogMyrtle_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_BogMyrtle_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_BogMyrtle_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_Dogwood_B*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_Dogwood_B"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_Dogwood_B.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_ScotsPine_LargeA*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Model_Instancel_ScotsPine_LargeA"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_ScotsPine_LargeA_Master.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;
#pragma endregion // INSTANCE_MODEL

	/* For.Prototype_Component_VIBuffer_Box */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Box"),
		CVIBuffer_Box::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_OcclusionQuery */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_OcclusionQuery"),
		COcclusionQuery::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype<CDummySkyBox>(g_iStaticLevel, CDummySkyBox::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Land */
	if (FAILED(m_pGameInstance->Add_Prototype<CLand>(g_iStaticLevel, CLand::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_BuildingContainer */
	if (FAILED(m_pGameInstance->Add_Prototype<CBuildingContainer>(g_iStaticLevel, CBuildingContainer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapObject_Collision */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapObject_Collision>(g_iStaticLevel, CMapObject_Collision::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapObject_LOD */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapObject_LOD>(g_iStaticLevel, CMapObject_LOD::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Interactable */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Interactable>(g_iStaticLevel, CMapElement_Interactable::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Door */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Door>(g_iStaticLevel, CMapElement_Door::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Chest */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Chest>(g_iStaticLevel, CMapElement_Chest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Chest_Lid */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Chest_Lid>(g_iStaticLevel, CMapElement_Chest_Lid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Static */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Static>(g_iStaticLevel, CMapElement_Static::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Light */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Light>(g_iStaticLevel, CMapElement_Light::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype<CTerrain>(g_iStaticLevel, CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_InstancedProp */
	if (FAILED(m_pGameInstance->Add_Prototype<CInstancedProp>(g_iStaticLevel, CInstancedProp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Unified */
	if (FAILED(m_pGameInstance->Add_Prototype<CUnified>(g_iStaticLevel, CUnified::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Unified */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Lake>(g_iStaticLevel, CMapElement_Lake::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapObject_Manager */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapObject_Manager>(g_iStaticLevel, CMapObject_Manager::Create(m_pDevice, m_pContext, ModelPrototypeTags, ModelPrototypePath))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXMesh>(g_iStaticLevel, CDummy_PhysXMesh::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	if(isLoad_Map)
	{
		for (_uint i = 0; i < Contents.size(); ++i) {
			for (_uint j = 0; j < (Contents[i])->size(); ++j) {
				Safe_Delete((*Contents[i])[j]);
			}
			Safe_Delete(Contents[i]);
		}
	}
#endif
	return S_OK;
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
//
//
