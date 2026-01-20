#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CUnified final : public CGameObject
{
public:
	typedef struct tagUnified_Desc
	{
		_float				fLodSwitchDistnace;
		_float4				vUnifiedCenterPos;
		_float3				vPosition;
		_float3				vRotation;
		_float3				vScale;
		_wstring			srtLayerTag;
		vector<_wstring>	srtModelPrototypeTags;
	}UNIFIED_DESC;
private:
	CUnified(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUnified(const CUnified& rhs);
	virtual ~CUnified() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool				m_isVisible = { false };
	_bool				m_isLayerEnable = { true };

	_uint				m_iMaxLodLevel = {};
	_uint				m_iLodIndex = {};
	_float				m_fUsingSurfaceParams = {};
	_float				m_fLodSwitchDistnace = {};

	CShader*			m_pShaderCom = { nullptr };

	_float4				m_vUnifiedCenterPos = {};
	_float4             m_vSurfaceColor = {};

	_wstring			m_srtLayerTag;
	vector<CModel*>		m_pModelComs;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CUnified* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
