#pragma once

#include "Client_Define.h"
#include "EffectObject.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CTrail;
NS_END

NS_BEGIN(Client)

class CTrailObject final : public CPartObject
{
public:

private:
	CTrailObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrailObject(const CTrailObject& rhs);
	virtual ~CTrailObject() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	void    Trail_Update(_fmatrix WorldMat, _float fTimeDelta);
	void    Rope_Trail_Update(_fmatrix WorldMat, _fmatrix EndWorldMat, _float fTimeDelta);

	HRESULT Load_Trail(CEffect_Container::TRAIL_SAVE_INFO Trail_Save_Info, LEVEL eLevel);
	HRESULT Load_Trail(const _char* pPath, LEVEL eLevel);

	void   SetDissolve(_bool isDissolve) { m_TrailInfo.isDissolve = true; }

private:
	HRESULT Bind_ShaderResources() override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Blur() override;
	virtual HRESULT Render_Bloom() override;

public:
	static CTrailObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pNoise_TextureCom = { nullptr };
	CTexture* m_pMasking_TextureCom = { nullptr };
	CTexture* m_pDistortion_TextureCom = { nullptr };

	CShader* m_pShaderCom = { nullptr };
	CTrail* m_pTrailCom = { nullptr };

	_wstring		m_strTrailDiffuseName = {};
	_wstring		m_strTrailNoiseName = {};
	_wstring		m_strTrailMaskingName = {};
	_wstring		m_strTrailDistortionName = {};

	_string			m_strPath = {};
	_string			m_strName = {};

	TRAIL_INFO		m_TrailInfo = {};
};

NS_END
