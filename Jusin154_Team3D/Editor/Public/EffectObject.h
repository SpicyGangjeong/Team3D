#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CInstance_Model;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Editor)

class CEffectObject abstract : public CGameObject
{
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
protected:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

protected:
	CTexture*			m_pDiffuse_TextureCom = { nullptr };
	CTexture*			m_pNoise_TextureCom = { nullptr };
	CTexture*			m_pMasking_TextureCom = { nullptr };
	CTexture*			m_pDissolve_TextureCom = { nullptr };

	CShader*			m_pShaderCom = { nullptr };
	CInstance_Model*	m_pInstance_ModelCom = { nullptr };

protected:
	_float4 m_vColor = { 0.f ,0.f ,0.f ,1.f };

	_float2 m_vDiffuseUVGainAmount = {};
	_float2 m_vMaskingUVGainAmount = {};

	_float2 m_vDiffuseNoiseUVGainAmount = {};
	_float2 m_vMaskNoiseUVGainAmount = {};

	_float2 m_vUVCutting = {1.f ,1.f};
			
	_bool	m_isDiffuse = {};
	_bool	m_isMasking = {};
	_bool	m_isDissolve = {};
	_bool	m_isNoise = {};
	_bool	m_isDiffuseUVMove = {};
	_bool	m_isMaskUVMove = {};
	_bool   m_isBlur = {};

	_float4 m_vEmissive = { 0.f ,0.f ,0.f ,0.f};
	_float  m_fColorOption = {};
	_float  m_fBlurIntensity = {};
	_float  m_fNoiseDistortionIntensity = {};
	_float  m_fEmissiveCutAlpha = {};


	RENDER m_eRenderOrder = {RENDER::EFFECT};

public:
	virtual void Free() override;
};

NS_END