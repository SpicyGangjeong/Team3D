#pragma once

#include "Editor_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Editor)

class CBlink final : public CEffect_Container
{
private:
	CBlink(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlink(const CBlink& rhs);
	virtual ~CBlink() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

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
	_wstring					m_wstrEffectName = {};

	_bool						m_isParticleEnd = {};
	_bool						m_isTrailEnd = {};
	_float4x4					m_TrailStopMat = {};

	_float						m_fMoveAccTime = {};
	_float						m_fAmountMove = {0.3f};
	_float						m_fMoveSpeed = {5.f};

	_float						m_fSizeAccTime = {};
	_float						m_fAmountSize = {};
	_float						m_fSpeed = {};

	class CEditEffect*			m_pBlink_Swirl = { nullptr };
	class CEditEffect*			m_pBlink_Circle = { nullptr };
	class CEditEffect*			m_pBlink_Circle2 = { nullptr };
	class CEditEffect*			m_pBlink_Distortion = { nullptr };

	vector<class CTrailObject*> m_Blink_Trails = {};

	_float      m_fLength = { 5.f };
	_string		m_BoneNames[10] = {};


public:
	static CBlink* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
