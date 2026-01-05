#include "pch.h"
#include "TrailObject.h"

#include "GameInstance.h"
#include "Trail.h"
#include "Effect_Container.h"


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


	/* 디졸브 타임*/
	if (m_TrailInfo.vDistortionTime.y != 0)
	{
		m_TrailInfo.vDistortionTime.x += fTimeDelta;

		if (m_TrailInfo.vDistortionTime.x > m_TrailInfo.vDistortionTime.y)
		{
			m_TrailInfo.vDistortionTime.x = 0.f;
		}
	}

	if (m_TrailInfo.isDissolve == true)
	{
		if (m_TrailInfo.vDissolveTime.y != 0)
		{
			m_TrailInfo.vDissolveTime.x += fTimeDelta;

			if (m_TrailInfo.vDissolveTime.x > m_TrailInfo.vDissolveTime.y)
			{
				m_bVisible = false;
				m_TrailInfo.vDissolveTime.x = 0.f;
				m_TrailInfo.isDissolve = false;
			}
		}
	}


	/* 블룸 타임*/

	if (m_TrailInfo.vBloomTime.y != 0)
	{
		m_TrailInfo.vBloomTime.x += fTimeDelta;

		if (m_TrailInfo.vBloomTime.x > m_TrailInfo.vBloomTime.y)
		{
			m_TrailInfo.vBloomTime.x = 0.f;
		}
	}

}

void CTrailObject::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;


	if (m_TrailInfo.isBloom == true)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::BLOOM, this);
	}


	if (m_TrailInfo.isBlur == true)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this);
	}

	if (m_TrailInfo.isOnlyBlur == true)
		return;

	m_pGameInstance->Add_RenderGroup(m_TrailInfo.eRenderOrder, this);

}

HRESULT CTrailObject::Load_Trail(CEffect_Container::TRAIL_SAVE_INFO Trail_Save_Info,  LEVEL eLevel)
{
	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pMasking_TextureCom);
	SAFE_RELEASE(m_pNoise_TextureCom);
	SAFE_RELEASE(m_pDistortion_TextureCom);


	if (Trail_Save_Info.wstrDiffuseName.length() != 0)
	{

		m_strTrailDiffuseName = Trail_Save_Info.wstrDiffuseName;

		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), m_strTrailDiffuseName,
			reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom))))
			return E_FAIL;
	}

	if (Trail_Save_Info.wstrNoiseName.length() != 0)
	{

		m_strTrailNoiseName = Trail_Save_Info.wstrNoiseName;

		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), m_strTrailNoiseName,
			reinterpret_cast<CComponent**>(&m_pNoise_TextureCom))))
			return E_FAIL;
	}


	if (Trail_Save_Info.wstrMaskingName.length() != 0)
	{

		m_strTrailMaskingName = Trail_Save_Info.wstrMaskingName;

		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), m_strTrailMaskingName,
			reinterpret_cast<CComponent**>(&m_pMasking_TextureCom))))
			return E_FAIL;
	}


	if (Trail_Save_Info.wstrDistortionName.length() != 0)
	{

		m_strTrailDistortionName = Trail_Save_Info.wstrDistortionName;

		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), m_strTrailDistortionName,
			reinterpret_cast<CComponent**>(&m_pDistortion_TextureCom))))
			return E_FAIL;


	}

	m_pTrailCom->Load_Trail(Trail_Save_Info.TrailComponentDesc);

	m_TrailInfo = Trail_Save_Info.TrailDesc;

	m_pTrailCom->ReStructVB(m_TrailInfo.iNumVertex);

	return S_OK;
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

		m_strTrailDiffuseName = CMyTools::ToWstring(szName);

		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), m_strTrailDiffuseName,
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

		m_strTrailNoiseName = CMyTools::ToWstring(szName);

		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), m_strTrailNoiseName,
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

		m_strTrailMaskingName = CMyTools::ToWstring(szName);

		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), m_strTrailMaskingName,
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

		m_strTrailDistortionName = CMyTools::ToWstring(szName);

		if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), m_strTrailDistortionName ,
			reinterpret_cast<CComponent**>(&m_pDistortion_TextureCom))))
			return E_FAIL;

	}


	m_pTrailCom->Load_Trail(hFile);


	if (!ReadFile(hFile, &m_TrailInfo, sizeof(TRAIL_INFO), &dwByte, nullptr)) {
		return E_FAIL;
	}

	m_pTrailCom->ReStructVB(m_TrailInfo.iNumVertex);

	CloseHandle(hFile);

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
	
	if (FAILED(Add_Component<CTrail>(g_iStaticLevel, &m_pTrailCom, &pTrailDesc))) {
		return E_FAIL;
	}

	return S_OK;
}

void CTrailObject::Trail_Update(_fmatrix WorldMat, _float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	m_pTrailCom->Trail_Update(fTimeDelta, WorldMat);
}

void CTrailObject::Rope_Trail_Update(_fmatrix WorldMat, _fmatrix EndWorldMat, _float fTimeDelta)
 {
	if (m_bVisible == false)
		return;

	m_pTrailCom->Rope_Trail_Update(WorldMat, fTimeDelta, m_TrailInfo.fDamping, m_TrailInfo.fRopeLength, m_TrailInfo.fMass, EndWorldMat);
}

void CTrailObject::Oneside_Rope_Trail_Update(_fmatrix WorldMat, _float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	m_pTrailCom->Rope_Trail_Update(WorldMat, fTimeDelta, m_TrailInfo.fDamping, m_TrailInfo.fRopeLength, m_TrailInfo.fMass);
}


HRESULT CTrailObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(m_TrailInfo.eShaderPass))))
		return E_FAIL;

	if (FAILED(m_pTrailCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrailObject::Render_Blur()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBlurIntensity", &m_TrailInfo.fBlurIntensity, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iBlurWeight", &m_TrailInfo.iBlurWeight, sizeof(_int)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_POSTEX::TRAIL_BLUR))))
		return E_FAIL;

	if (FAILED(m_pTrailCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrailObject::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBloomStrength", &m_TrailInfo.fBloomStrength, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iBloomType", &m_TrailInfo.eBloomType, sizeof(_int)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isBloomDissolve", &m_TrailInfo.isBloomDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isBloomReverseDissolve", &m_TrailInfo.isBloomReverseDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBloomTime", &m_TrailInfo.vBloomTime, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_POSTEX::TRAIL_BLOOM))))
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

HRESULT CTrailObject::Bind_ShaderResources()
{

	if (FAILED(m_pTrailCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture"))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}


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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isDissolve", &m_TrailInfo.isDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDissolveTime", &m_TrailInfo.vDissolveTime, sizeof(_float2)))) {
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

#ifdef _DEBUG
void CTrailObject::Describe_Entity()
{
	if (GUI::TreeNode("Rope Trail"))
	{

		GUI::DragFloat("Dampling", &m_TrailInfo.fDamping);
		GUI::DragFloat("Rope Length", &m_TrailInfo.fRopeLength);
		GUI::DragFloat("Mass", &m_TrailInfo.fMass);
		GUI::TreePop();
	}
}
#endif
