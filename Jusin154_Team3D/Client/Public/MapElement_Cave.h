#pragma once

#include "Client_Define.h"
#include "MapElement.h"

NS_BEGIN(Client)

class CMapElement_Cave : public CMapElement
{
private:
	CMapElement_Cave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapElement_Cave(const CMapElement_Cave& rhs);
	virtual ~CMapElement_Cave() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool							m_bReadyToCreatePhysX = { false };
	_bool							m_bConverted = { false };
	_float							m_fRadius = {};
	_float3							m_vWorldCenterPosition = {};

	class CTriggerBox*				m_pTriggerBox = { nullptr };

	vector<CRigidBody_Static*>		m_RigidBodies;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;
	
	HRESULT			Ready_TriggerBox();
	void			ReadyForPhysX();
	void			ConvertToPhysX();

	void			Enter_Cave();
public:
	static CMapElement_Cave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END

