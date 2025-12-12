#pragma once

#include "Client_Define.h"
#include "MapElement.h"

NS_BEGIN(Client)

class CMapElement_Door final : public CMapElement
{
public:
	typedef struct tagElement_Interactable_Desc : MAPELEMENT_DESC
	{
		_uint		iSubKind = {};
		_float3		vBoxLocalPosition;
		_float3		vBoxSize;
	}ELEMENT_DOOR_DESC;

private:
	CMapElement_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapElement_Door(const CMapElement_Door& rhs);
	virtual ~CMapElement_Door() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:

	CRigidBody* m_pRigidBody = { nullptr };
	PSX::PxRigidDynamic* m_pActor = { nullptr };

#ifdef _DEBUG
	_bool m_bUseSelectColor = { true };

	_float m_fPower = {};
	_float m_fMass = {};
	_float3 m_vBoxSize = {};
#endif // _DEBUG


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CMapElement_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
