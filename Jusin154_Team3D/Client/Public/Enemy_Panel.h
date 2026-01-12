#pragma once

#include "Client_Define.h"
#include "PanelObject.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)


class CEnemy_Panel final : public CPanelObject
{
	struct Pos 
	{
		CGameObject*	Prop;
		_float3			TargetPos{};
		_float2			UIScreenPos{};
		_float4			Position{};
	};
private:
	CEnemy_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy_Panel(const CEnemy_Panel& rhs);
	virtual ~CEnemy_Panel() = default;

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
	void Update_Target();
	void RanRokPropCreate(_fvector Position);

private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CInfoInstance* m_pInfoInstance = { nullptr };
	CGameObject* m_pEnemy_HpBar = { nullptr };
	CGameObject* m_pEnemy_Info = { nullptr };
	CGameObject* m_pBoss_HpBar = { nullptr };
	CGameObject* m_pRanrokProp = { nullptr };

	vector<Pos> m_RanrokProps;
	deque<Pos> m_pCurrentRanrokProps;
	Pos		m_Pos{};

public:
	static CEnemy_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
