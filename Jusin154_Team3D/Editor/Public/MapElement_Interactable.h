#pragma once

#include "Editor_Define.h"
#include "MapElement.h"

NS_BEGIN(Editor)

class CMapElement_Interactable : public CMapElement
{
private:
	CMapElement_Interactable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapElement_Interactable(const CMapElement_Interactable& rhs);
	virtual ~CMapElement_Interactable() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint		m_iSaveIndex = {}; // 원래 위치 받아올때 필요한 인덱스

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CMapElement_Interactable* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END