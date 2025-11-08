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

protected:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

protected:
	CTexture*			m_pTextureCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CInstance_Model*	m_pInstance_ModelCom = { nullptr };


public:
	virtual void Free() override;
};

NS_END