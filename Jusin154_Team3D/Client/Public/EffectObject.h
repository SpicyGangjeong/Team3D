#pragma once

#include "Client_Define.h"
#include "PartObject.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
class CInstance_Model;
class CShader;
class CTexture;
class CLight;
class CTrail;
NS_END

NS_BEGIN(Client)

class CEffectObject abstract : public CPartObject
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
	virtual HRESULT Render_Bloom() override;
public:
	HRESULT Load(CEffect_Container::EFFECT_SAVE_INFO EffectSaveInfo, LEVEL eLevel);
	HRESULT Load(const _char* pFilePath, LEVEL eLevel);
	void    Disable_Light();
	void    Add_Light();
	void    FollowParents(const _float4x4* pParentsMat, const _float4x4* pOffsetMat = nullptr);
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

	const _float4x4* m_pParentMatrix = { nullptr };

	_float4x4    m_ScreenViewMatrix = {};
	_float4x4    m_OrthographicMatrix = {};
	_float4x4    m_FinalParentMatrix;

protected:
	EFFECT_INFO  m_EffectInfo = {};
	_wstring	 m_strDiffuseName = {};
	_wstring	 m_strNoiseName = {};
	_wstring	 m_strMaskingName = {};
	_wstring	 m_strDissolveName = {};
	_wstring	 m_strModelName = {};
	_wstring     m_strEmissiveName = {};
	_wstring     m_strDistortionName = {};
	_wstring     m_strNomalMapName = {};

public:
	virtual void Free() override;
};

NS_END
