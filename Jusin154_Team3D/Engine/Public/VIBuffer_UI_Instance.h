#pragma once

#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_UI_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagUIInstanceDesc final : public CVIBuffer_Instance::INSTANCE_DESC
	{
	}UI_INSTANCE_DESC;
	typedef struct tagUIAtlasDesc
	{
		_float2 fUVStart{};
		_float2 fUVEnd{};
	}UI_ATLAS_DESC;
private:
	CVIBuffer_UI_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_UI_Instance(const CVIBuffer_UI_Instance& Prototype);
	virtual ~CVIBuffer_UI_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Set_Index_Position(_uint iIndex, _float fX, _float fY);
	void Set_Index_Size(_uint iIndex, _float fSizeX, _float fSizeY);
	void Set_Pos(_float fX, _float fY, _float OffSetX, _float OffSetY, _uint iCols);
	void Set_Size(_float fSizeX, _float fSizeY);
	void Set_SizeX(_float fSizeX);
	void Set_SizeY(_float fSizeY);
	void Set_Cloned(_bool isCloned) { m_isCloned = isCloned; }
	void Set_ImageUV(UI_ATLAS_DESC* AtlasUV = nullptr);

private:
	VTX_INSTANCE_UI*	m_pInstanceVertices = { nullptr };
	_bool				m_isCloned = { false };
	_float2*			m_fSize{ nullptr };
	_float2*			m_fPosition{ nullptr };
	_float				m_fAddPosition{};


public:
	static CVIBuffer_UI_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
