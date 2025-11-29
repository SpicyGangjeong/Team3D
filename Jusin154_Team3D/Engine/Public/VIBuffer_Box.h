#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Box final :
	public CVIBuffer
{
private:
	CVIBuffer_Box(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Box(const CVIBuffer_Box& rhs);
	virtual ~CVIBuffer_Box() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
#ifdef EDITOR_PROJECT
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Update(_float3* pVertices);

#else
private:
	virtual HRESULT Initialize(void* pArg) override;
#endif // EDITOR_PROJECT
	

public:
	static CVIBuffer_Box* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CVIBuffer_Box* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
