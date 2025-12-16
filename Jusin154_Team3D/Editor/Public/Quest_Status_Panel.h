#pragma once

#include "Editor_Define.h"
#include "PanelObject.h"

NS_BEGIN(Editor)

class CQuest_Status_Panel final : public CPanelObject
{
private:
	CQuest_Status_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuest_Status_Panel(const CQuest_Status_Panel& rhs);
	virtual ~CQuest_Status_Panel() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Ready_Element(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	void Set_Status();
	void Slot_Hover(_int Index);

private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CGameObject* m_pQuest_Status1 = { nullptr };
	CGameObject* m_pQuest_Status2 = { nullptr };
	CGameObject* m_pQuest_Status3 = { nullptr };

	_bool	m_bClick[3] = { false };

public:
	static CQuest_Status_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
