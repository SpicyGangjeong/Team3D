#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CMapElement abstract : public CGameObject
{
public:
	typedef struct tagMapElement_Desc {
		_bool					bVisible = {};
		_uint					iRenderType{};
		CTransform*				pParentTransform = { nullptr };
		_float3					vScale;
		_float3					vRotation;
		_float3					vPosition;
		_uint					iMaxLodLevel{};
		vector<_wstring>		ModelPrototypeTags;
		vector<_uint>*			pModelPathIndices = { nullptr };
	}MAPELEMENT_DESC;
	
protected:
	CMapElement(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapElement(const CMapElement& rhs);
	virtual ~CMapElement() = default;

public:
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;



protected:
	_uint				m_iMaxLodLevel = {};
	_uint				m_iLodIndex = {};

	CShader*			m_pShaderCom = { nullptr };
	vector<CModel*>		m_pModelComs;
	vector<_wstring>	m_ModelPrototypeTags;

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) PURE;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
