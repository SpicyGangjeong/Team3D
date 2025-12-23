#pragma once

#include "Client_Define.h"
#include "Unit.h"

NS_BEGIN(Client)

class CNPC_EleazarFig final : public CUnit
{
protected:
	CNPC_EleazarFig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_EleazarFig(const CNPC_EleazarFig& Prototype);
	virtual ~CNPC_EleazarFig() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(SHADOW eType) override;
protected:
	CInfoInstance* m_pInfoInstance = { nullptr };

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources() override;

public:
	static CNPC_EleazarFig* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr);
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG
};

NS_END
