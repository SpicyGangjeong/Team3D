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

		_float2 vDiffuseDistortionUVGainAmount = {};
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
		_float  fEmissiveCutAlpha = {};

		LIGHT_DESC	LightDesc = {};
		RENDER		eRenderOrder = { RENDER::EFFECT };

		_bool   isReverseDissolve = {};

		EFFECT_TYPE eEffectType = { EFFECT_TYPE::EFFECT };

	}EFFECT_INFO;

	typedef struct tagPreEffectInfo
	{
		_float4 vColor = { 0.f ,0.f ,0.f ,1.f };

		_float2 vDiffuseUVGainAmount = {};
		_float2 vMaskingUVGainAmount = {};
		_float2 vNoiseUVGainAmount = {};

		_float2 vDiffuseDistortionUVGainAmount = {};
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
		_float  fEmissiveCutAlpha = {};

		LIGHT_DESC	LightDesc = {};
		RENDER		eRenderOrder = { RENDER::EFFECT };

		_bool   isReverseDissolve = {};



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
public:
	HRESULT Load(const _char* pFilePath, LEVEL eLevel);

public:
#ifdef _DEBUG
		HRESULT LoadPre(const _char* pFilePath, LEVEL eLevel);
#endif // DEBUG


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

	CShader* m_pShaderCom = { nullptr };
	CLight*  m_pLightCom = { nullptr };

	CInstance_Model* m_pInstance_ModelCom = { nullptr };


protected:
	EFFECT_INFO m_EffectInfo = {};
	_string		m_strDiffuseName = {};
	_string		m_strNoiseName = {};
	_string		m_strMaskingName = {};
	_string		m_strDissolveName = {};
	_string		m_strModelName = {};
	_string     m_strEmissiveName = {};
	_string     m_strDistortionName = {};

	_string		m_strPath = {};
	_string     m_strName = {};

public:
	virtual void Free() override;
};

NS_END
