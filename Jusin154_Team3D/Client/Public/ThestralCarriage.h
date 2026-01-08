#pragma once

#include "Client_Define.h"
#include "Unit.h"
NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CThestralCarriage final : public CGameObject
{ 
private:
	CThestralCarriage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CThestralCarriage(const CThestralCarriage& Prototype);
	virtual ~CThestralCarriage() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CInfoInstance* m_pInfoInstance = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
public:
	static CThestralCarriage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
