#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Editor)

class CUnified final : public CGameObject
{
private:
	CUnified(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUnified(const CUnified& rhs);
	virtual ~CUnified() = default;

public:
	_uint Get_LodLevel() const { return m_iMaxLodLevel; }

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool				m_bSelected = { false };
	_bool				m_isVisible = { true };

	_uint				m_iMaxLodLevel = {};
	_uint				m_iLodIndex = {};
	_float				m_fUsingSurfaceParams = {};

	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
	_float4				m_vPosition = {};

	_float4x4			m_ProjMatrix = {};
	_float4x4			m_ViewMatrix = {};
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CUnified* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
