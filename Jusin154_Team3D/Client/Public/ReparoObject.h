#pragma once

#include "Client_Define.h"
#include "Monster.h"

NS_BEGIN(Client)

class CReparoObject final : public CMonster
{
	// 되돌리기 전 Before
	enum class REPARO_OBJECT_STATE { SLEEP, BEFORE, PLAYANIM, AFTER, END };
public:
	typedef struct tagReparoObjectDesc
	{
		_uint		iModelID = {};
		_float		fRadius = {};
		_float3		vPosition = {};
		_float3		vRotation = {};
	}REPARO_OBJECT_DESC;
	
private:
	CReparoObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CReparoObject(const CReparoObject& Prototype);
	virtual ~CReparoObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_bool IsRepairable(_bool isAwake);
private:
	REPARO_OBJECT_STATE		m_ePreState = { REPARO_OBJECT_STATE::END };
	REPARO_OBJECT_STATE		m_eCurState = { REPARO_OBJECT_STATE::END };

	_uint					m_iModelID = {};
	_float					m_fRadius = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Render_OutLine();

	void	Change_State();

public:
	static CReparoObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
