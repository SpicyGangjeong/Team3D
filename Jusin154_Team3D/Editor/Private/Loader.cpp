#include "pch.h"
#include "Loader.h"
#include "DebugCamera.h"
#include "GameInstance.h"
#include "MainApp.h"
#include "Dummy_Goblin.h"
#include "Dummy_Cube.h"

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
		return 1;
	}

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{

	if (FAILED(CoInitializeEx(nullptr, 0))) {
		return E_FAIL;
	}

	EnterCriticalSection(&m_CriticalSection);

	HRESULT		hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Loading_For_Logo();
		break;
	//case LEVEL::MAP:
	//	hr = Loading_For_MapViewer();
	//	break;
	case LEVEL::OBJECT:
		hr = Loading_For_ObjectViewer();
		break;
	//case LEVEL::COMBINED:
	//	hr = Loading_For_CombinedViewer();
	//	break;
	//case LEVEL::EFFECT:
	//	hr = Loading_For_Effect();
	//	break;
	//case LEVEL::SKIllSTUDIO:
	//	hr = Loading_For_SkillStudio();
	//	break;
	//case LEVEL::PARTICLE:
	//	hr = Loading_For_Particle();
	//	break;
	default:
		assert(false);
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

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
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

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

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	if (FAILED(m_pGameInstance->Add_Prototype<CDebugCamera>(g_iStaticLevel, CDebugCamera::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_ObjectViewer()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");
	
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_GoblinBody_Model"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Goblin/GoblinBody.fbx", MODEL::ANIM, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixIdentity(), 0))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Desc_Box"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Box/Box.fbx", MODEL::NONANIM, XMMatrixIdentity(), 0))))
		return E_FAIL;

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("이펙트를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	/* For.Prototype_GameObject_Dummy_Goblin */
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_Goblin>(g_iStaticLevel, CDummy_Goblin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Dummy_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_Cube>(g_iStaticLevel, CDummy_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strMessage = TEXT("정보를 불러오는 중입니다.");

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

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

	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
