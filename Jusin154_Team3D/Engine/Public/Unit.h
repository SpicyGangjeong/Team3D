#pragma once

#include "Engine_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CFSM;

class ENGINE_DLL CUnit abstract : public CGameObject
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

public:
	pair<_uint, _bool> Get_AnimInfo(_uint iIndex) { return m_Animation[iIndex]; }

	virtual _bool IsWalking() { return false; }
	virtual _bool IsDodge() { return false; }
	virtual _bool IsSprint() { return false; }
	virtual _bool IsJump() { return false; }

protected:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CFSM*		m_pFSM = { nullptr };
	_wstring	m_strModelPrototypeTag;
	_bool		m_bRimLight = { true };
	_float		m_fRimLightPower = { 3.2f };
	_float		m_fRimLightStrength = { 3.04f };
	_float3		m_vRimLightColor = { 69.f / 255.f, 5.f / 255.f, 10.f / 255.f };

	vector<pair<_uint,_bool>> m_Animation;

private:
	virtual void Add_FSM() {};
	virtual void Set_Anim() {};

protected:
	HRESULT Ready_Components(void*pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)PURE;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
