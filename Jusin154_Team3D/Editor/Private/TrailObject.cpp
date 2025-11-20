#include "pch.h"
#include "TrailObject.h"

#include "GameInstance.h"
#include "Trail.h"


CTrailObject::CTrailObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CTrailObject::CTrailObject(const CTrailObject& rhs)
	: CPartObject(rhs)
{
}

HRESULT CTrailObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrailObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	return S_OK;
}

void CTrailObject::Priority_Update(_float fTimeDelta)
{

}

void CTrailObject::Update(_float fTimeDelta)
{
	m_pTrailCom->Trail_Update(fTimeDelta, m_pParentTransformCom->Get_XMWorldMatrix());
}

void CTrailObject::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::EFFECT, this);
	}

}

#ifdef _DEBUG

HRESULT CTrailObject::Save_Path(HANDLE hFile)
{
	DWORD dwByte;

	size_t iObjectLength = m_strPath.length();
	const _char* pFilePath = m_strPath.c_str();

	if (!WriteFile(hFile, &m_eEffectType, sizeof(EFFECT_TYPE), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (!WriteFile(hFile, &iObjectLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (pFilePath != 0)
	{
		if (!WriteFile(hFile, pFilePath, sizeof(_char) * ((DWORD)iObjectLength + 1), &dwByte, nullptr)) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CTrailObject::Save_Trail(const _char* pPath)
{
	_string strPerfectFilePath = pPath;
	strPerfectFilePath += ".bin";

	HANDLE	hFile = CreateFile(CMyTools::ToWstring(strPerfectFilePath).c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL
	);


	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"트레일 오브젝트 저장 실패", L"System Message", MB_OK);
		return E_FAIL;
	}

	m_strPath = strPerfectFilePath;

	DWORD	dwByte(0);

	if (!WriteFile(hFile, &m_eEffectType, sizeof(EFFECT_TYPE), &dwByte, nullptr)) {
		return E_FAIL;
	}



	size_t iComponentLength = m_strTrailDiffuseName.length();
	const _char* pFileComponentPath = m_strTrailDiffuseName.c_str();

	if (!WriteFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (iComponentLength != 0)
	{
		if (!WriteFile(hFile, pFileComponentPath, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
			return E_FAIL;
		}
	}

	iComponentLength = m_strTrailNoiseName.length();
	pFileComponentPath = m_strTrailNoiseName.c_str();

	if (!WriteFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (iComponentLength != 0)
	{
		if (!WriteFile(hFile, pFileComponentPath, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
			return E_FAIL;
		}
	}


	iComponentLength = m_strTrailMaskingName.length();
	pFileComponentPath = m_strTrailMaskingName.c_str();

	if (!WriteFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (iComponentLength != 0)
	{
		if (!WriteFile(hFile, pFileComponentPath, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
			return E_FAIL;
		}
	}

	m_pTrailCom->Save_Trail(hFile);

	CloseHandle(hFile);

	MessageBox(NULL, L"트레일  저장 성공", L"System Message", MB_OK);
	return S_OK;
}

#endif

HRESULT CTrailObject::Load_Trail(const _char* pPath, LEVEL eLevel)
{
	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pMasking_TextureCom);
	SAFE_RELEASE(m_pNoise_TextureCom);

	_string strPerfectFilePath = pPath;

	strPerfectFilePath += ".bin";

	DWORD	dwByte(0);

	HANDLE hFile = CreateFileW(
		CMyTools::ToWstring(strPerfectFilePath).c_str(),               // 파일 이름
		GENERIC_READ,              // 읽기 모드
		FILE_SHARE_READ,           // 다른 프로세스도 읽기 가능
		NULL,
		OPEN_EXISTING,             // 기존 파일 열기
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	m_strPath = strPerfectFilePath;

	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"오브젝트 읽기 실패", L"System Message", MB_OK);
		return E_FAIL;
	}


	if (!ReadFile(hFile, &m_eEffectType, sizeof(EFFECT_TYPE), &dwByte, nullptr)) {
		return E_FAIL;
	}

	size_t iComponentLength = {};

	if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (iComponentLength != 0)
	{
		_char szName[MAX_PATH] = {};

		if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
			return E_FAIL;
		}

		m_strTrailDiffuseName = szName;

		m_isDiffuse = true;

		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strTrailDiffuseName),
			reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom))))
			return E_FAIL;


	}

	if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (iComponentLength != 0)
	{
		_char szName[MAX_PATH] = {};

		if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
			return E_FAIL;
		}

		m_strTrailNoiseName = szName;

		m_isNoise = true;

		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strTrailNoiseName),
			reinterpret_cast<CComponent**>(&m_pNoise_TextureCom))))
			return E_FAIL;

	}

	if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (iComponentLength != 0)
	{
		_char szName[MAX_PATH] = {};

		if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
			return E_FAIL;
		}
		
		m_strTrailMaskingName = szName;

		m_isMask = true;

		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strTrailMaskingName),
			reinterpret_cast<CComponent**>(&m_pMasking_TextureCom))))
			return E_FAIL;
	}

	m_pTrailCom->Load_Trail(hFile);

	CloseHandle(hFile);


	MessageBox(NULL, L"트레일 로드 성공", L"System Message", MB_OK);

	return S_OK;
}

HRESULT CTrailObject::Ready_Components(void* pArg)
{

	if (FAILED(__super::Ready_Components(nullptr))) {
		return E_FAIL;
	}

	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_POSTEX, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	CTrail::TRAIL_DESC pTrailDesc;

	pTrailDesc.vHigh = _float3(0.f, 1.f, 0.f);
	pTrailDesc.vLow = _float3(0.f, -1.f, 0.f);

	if (FAILED(Add_Component<CTrail>(ENUM_CLASS(LEVEL::EFFECT), &m_pTrailCom, &pTrailDesc))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTrailObject::Render()
{
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_POSTEX::TRAIL))))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTrailCom->Render()))
		return E_FAIL;

	return S_OK;
}




CTrailObject* CTrailObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrailObject* pInstance = new CTrailObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrailObject");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CTrailObject::Clone(void* pArg, CGameObject* pOwner)
{
	CTrailObject* pInstance = new CTrailObject(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrailObject");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTrailObject::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pMasking_TextureCom);
	SAFE_RELEASE(m_pNoise_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pTrailCom);
}

void CTrailObject::Describe_Entity()
{

	if (GUI::TreeNode("TRAIL"))
	{
		m_pTrailCom->Describe_Entity();

		GUI::Checkbox("Diffuse", &m_isDiffuse);
		GUI::Checkbox("Masking", &m_isMask);
		GUI::Checkbox("Noise", &m_isNoise);

		if (m_isDiffuse == true)
		{
			if (GUI::TreeNode("TRAIL DIFFUSE"))
			{
				_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "DIFFUSE_TEXTURE", (CComponent**)&m_pDiffuse_TextureCom, nullptr, this);

				if (strName != "") {
					m_strTrailDiffuseName = strName;
				}

				GUI::TreePop();
			}
		}

		if (m_isMask == true)
		{
			if (GUI::TreeNode("TRAIL MASK"))
			{
				_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "MASK_TEXTURE", (CComponent**)&m_pMasking_TextureCom, nullptr, this);

				if (strName != "") {
					m_strTrailMaskingName = strName;
				}

				GUI::TreePop();
			}
		}

		if (m_isNoise == true)
		{
			if (GUI::TreeNode("TRAIL NOISE"))
			{
				_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "NOISE_TEXTURE", (CComponent**)&m_pNoise_TextureCom, nullptr, this);

				if (strName != "") {
					m_strTrailNoiseName = strName;
				}

				GUI::TreePop();
			}
		}

		GUI::TreePop();
	}

	GUI::Separator();
	GUI::Spacing();
	
}

HRESULT CTrailObject::Bind_ShaderResources()
{

	if (FAILED(m_pTrailCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isDiffuse", &m_isDiffuse, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isMask", &m_isMask, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isNoise", &m_isNoise, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (m_pDiffuse_TextureCom != nullptr)
	{
		if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0))) {
			return E_FAIL;
		}
	}

	if (m_pNoise_TextureCom != nullptr)
	{
		if (FAILED(m_pNoise_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 0))) {
			return E_FAIL;
		}
	}

	if (m_pMasking_TextureCom != nullptr)
	{
		if (FAILED(m_pMasking_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskingTexture", 0))) {
			return E_FAIL;
		}
	}

	return S_OK;
}
