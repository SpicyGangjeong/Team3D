#pragma once

#include "Editor_Define.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CInstance_Model;
class CShader;
class CTexture;
class CLight;
class CTrail;
NS_END

NS_BEGIN(Editor)

class CEffectObject abstract : public CPartObject
{
public:
	typedef struct tagEffectInfo
	{
		_float4 vColor = { 0.f ,0.f ,0.f ,1.f };

		_float2 vDiffuseUVGainAmount = {};
		_float2 vMaskingUVGainAmount = {};
		_float2 vNoiseUVGainAmount = {};

		_float2 vDistortionTime = {};
		_float2 vMaskDistortionUVGainAmount = {};

		_float2 vUVCutting = { 1.f ,1.f };
		_float2 vUVMaskCutting = { 1.f ,1.f };

		_int    iMaskMoveLerpOption = { 0 };
		_int    iDiffuseMoveLerpOption = { 0 };
		_int    iNoiseMoveLerpOption = { 0 };

		_int    iMaskDistortionMoveLerpOption = { 0 };
		_int    iDiffuseDistortionMoveLerpOption = { 0 };

		_int	iBlurWeight = { 0 };


		_bool	isDiffuse = {};
		_bool	isMasking = {};
		_bool	isDissolve = {};
		_bool	isNoise = {};
		_bool   isEmissive = {};
		_bool   isDistortion = {};
		_bool	isDiffuseUVMove = {};
		_bool   isNoiseUVMove = {};
		_bool	isMaskUVMove = {};
		_bool   isBlur = {};
		_bool   isBillboard = {};
	
		_float4 vEmissive = { 0.f ,0.f ,0.f ,0.f };
		_float  fDiffuseAlpha = { 1.f };
		_float  fBlurIntensity = {};
		_float  fNoiseDistortionIntensity = {};
		_float  fEmissiveStrength = { 0.f };

		LIGHT_DESC	LightDesc = {};
		RENDER		eRenderOrder = { RENDER::EFFECT };

		_bool   isReverseDissolve = {};

		EFFECT_TYPE eEffectType = { EFFECT_TYPE::EFFECT };

		_bool   isEmissiveDissolve = { false };

		_bool   isMaskClampSample = { false };
		_bool   isNoiseColor = { false };
		_bool   isNoiseAlpha = { false };
		_bool	isNomalDissolve = {};

		_float fSoftenExp = { 1.31429f };
		_float fSoftStrength = {};
		_float fCoreBoost = {};
		_float fRadius = {};

		_float fSoftMaskEdge = {};
		_float fSoftMask = {};

		_bool   isEmissiveDissolveReverse = { false };
		_bool   isOnlyBlur = { false };

		SHADER_PASS_INSTANCE_MODEL eShaderPass = { SHADER_PASS_INSTANCE_MODEL::NON_NOMALMAP };

		_bool       isBlurNoEmissive = {};


		_bool	    isMotionBlur = {};

		_bool       isNoise_G = {};
		_bool       isNoise_B= {};

		_float      fModelBlurIntensity = {0.f};

		_bool       isBloom = {};
		_bool       isBloomDissolve = {};
		_bool       isBloomReverseDissolve = {};
		_float      fBloomStrength = {};
		BLOOM_TYPE  eBloomType = {};


		_float		fDissolveDelay = {};
		_float		fReverseDissolveDelay = {};
		_float2		vDissolveUVGainAmount = {};
		_bool		isDissolveMove = {};

		_float3		vDissolveValue = {}; /* Soft Mask , Cut Ratio*/

		_float      fModelDistortIntensity = {};
		_float		fPadding0 = {};

		_float      fLightDeley = {};
		_float		fLightIntensity = {};
		_float		isLightTime = { 0.f };
		_bool		isLightDissolve = {};

		_bool		isDissolve_G = {};

		_float4     vBlurColor = {};
		_float2     vUVNoiseCutting = { 1.f ,1.f };

		_float		fEmissiveColorCut = {};

		_float      fRimLightPower = {};
		_float      fRimLightStrength = {};

		_float4     vNoiseColor = {};
		_float4     vRimLightColor = {};

		_float2     vMaskOffset = {};

		_bool       isDiffuse_R = {};
		_bool       isDiffuse_G = {};
		_bool       isDiffuse_B = {};
		_bool       isBlurColor = {};

		_bool       isBlurDissolve = {};
		_bool       isNomalMap = {};
		_bool       isNoDissolveSmoothStep = {};
		_bool       isPadding3 = {};
		
		_float4     vPadding5 = {};

		_bool		isRimLight = {};
		_bool		isMask_G = {};
		_bool		isMask_B = {};
		_bool		isDissolve_B = {};

	}EFFECT_INFO;

	typedef struct tagPreEffectInfo
	{
		_float4 vColor = { 0.f ,0.f ,0.f ,1.f };

		_float2 vDiffuseUVGainAmount = {};
		_float2 vMaskingUVGainAmount = {};
		_float2 vNoiseUVGainAmount = {};

		_float2 vDistortionTime = {};
		_float2 vMaskDistortionUVGainAmount = {};

		_float2 vUVCutting = { 1.f ,1.f };
		_float2 vUVMaskCutting = { 1.f ,1.f };

		_int    iMaskMoveLerpOption = { 0 };
		_int    iDiffuseMoveLerpOption = { 0 };
		_int    iNoiseMoveLerpOption = { 0 };

		_int    iMaskDistortionMoveLerpOption = { 0 };
		_int    iDiffuseDistortionMoveLerpOption = { 0 };

		_int	iBlurWeight = { 0 };


		_bool	isDiffuse = {};
		_bool	isMasking = {};
		_bool	isDissolve = {};
		_bool	isNoise = {};
		_bool   isEmissive = {};
		_bool   isDistortion = {};
		_bool	isDiffuseUVMove = {};
		_bool   isNoiseUVMove = {};
		_bool	isMaskUVMove = {};
		_bool   isBlur = {};
		_bool   isBillboard = {};

		_float4 vEmissive = { 0.f ,0.f ,0.f ,0.f };
		_float  fDiffuseAlpha = { 1.f };
		_float  fBlurIntensity = {};
		_float  fNoiseDistortionIntensity = {};
		_float  fEmissiveStrength = { 0.f };

		LIGHT_DESC	LightDesc = {};
		RENDER		eRenderOrder = { RENDER::EFFECT };

		_bool   isReverseDissolve = {};

		EFFECT_TYPE eEffectType = { EFFECT_TYPE::EFFECT };

		_bool   isEmissiveDissolve = { false };

		_bool   isMaskClampSample = { false };
		_bool   isNoiseColor = { false };
		_bool   isNoiseAlpha = { false };
		_bool	isNomalDissolve = {};

		_float fSoftenExp = { 1.31429f };
		_float fSoftStrength = {};
		_float fCoreBoost = {};
		_float fRadius = {};

		_float fSoftMaskEdge = {};
		_float fSoftMask = {};

		_bool   isEmissiveDissolveReverse = { false };
		_bool   isOnlyBlur = { false };

		SHADER_PASS_INSTANCE_MODEL eShaderPass = { SHADER_PASS_INSTANCE_MODEL::NON_NOMALMAP };

		_bool       isBlurNoEmissive = {};
		_bool	    isTexBlur = {};
		_bool       isBlurDissolve = {};
		_bool       isBlurReverseDissolve = {};

		_float      fBluringStrength = { 0.01f };

		_bool       isBloom = {};
		_bool       isBloomDissolve = {};
		_bool       isBloomReverseDissolve = {};
		_float      fBloomStrength = {};
		BLOOM_TYPE  eBloomType = {};


		_float		fDissolveDelay = {};
		_float		fReverseDissolveDelay = {};
		_float2		vDissolveUVGainAmount = {};
		_bool		isDissolveMove = {};

		_float3		vDissolveValue = {};


		_float2		vPadding1 = {};

		_float      fLightDeley = {};
		_float		fLightIntensity = {};
		_float		isLightTime = { 0.f };
		_bool		isLightDissolve = {};

		_bool		isPadding1 = {};

	}PRE_EFFECT_INFO;

protected:
	CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectObject(const CEffectObject& rhs);
	virtual ~CEffectObject() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT Render_Blur() override;
	virtual HRESULT Render_Bloom() override;
public:
	HRESULT Load(const _char* pFilePath, LEVEL eLevel);
	HRESULT Load();
	void    FollowParants(const _float4x4* pParantsMat);
public:
#ifdef _DEBUG
		HRESULT LoadPre(const _char* pFilePath, LEVEL eLevel);
#endif // DEBUG
	EFFECT_INFO* Get_Effect_Info() { return &m_EffectInfo; }

protected:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

protected:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pNoise_TextureCom = { nullptr };
	CTexture* m_pMasking_TextureCom = { nullptr };
	CTexture* m_pDissolve_TextureCom = { nullptr };
	CTexture* m_pEmissive_TextureCom = { nullptr };
	CTexture* m_pDistortion_TextureCom = { nullptr };
	CTexture* m_pNormal_TextureCom = { nullptr };
	CTexture* m_pSurface_TextureCom = { nullptr };


	CShader* m_pShaderCom = { nullptr };
	CLight*  m_pLightCom = { nullptr };

	CInstance_Model* m_pInstance_ModelCom = { nullptr };

	const _float4x4* m_pPerentMatrix = { nullptr };
protected:
	EFFECT_INFO m_EffectInfo = {};
	_string		m_strDiffuseName = {};
	_string		m_strNoiseName = {};
	_string		m_strMaskingName = {};
	_string		m_strDissolveName = {};
	_string		m_strModelName = {};
	_string     m_strEmissiveName = {};
	_string     m_strDistortionName = {};
	_string     m_strNomalMapName = {};

	_string		m_strPath = {};
	_string     m_strName = {};

public:
	virtual void Free() override;
};

NS_END
