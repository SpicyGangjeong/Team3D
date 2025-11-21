#pragma once

#include "Editor_Define.h"
#include "EffectObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CTrail;
NS_END

NS_BEGIN(Editor)

class CTrailObject final : public CPartObject
{
private:
	CTrailObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrailObject(const CTrailObject& rhs);
	virtual ~CTrailObject() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
#ifdef _DEBUG
	HRESULT Save_Path(HANDLE hFile);
	HRESULT Save_Trail(const _char* pPath);
#endif
	HRESULT Load_Trail(const _char* pPath, LEVEL eLevel);
private:
	HRESULT Bind_ShaderResources() override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Render() override;

public:
	static CTrailObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;

private:
	CTexture*	m_pDiffuse_TextureCom = { nullptr };
	CTexture*	m_pNoise_TextureCom = { nullptr };
	CTexture*	m_pMasking_TextureCom = { nullptr };

	CShader*	m_pShaderCom = { nullptr };
	CTrail*		m_pTrailCom = { nullptr };

	_string		m_strTrailDiffuseName = {};
	_string		m_strTrailNoiseName = {};
	_string		m_strTrailMaskingName = {};

	_string		m_strPath = {};
	_string     m_strName = {};

	_bool		m_isDiffuse = {};
	_bool       m_isNoise = {};
	_bool       m_isMask = {};

	EFFECT_TYPE m_eEffectType = { EFFECT_TYPE::TRAIL };
};

NS_END
