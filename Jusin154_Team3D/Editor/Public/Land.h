#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Editor)

class CLand final : public CGameObject
{
public:
	typedef struct tagLandDesc
	{
		_bool	bEdit;
		_float3 vPosition;
		_float3 vScale;
		_wstring strModelComTag;
	}LAND_DESC;
private:
	CLand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLand(const CLand& rhs);
	virtual ~CLand() = default;

public:
	_uint Get_LodLevel() const { return m_iMaxLodLevel; }

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool				m_bSelected = { false };
	_bool				m_bEdit = {false};
	_uint				m_iMaxLodLevel = {};
	_uint				m_iLodIndex = {};

	_float				m_fUsingSurfaceParams = {};

	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };

	_float4				m_vPosition = {};
	_float3				m_vScale = {};
	_float3				m_vRotation = {};


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CLand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
