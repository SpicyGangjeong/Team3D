#pragma once

#include "Client_Define.h"
#include "GameInstance.h"
#include "GameObject.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CTriggerBox final : public CGameObject
{
public:
	typedef struct tagTriggerDesc {
		_float4 vPosition_Radius = { 0.f, 0.f, 0.f, 1.f };
	}TRIGGERBOX_DESC;
private:
	CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTriggerBox(const CTriggerBox& rhs);
	virtual ~CTriggerBox() = default;

public:
	HRESULT TryScanArea(_float fTimeDelta);

	virtual HRESULT Render()override;
	HRESULT Bind_ShaderResources() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

private:
	HRESULT Initialize(TRIGGERBOX_DESC*);
	HRESULT Scan();
	HRESULT CheckPlayerHit(PSX::PxActor* pActor, PSX::PxShape* pShape);


private:
	_float2 m_vScanTimer = { 0.f, 0.8f };
#ifdef _DEBUG
	_bool m_bRender = false;
	unique_ptr<GeometricPrimitive> m_pSubShape = { nullptr };
	unique_ptr<PrimitiveBatch<VertexPositionColor>> m_Batch;
#endif // _DEBUG

public:
	static CTriggerBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TRIGGERBOX_DESC* Desc);
	virtual void Free() override;

};

NS_END
