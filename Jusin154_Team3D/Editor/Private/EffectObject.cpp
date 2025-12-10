#include "pch.h"
#include "EffectObject.h"
#include "GameInstance.h"

CEffectObject::CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPartObject(pDevice, pContext)
{
}

CEffectObject::CEffectObject(const CEffectObject& rhs)
	:CPartObject(rhs),
	m_strPath(rhs.m_strPath)
{
}

void CEffectObject::Priority_Update(_float fTimeDelta)
{
}

void CEffectObject::Update(_float fTimeDelta)
{

}

void CEffectObject::Late_Update(_float fTimeDelta)
{

}

HRESULT CEffectObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pInstance_ModelCom->Get_NumMeshes(); i++)
	{


		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(m_EffectInfo.eShaderPass)))) {
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Bind_DepthStencil(m_pShaderCom, "g_DepthStencilTexture")))
			return E_FAIL;

		if (FAILED(m_pInstance_ModelCom->Bind_CS_Output(5, 1)))
			return E_FAIL;

		if (FAILED(m_pInstance_ModelCom->Render(i)))
		{
			return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CEffectObject::Render_Blur()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBlurIntensity", &m_EffectInfo.fBlurIntensity, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iBlurWeight", &m_EffectInfo.iBlurWeight, sizeof(_int)))) {
		return E_FAIL;
	}

	SHADER_PASS_INSTANCE_MODEL BlurPass = {};

	if (m_EffectInfo.eShaderPass == SHADER_PASS_INSTANCE_MODEL::NON_WORLD || m_EffectInfo.eShaderPass == SHADER_PASS_INSTANCE_MODEL::BLEND_NOWORLD)
	{
		if (m_EffectInfo.isBlurNoEmissive == false)
		{
			BlurPass = SHADER_PASS_INSTANCE_MODEL::NON_WORLD_BLUR;
		}
		else
		{
			BlurPass = SHADER_PASS_INSTANCE_MODEL::BLUR_NO_WORLD_NO_EMISSIVE;
		}
	}
	else
	{
		if (m_EffectInfo.isBlurNoEmissive == false)
		{
			BlurPass = SHADER_PASS_INSTANCE_MODEL::BLUR;
		}
		else
		{
			BlurPass = SHADER_PASS_INSTANCE_MODEL::BLUR_NO_EMMISVE;
		}


	}

	for (_uint i = 0; i < m_pInstance_ModelCom->Get_NumMeshes(); i++)
	{
	

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(BlurPass)))) {
			return E_FAIL;
		}

		if (FAILED(m_pInstance_ModelCom->Bind_CS_Output(5, 1)))
			return E_FAIL;


		if (FAILED(m_pInstance_ModelCom->Render(i)))
		{
			return E_FAIL;
		}

	}




	return S_OK;
}

HRESULT CEffectObject::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBloomStrength", &m_EffectInfo.fBloomStrength, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iBloomType", &m_EffectInfo.eBloomType, sizeof(_int)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isBloomDissolve", &m_EffectInfo.isBloomDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isBloomReverseDissolve", &m_EffectInfo.isBloomReverseDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	SHADER_PASS_INSTANCE_MODEL BloomPass = {};

	if (m_EffectInfo.eShaderPass == SHADER_PASS_INSTANCE_MODEL::NON_WORLD || m_EffectInfo.eShaderPass == SHADER_PASS_INSTANCE_MODEL::BLEND_NOWORLD)
	{
		BloomPass = SHADER_PASS_INSTANCE_MODEL::BLOOM_NOWORLD;
	}
	else
	{
		BloomPass = SHADER_PASS_INSTANCE_MODEL::BLOOM;
	}

	for (_uint i = 0; i < m_pInstance_ModelCom->Get_NumMeshes(); i++)
	{

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(BloomPass)))) {
			return E_FAIL;
		}

		if (FAILED(m_pInstance_ModelCom->Render(i)))
		{
			return E_FAIL;
		}

	}

	return S_OK;
}





HRESULT CEffectObject::Load(const _char* pFilePath , LEVEL eLevel)
{
	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pNoise_TextureCom);
	SAFE_RELEASE(m_pMasking_TextureCom);
	SAFE_RELEASE(m_pDissolve_TextureCom);
	SAFE_RELEASE(m_pEmissive_TextureCom);

	SAFE_RELEASE(m_pInstance_ModelCom);

	if (m_pLightCom != nullptr)
		SAFE_RELEASE(m_pLightCom);
	
	_string strPerfectFilePath = pFilePath;

	m_strPath = strPerfectFilePath;

	strPerfectFilePath += ".bin";

	HANDLE hFile = CreateFileW(
		CMyTools::ToWstring(strPerfectFilePath).c_str(),               // 파일 이름
		GENERIC_READ,              // 읽기 모드
		FILE_SHARE_READ,           // 다른 프로세스도 읽기 가능
		NULL,
		OPEN_EXISTING,             // 기존 파일 열기
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);


	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"오브젝트 읽기 실패", L"System Message", MB_OK);
		return E_FAIL;
	}

	// 나중에 패키징할때 패스를 저장할거임


	DWORD	dwByte(0);

	if (!ReadFile(hFile, &m_EffectInfo, sizeof(EFFECT_INFO), &dwByte, nullptr)) {
		CloseHandle(hFile);
		return E_FAIL;
	}

	m_EffectInfo.LightDesc.pPosition = m_pTransformCom->Get_StatePtr(STATE::POSITION);
	m_EffectInfo.LightDesc.iLevel = ENUM_CLASS(eLevel);


	if (m_EffectInfo.LightDesc.eType != LIGHT::DIRECTIONAL) // 0이 아닐때만 생성
	{
		if (FAILED(Add_Component<CLight>(g_iStaticLevel, &m_pLightCom, &m_EffectInfo.LightDesc)))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		m_pLightCom->Set_LightIntensity(m_EffectInfo.fLightIntensity);
	}


		

	if (m_EffectInfo.isDiffuse)
	{
		size_t iComponentLength = {};

		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			m_strDiffuseName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strDiffuseName),
				reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom))))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}

		}


	}

	if (m_EffectInfo.isNoise)
	{
		size_t iComponentLength = {};


		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			m_strNoiseName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strNoiseName),
				reinterpret_cast<CComponent**>(&m_pNoise_TextureCom))))
				return E_FAIL;
		}


	}

	if (m_EffectInfo.isMasking)
	{
		size_t iComponentLength = {};


		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{

			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			m_strMaskingName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strMaskingName),
				reinterpret_cast<CComponent**>(&m_pMasking_TextureCom))))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}
		}

	
	}

	if (m_EffectInfo.isDissolve)
	{
		size_t iComponentLength = {};

		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			m_strDissolveName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strDissolveName),
				reinterpret_cast<CComponent**>(&m_pDissolve_TextureCom))))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}


		}

	
	}

	if (m_EffectInfo.isEmissive)
	{
		size_t iComponentLength = {};

		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			m_strEmissiveName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strEmissiveName),
				reinterpret_cast<CComponent**>(&m_pEmissive_TextureCom))))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}
		}

	
	}

	if (m_EffectInfo.isDistortion)
	{
		size_t iComponentLength = {};

		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			m_strDistortionName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strDistortionName),
				reinterpret_cast<CComponent**>(&m_pDistortion_TextureCom))))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}
		}


	}

	size_t iComponentLength = {};


	if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
		CloseHandle(hFile);
		return E_FAIL;
	}

	if (iComponentLength != 0)
	{
		_char szName[MAX_PATH] = {};

		if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)(DWORD)iComponentLength + 1), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		m_strModelName = szName;

		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strModelName),
			reinterpret_cast<CComponent**>(&m_pInstance_ModelCom))))
		{
			CloseHandle(hFile);
			return E_FAIL;;
		}
	}



	m_pInstance_ModelCom->Load_InstanceModel(hFile);

	CloseHandle(hFile);



	return S_OK;
}

HRESULT CEffectObject::Load()
{

	if (FAILED(Load(m_strPath.c_str(), static_cast<LEVEL>(NEXT_LEVEL))))
		return E_FAIL;



	return S_OK;
}

#ifdef _DEBUG

HRESULT CEffectObject::LoadPre(const _char* pFilePath, LEVEL eLevel)
{

	_string strPerfectFilePath = pFilePath;

	HANDLE hFile = CreateFileW(
		CMyTools::ToWstring(strPerfectFilePath).c_str(),               // 파일 이름
		GENERIC_READ,              // 읽기 모드
		FILE_SHARE_READ,           // 다른 프로세스도 읽기 가능
		NULL,
		OPEN_EXISTING,             // 기존 파일 열기
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);


	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"오브젝트 읽기 실패", L"System Message", MB_OK);
		return E_FAIL;
	}


	DWORD	dwByte(0);

	PRE_EFFECT_INFO PreEffectInfo = {};

	if (!ReadFile(hFile, &m_EffectInfo, sizeof(PRE_EFFECT_INFO), &dwByte, nullptr)) {
		return E_FAIL;
	}

	m_EffectInfo.LightDesc.pPosition = m_pTransformCom->Get_StatePtr(STATE::POSITION);
	m_EffectInfo.LightDesc.iLevel = ENUM_CLASS(eLevel);


	if (m_EffectInfo.LightDesc.eType != LIGHT::DIRECTIONAL) // 0이 아닐때만 생성
	{
		if (FAILED(Add_Component<CLight>(g_iStaticLevel, &m_pLightCom, &m_EffectInfo.LightDesc)))
		{
			return E_FAIL;
		}
	}


	if (m_EffectInfo.isDiffuse)
	{
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

			m_strDiffuseName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strDiffuseName),
				reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom))))
				return E_FAIL;

		}
	}

	if (m_EffectInfo.isNoise)
	{
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

			m_strNoiseName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strNoiseName),
				reinterpret_cast<CComponent**>(&m_pNoise_TextureCom))))
				return E_FAIL;
		}


	}

	if (m_EffectInfo.isMasking)
	{
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

			m_strMaskingName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strMaskingName),
				reinterpret_cast<CComponent**>(&m_pMasking_TextureCom))))
				return E_FAIL;
		}


	}

	if (m_EffectInfo.isDissolve)
	{
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

			m_strDissolveName = szName;


			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strDissolveName),
				reinterpret_cast<CComponent**>(&m_pDissolve_TextureCom))))
				return E_FAIL;
		}

	}

	if (m_EffectInfo.isEmissive)
	{
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

			m_strEmissiveName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strEmissiveName),
				reinterpret_cast<CComponent**>(&m_pEmissive_TextureCom))))
				return E_FAIL;
		}


	}

	if (m_EffectInfo.isDistortion)
	{
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

			m_strDistortionName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strDistortionName),
				reinterpret_cast<CComponent**>(&m_pDistortion_TextureCom))))
				return E_FAIL;
		}


	}

	size_t iComponentLength = {};


	if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (iComponentLength != 0)
	{
		_char szName[MAX_PATH] = {};

		if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)(DWORD)iComponentLength + 1), &dwByte, nullptr)) {
			return E_FAIL;
		}

		m_strModelName = szName;


		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strModelName),
			reinterpret_cast<CComponent**>(&m_pInstance_ModelCom))))
			return E_FAIL;;
	}


	m_pInstance_ModelCom->PreLoad(hFile);

	CloseHandle(hFile);
	
	return S_OK;
}
#endif // _DEBUG

void CEffectObject::FollowParants(const _float4x4* pParantsMat)
{
	if (pParantsMat == nullptr)
		return;


	m_pPerentMatrix = pParantsMat;
	m_bVisible = true;
}


HRESULT CEffectObject::Bind_ShaderResources()
{

	if (m_pPerentMatrix != nullptr)
	{

		_matrix socketMatrix = XMLoadFloat4x4(m_pPerentMatrix);

		for (int i = 0; i < 3; ++i) {
			socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
		}
		socketMatrix = socketMatrix * m_pParentTransformCom->Get_XMWorldMatrix();

		m_pTransformCom->Set_State(STATE::POSITION, socketMatrix.r[3]);
	}


	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture"))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW_INV)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ_INV)))) {
		return E_FAIL;

	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isDiffuse", &m_EffectInfo.isDiffuse, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isMasking", &m_EffectInfo.isMasking, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isDissolve", &m_EffectInfo.isDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isNoise", &m_EffectInfo.isNoise, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isEmissive", &m_EffectInfo.isEmissive, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isDistortion", &m_EffectInfo.isDistortion, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isDiffuseUVMove", &m_EffectInfo.isDiffuseUVMove, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isMaskUVMove", &m_EffectInfo.isMaskUVMove, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isNoiseUVMove", &m_EffectInfo.isNoiseUVMove, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isReverseDissolve", &m_EffectInfo.isReverseDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isEmissiveDissolve", &m_EffectInfo.isEmissiveDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isMaskClampSample", &m_EffectInfo.isMaskClampSample, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isNoiseColor", &m_EffectInfo.isNoiseColor, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isNoiseAlpha", &m_EffectInfo.isNoiseAlpha, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isNomalDissolve", &m_EffectInfo.isNomalDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isEmissiveDissolveReverse", &m_EffectInfo.isEmissiveDissolveReverse, sizeof(_bool)))) {
		return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_isBlurNoEmissive", &m_EffectInfo.isBlurNoEmissive, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isTexBlur", &m_EffectInfo.isTexBlur, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isBlurDissolve", &m_EffectInfo.isBlurDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isBlurReverseDissolve", &m_EffectInfo.isBlurReverseDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_EffectInfo.vColor, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vEmissive", &m_EffectInfo.vEmissive, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveStrength", &m_EffectInfo.fEmissiveStrength, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNoiseDistortionIntensity", &m_EffectInfo.fNoiseDistortionIntensity, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDiffuseUVGainAmount", &m_EffectInfo.vDiffuseUVGainAmount, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskingUVGainAmount", &m_EffectInfo.vMaskingUVGainAmount, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vNoiseUVGainAmount", &m_EffectInfo.vNoiseUVGainAmount, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDistortionTime", &m_EffectInfo.vDistortionTime, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskDistortionUVGainAmount", &m_EffectInfo.vMaskDistortionUVGainAmount, sizeof(_float2)))) {
		return E_FAIL;
	}


	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVCutting", &m_EffectInfo.vUVCutting, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVMaskCutting", &m_EffectInfo.vUVMaskCutting, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iMaskMoveLerpOption", &m_EffectInfo.iMaskMoveLerpOption, sizeof(_int)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iDiffuseMoveLerpOption", &m_EffectInfo.iDiffuseMoveLerpOption, sizeof(_int)))) {
		return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_iMaskDistortionMoveLerpOption", &m_EffectInfo.iMaskDistortionMoveLerpOption, sizeof(_int)))) {
		return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_iDiffuseDistortionMoveLerpOption", &m_EffectInfo.iDiffuseDistortionMoveLerpOption, sizeof(_int)))) {
		return E_FAIL;
	}



	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseAlpha", &m_EffectInfo.fDiffuseAlpha, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSoftenExp", &m_EffectInfo.fSoftenExp, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSoftStrength", &m_EffectInfo.fSoftStrength, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCoreBoost", &m_EffectInfo.fCoreBoost, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRadius", &m_EffectInfo.fRadius, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSoftMaskEdge", &m_EffectInfo.fSoftMaskEdge, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSoftMask", &m_EffectInfo.fSoftMask, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBluringStrength", &m_EffectInfo.fBluringStrength, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveDelay", &m_EffectInfo.fDissolveDelay, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fReverseDissolveDelay", &m_EffectInfo.fReverseDissolveDelay, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDissolveUVGainAmount", &m_EffectInfo.vDissolveUVGainAmount, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isDissolveMove", &m_EffectInfo.isDissolveMove, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveMaskEdge", &m_EffectInfo.vDissolveValue.x, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveSoftMask", &m_EffectInfo.vDissolveValue.y, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveCutRatio", &m_EffectInfo.vDissolveValue.z, sizeof(_float)))) {
		return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDissolveColor", &m_EffectInfo.vDissolveColor, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDissolveColorCut", &m_EffectInfo.vDissolveColorCut, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vNoiseColor", &m_EffectInfo.vNoiseColor, sizeof(_float4)))) {
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

	if (m_pDissolve_TextureCom != nullptr)
	{
		if (FAILED(m_pDissolve_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0))) {
			return E_FAIL;
		}
	}

	if (m_pEmissive_TextureCom != nullptr)
	{
		if (FAILED(m_pEmissive_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_EmissiveTexture", 0))) {
			return E_FAIL;
		}
	}

	if (m_pDistortion_TextureCom != nullptr)
	{
		if (FAILED(m_pDistortion_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_DistortionTexture", 0))) {
			return E_FAIL;
		}
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}




	return S_OK;
}

HRESULT CEffectObject::Ready_Components(void* pArg)
{
	CTransform::TRANSFORM_DESC TransformDesc = {};

	TransformDesc.fRadius = 20.f;
	TransformDesc.fRotationPerSec = 10.f;
	TransformDesc.fSpeedPerSec = 10.f;


	if (FAILED(__super::Ready_Components(&TransformDesc))) {
		return E_FAIL;
	}
	 

	//나중에 로드할 때 라이트를 값을 입력받아서 생성 하자
	return S_OK;
}

HRESULT CEffectObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffectObject::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	return S_OK;
}



void CEffectObject::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pNoise_TextureCom);
	SAFE_RELEASE(m_pMasking_TextureCom);
	SAFE_RELEASE(m_pDissolve_TextureCom);
	SAFE_RELEASE(m_pEmissive_TextureCom);
	SAFE_RELEASE(m_pDistortion_TextureCom);

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pInstance_ModelCom);

	if(m_pLightCom != nullptr)
		SAFE_RELEASE(m_pLightCom);
}


