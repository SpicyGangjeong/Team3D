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

	if (m_bVisible == false)
		return;

	if (m_TrailInfo.vDistortionTime.y == 0)
		return;

	m_TrailInfo.vDistortionTime.x += fTimeDelta;

	if (m_TrailInfo.vDistortionTime.x > m_TrailInfo.vDistortionTime.y)
	{
		m_TrailInfo.vDistortionTime.x = 0.f;
	}
}

void CTrailObject::Late_Update(_float fTimeDelta)
{

	if (m_bVisible == false)
		return;

	if (m_TrailInfo.isBlur == true)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this);
	}

	if (m_TrailInfo.isOnlyBlur == true)
		return;

	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::EFFECT, this);
	}

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

	if (FAILED(Add_Component<CTrail>(g_iStaticLevel, &m_pTrailCom, &pTrailDesc))) {
		return E_FAIL;
	}

	return S_OK;
}

void CTrailObject::Trail_Update(_fmatrix WorldMat , _float fTimeDelta)
{
	m_pTrailCom->Trail_Update(fTimeDelta, WorldMat);
}

HRESULT CTrailObject::Load_Trail(const _char* pPath, LEVEL eLevel)
{
	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pMasking_TextureCom);
	SAFE_RELEASE(m_pNoise_TextureCom);
	SAFE_RELEASE(m_pDistortion_TextureCom);

	_string strPerfectFilePath = pPath;

	strPerfectFilePath += ".trail";

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

		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strTrailMaskingName),
			reinterpret_cast<CComponent**>(&m_pMasking_TextureCom))))
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

		m_strTrailDistortionName = szName;

		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strTrailDistortionName),
			reinterpret_cast<CComponent**>(&m_pDistortion_TextureCom))))
			return E_FAIL;

	}


	m_pTrailCom->Load_Trail(hFile);


	if (!ReadFile(hFile, &m_TrailInfo, sizeof(TRAIL_INFO), &dwByte, nullptr)) {
		return E_FAIL;
	}

	CloseHandle(hFile);

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

HRESULT CTrailObject::Render_Blur()
{
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_POSTEX::TRAIL_BLUR))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBlurIntensity", &m_TrailInfo.fBlurIntensity, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iBlurWeight", &m_TrailInfo.iBlurWeight, sizeof(_int)))) {
		return E_FAIL;
	}

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
	SAFE_RELEASE(m_pDistortion_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pTrailCom);
}

void CTrailObject::Describe_Entity()
{
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

	/* 디퓨즈 */

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isDiffuse", &m_TrailInfo.isDiffuse, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseAlpha", &m_TrailInfo.fDiffuseAlpha, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_TrailInfo.vColor, sizeof(_float4)))) {
		return E_FAIL;
	}

	/* 마스크 */

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isMask", &m_TrailInfo.isMask, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSoftMask", &m_TrailInfo.fSoftMask, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSoftMaskEdge", &m_TrailInfo.fSoftMaskEdge, sizeof(_float)))) {
		return E_FAIL;
	}

	/* 노이즈 */

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isNoise", &m_TrailInfo.isNoise, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isNoiseColor", &m_TrailInfo.isNoiseColor, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isNoiseAlpha", &m_TrailInfo.isNoiseAlpha, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNoiseStrength", &m_TrailInfo.fNoiseStrength, sizeof(_float)))) {
		return E_FAIL;
	}

	/* 디스토션 */
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isDistortion", &m_TrailInfo.isDistortion, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDistortionTime", &m_TrailInfo.vDistortionTime, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDiffuseDistortionUVGainAmount", &m_TrailInfo.vDiffuseDistortioUVAmount, sizeof(_float2)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDistortionIntensity", &m_TrailInfo.fDistortionIntensity, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskDistortionUVGainAmount", &m_TrailInfo.vMaskDistortionUVAmount, sizeof(_float2)))) {
		return E_FAIL;
	}



	/* 이미시브 */
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vEmissive", &m_TrailInfo.vEmissive, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveStrength", &m_TrailInfo.fEmissiveStrength, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSoftenExp", &m_TrailInfo.fSoftenExp, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSoftStrength", &m_TrailInfo.fSoftStrength, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCoreBoost", &m_TrailInfo.fCoreBoost, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRadius", &m_TrailInfo.fRadius, sizeof(_float)))) {
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

	if (m_pDistortion_TextureCom != nullptr)
	{
		if (FAILED(m_pDistortion_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_DistortionTexture", 0))) {
			return E_FAIL;
		}
	}

	return S_OK;
}
