#pragma once

#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_UI_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagUIInstanceDesc final : public CVIBuffer_Instance::INSTANCE_DESC
	{

	}UI_INSTANCE_DESC;
private:
	CVIBuffer_UI_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_UI_Instance(const CVIBuffer_UI_Instance& Prototype);
	virtual ~CVIBuffer_UI_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Set_Pos(_float fX, _float fY, _float OwnerfX, _float OwnerfY, _float Spacing);
	virtual void Set_Size(_float fSizeX, _float fSizeY);

private:
	VTX_INSTANCE_UI* m_pInstanceVertices = { nullptr };

public:
	static CVIBuffer_UI_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
