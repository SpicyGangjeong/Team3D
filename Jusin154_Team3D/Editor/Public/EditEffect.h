#pragma once

#include "Editor_Define.h"
#include "EffectObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;

NS_END

NS_BEGIN(Editor)

class CEditEffect final : public CEffectObject
{
private:
	CEditEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEditEffect(const CEditEffect& rhs);
	virtual ~CEditEffect() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override; 
	virtual _vector Get_WorldPostion() override;

public:
	void	Reference_Mat_For_EditEffect();
	HRESULT Save_Effect(const _char* pPath);
	HRESULT Save_Path(HANDLE hFile);
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;

private:
	_char			m_szBuffer[MAX_PATH] = {};
	_string         m_strBoneName = {};
	_int			m_iSelectTextureNum = {};
	vector<_float>	m_ValueVector = {};
	_float			m_fInputValue = {};
	
	_bool		    m_isWandPos = {};
	_bool			m_isCamPos = {};

	_float4			m_vCamOffset = {};


public:
	static CEditEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
