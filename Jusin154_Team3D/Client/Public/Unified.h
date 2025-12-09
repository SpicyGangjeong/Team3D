#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CUnified final : public CGameObject
{
private:
	CUnified(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUnified(const CUnified& rhs);
	virtual ~CUnified() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool				m_isVisible = { true };
	_uint				m_iNumMesh = {};
	_float				m_fUsingSurfaceParams = {};

	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
	_float4             m_vSurfaceColor = {};
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CUnified* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
