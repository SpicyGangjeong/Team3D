#pragma once

#include "Client_Define.h"
#include "PanelObject.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)

class CMission_Panel final : public CPanelObject
{
private:
	CMission_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMission_Panel(const CMission_Panel& rhs);
	virtual ~CMission_Panel() = default;

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
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CGameObject* m_pMissionBanner_Border = { nullptr };
	CGameObject* m_pMissionBanner_Key = { nullptr };
	CGameObject* m_pMission_KeyHold = { nullptr };
	CGameObject* m_pMission_Key = { nullptr };
	CGameObject* m_pActive_Icon = { nullptr };

public:
	static CMission_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
