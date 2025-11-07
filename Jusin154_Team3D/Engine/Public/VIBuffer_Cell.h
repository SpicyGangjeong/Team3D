#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cell final :
	public CVIBuffer
{
protected:
	CVIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Cell(const CVIBuffer_Cell& rhs);
	virtual ~CVIBuffer_Cell() = default;

private:
	virtual HRESULT Initialize_Prototype(const _float3* pPoints);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Change_PrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY eType = D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);

public:
	static CVIBuffer_Cell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END