#pragma once

#include "Editor_Define.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CInstance_Model;
class CShader;
class CTexture;
class CLight;
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

		_float2 vDiffuseNoiseUVGainAmount = {};
		_float2 vMaskNoiseUVGainAmount = {};

		_float2 vUVCutting = { 1.f ,1.f };
		_float2 vUVMaskCutting = { 1.f ,1.f };

		_int    iMaskMoveLerpOption = { 0 };
		_int    iDiffuseMoveLerpOption = { 0 };
		_int    iMaskNoiseMoveLerpOption = { 0 };
		_int    iDiffuseNoiseMoveLerpOption = { 0 };
		_int	iBlurWeight = { 0 };


		_bool	isDiffuse = {};
		_bool	isMasking = {};
		_bool	isDissolve = {};
		_bool	isNoise = {};
		_bool   isEmissive = {};
		_bool	isDiffuseUVMove = {};
		_bool	isMaskUVMove = {};
		_bool   isBlur = {};
		_bool   isBillboard = {};


		_float4 vEmissive = { 0.f ,0.f ,0.f ,0.f };
		_float  fColorOption = {};
		_float  fBlurIntensity = {};
		_float  fNoiseDistortionIntensity = {};
		_float  fEmissiveCutAlpha = {};

		LIGHT_DESC	LightDesc = {};
		RENDER		eRenderOrder = { RENDER::EFFECT };

	}EFFECT_INFO;

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

public:
	virtual void Free() override;
};

NS_END
