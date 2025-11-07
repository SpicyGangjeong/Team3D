#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect final :
	public CVIBuffer
{
protected:
	CVIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect(const CVIBuffer_Rect& rhs);
	virtual ~CVIBuffer_Rect() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CVIBuffer_Rect* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END