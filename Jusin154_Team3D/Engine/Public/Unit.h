#pragma once

#include "Engine_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUnit abstract : public CContainerObject
{
public:
	typedef struct tagObjectDesc
	{
		const _tchar* pModelPrototypeTag;
	}PARTS_OBJECT_DESC;
protected:
	CUnit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUnit(const CUnit& Prototype);
	virtual ~CUnit() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	_wstring& Get_PrototypeTag() { return m_strModelPrototypeTag; }
#ifdef _DEBUG
	void Load_KeyFrame();
#endif // _DEBUG

public:

#pragma region STATE
	pair<_uint, _bool> Get_AnimInfo(_uint iIndex) { return m_Animation[iIndex]; }
	virtual _bool Check(FSMSTATE::ESTATE state) { return false; }

	void Reset_LightCombo() { m_iLightCombo = 0; }
	_uint Next_LightCombo() { return ++m_iLightCombo; }
	void Set_LightCombo(_uint LightCombo) { m_iLightCombo = LightCombo; }
	_float Get_KeyFrame(_string FrameName);
	_bool IsCurrentKeyFrame(_string FrameName);

	virtual void Reset_Sprint() {};
	virtual void Reset_Walk() {};
#pragma endregion

protected:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CFSM*			m_pFSM = { nullptr };
	_wstring		m_strModelPrototypeTag;
	_bool			m_bRimLight = { true };
	_float			m_fRimLightPower = { 3.2f };
	_float			m_fRimLightStrength = { 3.04f };
	_float3			m_vRimLightColor = { 69.f / 255.f, 5.f / 255.f, 10.f / 255.f };
	_uint			m_iLightCombo = { 0 };

	map<_string, _float> m_KeyFrames;

	vector<pair<_uint, _bool>> m_Animation;

	unordered_map<size_t, CState*> m_States = { };
	size_t m_iStateMask = { 0 };

	STATEANIM::ESTATE m_eSpell = { STATEANIM::END };

private:
	virtual void Add_FSM() {};
	virtual void Set_FSM() {};
	virtual void Set_Anim() {};

protected:
	HRESULT Ready_Components(void*pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)PURE;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
