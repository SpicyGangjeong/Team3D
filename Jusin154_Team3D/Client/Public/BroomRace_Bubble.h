#pragma once

#include "Client_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CBroomRace_Bubble final : public CEffect_Container
{
private:
	CBroomRace_Bubble(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroomRace_Bubble(const CBroomRace_Bubble& rhs);
	virtual ~CBroomRace_Bubble() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	void CollisionCheck(_fvector StartPos, _fvector EndPos, _float fRadius);

public:
	virtual	HRESULT	Pre_Setting(CGameObject* pObject, void* pArg = nullptr) override;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT         Ready_Child();
	HRESULT			Bind_ShaderResources() override;
	virtual void	OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr) override;
private:
	_wstring							  m_wstrEffectName = {};
	class CEffectParts*					  m_pPT1 = { nullptr };
	class CEffectParts*					  m_pPT2 = { nullptr };
	class CTrailObject*					  m_pAttack_Trail = { nullptr };
	const _float4x4*					  m_vMat = { nullptr };

	_bool							  m_isTrailEnd = {};
	_float4x4						  m_TrailStopMat = {};

public:
	static CBroomRace_Bubble* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
