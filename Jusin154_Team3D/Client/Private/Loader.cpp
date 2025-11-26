#include "pch.h"
#include "Loader.h"
#include "GameInstance.h"
#include "MainApp.h"
#include "Light_Main.h"
#include "Camera_Debug.h"
//#include "GamePlay_Canvas.h"
//#include "Loading_Panel.h"
//#include "LoadingWidget.h"

#include "Player.h"
#include "Camera_Gaze.h"
#include "CamPosition_Socket.h"
#include "CamPosition_Target.h"
#include "CamPosition_Shoulder.h"
#include "CamPosition_Arm.h"
#include "Wand.h"
#include "SkyBox.h"
#include "Broom.h"
#include "Goblin.h"

#pragma region PHYSX

#include "Dummy_PhysXWall.h"
#include "Dummy_PhysXPlayable.h"
#include "PhysXEffectHitBox.h"

#pragma endregion

#pragma region EFFECT

#include "EffectParts.h"
#include "Bombard.h"
#include "TrailObject.h"
#include "Instance_Model.h"
#include "Trail.h"

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

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("LoadingWidget"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Loading/LoadingWidget.png"), 0)))) {
		return E_FAIL;
	}


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

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

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

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");
	
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_UIEDITOR,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_UIEditor.hlsl"),
			VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_INSTANCE_MODEL,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxModelInstance.hlsl"),
			VTX_MODEL_INSTANCE_PARTICLE::Elements, VTX_MODEL_INSTANCE_PARTICLE::iNumElements)))) {
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
		Desc.vhalfGeometryInfo = { 2.5f, 4.5f, 3.5f };
		Desc.fDensity = 10.f;
		Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
		Desc.eLockFlag = {};
		Desc.vAutoDamping = { 100.f, 100.f };
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

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Box"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Box/Box.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixIdentity())))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Npc_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Human/Npc/Npc.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Wand_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Object/Wand/Wand.bin", XMMatrixIdentity()))))

		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Broom_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Object/Broom/Broom.bin", XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Goblin_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Goblin/Goblin.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity()))))
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



	m_strMessage = TEXT("Model Loading..");


	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_SkyboxModel"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/SkyBox/SkyBox.bin", XMMatrixIdentity()))))
		return E_FAIL;

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

	//if (FAILED(m_pGameInstance->Add_Prototype<CGamePlay_Canvas>(g_iStaticLevel, CGamePlay_Canvas::Create(m_pDevice, m_pContext)))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pGameInstance->Add_Prototype<CLoading_Panel>(g_iStaticLevel, CLoading_Panel::Create(m_pDevice, m_pContext)))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pGameInstance->Add_Prototype<CLoadingWidget>(g_iStaticLevel, CLoadingWidget::Create(m_pDevice, m_pContext)))) {
	//	return E_FAIL;
	//}
		
	/* For.Prototype_GameObject_SkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype<CSkyBox>(g_iStaticLevel, CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_FSM */
	if (FAILED(m_pGameInstance->Add_Prototype<CFSM>(g_iStaticLevel, CFSM::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype<CPlayer>(g_iStaticLevel, CPlayer::Create(m_pDevice, m_pContext))))
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

	/* For.Prototype_GameObject_Broom */
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom>(g_iStaticLevel, CBroom::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Goblin */
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin>(g_iStaticLevel, CGoblin::Create(m_pDevice, m_pContext))))
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
