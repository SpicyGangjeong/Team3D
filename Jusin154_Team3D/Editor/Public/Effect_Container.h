#pragma once

#include "Editor_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Editor)

class CEffect_Container abstract : public CContainerObject
{
protected:
	CEffect_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Container(const CEffect_Container& rhs);
	virtual ~CEffect_Container() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr) override;
public:
	HRESULT			Load_Directory(const _char* pPath);
	HRESULT         Load_Package(const _char* pPath);
	virtual	HRESULT	Pre_Setting(CGameObject* pObject, void* pArg = nullptr);
	void			Reset_Light();
	_uint			Get_SkillType() const { return m_iSkillType; }
protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT         Ready_Child();
	HRESULT			Bind_ShaderResources() override;
	HRESULT			Reset_EditEffect(); 

	void			Update_Event(_float fTimeDelta);
	_int            CollisionCheck();
	ON_COLLISION_INFO	SweepTarget(_vector StartPos, _vector EndPos, _float fRadius, _bool isTerrainCollision = false);
protected:

	_uint							m_iSkillType = ENUM_CLASS(SKILL_TYPE::END);
	_float4							m_vStartPos = {};
	_float4							m_vEndPos = {};
	PSX::PxSweepBufferN<12>			m_Hitbuffer = {};
	_bool							m_bHit = { false };
protected:
	_wstring						m_wstrEffectName = {};

	_bool							m_isLoop = {};

	_float							m_fAccTime = {};
	_float							m_fPreAccTime = {};
	_float							m_fDuration = {};
	_float							m_fDelay = {};
	_bool							m_isDelayed = { false };
	_bool							m_isCollisionEnter = { false };

	map<_float, function<void()>>	m_Events = {};
	LOCKON_INFO						m_Info = {};
public:
	virtual void Free() override;
};

NS_END
