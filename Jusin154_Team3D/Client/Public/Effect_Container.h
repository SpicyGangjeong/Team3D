#pragma once

#include "Client_Define.h"
#include "ContainerObject.h"
#include "Trail.h"

NS_BEGIN(Engine)
class CInstance_Model;
class CTrail;
NS_END

NS_BEGIN(Client)

class CEffect_Container abstract : public CContainerObject
{
public:
	typedef struct tagEffectSaveInfo
	{
		EFFECT_INFO EffectInfo = {};
		LIGHT_DESC  LightInfo = {};

		_wstring    wstrDiffuseName = {};
		_wstring    wstrNoiseName = {};
		_wstring    wstrMaskingName = {};
		_wstring    wstrDissolveName = {};
		_wstring    wstrEmissiveName = {};
		_wstring    wstrDistortionName = {};
		_wstring    wstrNomalName = {};
		_wstring    wstrModelName = {};

		_wstring    wstrEffectName = {};

		CInstance_Model::INSTANCE_DESC InstanceModelInfo = {};
	}EFFECT_SAVE_INFO;

	typedef struct tagTrailSaveInfo
	{
		_wstring    wstrDiffuseName = {};
		_wstring    wstrNoiseName = {};
		_wstring    wstrMaskingName = {};
		_wstring    wstrDistortionName = {};

		_wstring    wstrTrailName = {};

		CTrail::TRAIL_DESC TrailComponentDesc = {};
		TRAIL_INFO		   TrailDesc = {};
	}TRAIL_SAVE_INFO;

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
	virtual	HRESULT	Pre_Setting(CGameObject* pObject, void* pArg = nullptr);
	void			Setting_Pos(_fvector vPos);
	HRESULT         Load_Package(const _char* pPath);
	HRESULT         Load_Data(const _char* pPath);
	HRESULT         Create_Effect();
	HRESULT         Load_Trail_Data(const _char* pPath);
	_uint			Get_SkillType() const { return m_iSkillType; }
	void			Reset_Light();

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT         Ready_Child();
	HRESULT			Bind_ShaderResources() override;

	void				Update_Event(_float fTimeDelta);
	HRESULT				Reset_EffectParts();
	_int				CollisionCheck();
	ON_COLLISION_INFO	SweepTarget(_vector StartPos, _vector EndPos, _float fRadius, _bool isTerrainCollision = false);
	ON_COLLISION_INFO	MonsterSweepTarget(_vector StartPos, _vector EndPos, _float fRadius, _bool isTerrainCollision = false);
	ON_COLLISION_INFO	MonsterRayCast(_vector StartPos, _vector vDir, _float fLength, _uint iMaxHitCapacity);

protected:
	_float4							m_vStartPos = {};
	_float4							m_vEndPos = {};
	PSX::PxSweepBufferN<12>			m_Hitbuffer = {};
protected:
	_wstring						m_wstrEffectName = {};
	_bool							m_bHit = { false };
	_bool							m_isLoop = {};
	_bool							m_isStop = {};

	_float							m_fAccTime = {};
	_float							m_fPreAccTime = {};
	_float							m_fDuration = {};
	_float							m_fDelay = {};
	_bool							m_isDelayed = { false };
	_bool							m_isCollisionEnter = { false };
	_uint							m_iHitCount = {};

	map<_float, function<void()>>	m_Events = {};
	_uint							m_iSkillType = ENUM_CLASS(SKILL_TYPE::END);
	_bool							m_bHasDamage = { false };
	LOCKON_INFO						m_Info = {};
private:
	vector<EFFECT_SAVE_INFO>        m_EffectsInfo = {};
	vector<TRAIL_SAVE_INFO>			m_TrailsInfo = {};
public:
	virtual void Free() override;
};

NS_END
