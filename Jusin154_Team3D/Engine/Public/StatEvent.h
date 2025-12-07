#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CStatEvent final : public CComponent
{
private:
	CStatEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatEvent(const CStatEvent& Prototype);
	virtual ~CStatEvent() = default;
private:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void AddEvent(const function<void()> _Equipfunc, const function<void()> _UnEquipfunc);
	_uint PopEvent(_uint iIndex);


	void Equip();
	void UnEquip();
	void Clear();

	void Push_Order(_uint _iId);

public:
	static CStatEvent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG



private:
	vector<function<void()>> m_EquipEventVector = {};
	vector<function<void()>> m_UnEquipEventVector = {};
	vector<_uint>			 m_ActiveAbillityIDVec = {};

};

NS_END
