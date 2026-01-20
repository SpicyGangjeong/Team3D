#pragma once

#include "Editor_Define.h"
#include "MapElement.h"

NS_BEGIN(Editor)

class CMapElement_Balloon : public CMapElement
{
public:
	typedef struct tagMapElemntBalloonDesc
	{
		_bool		isFloating;
		_uint		iDiffuseIndex;
		_float3		vPosition;
		_float3		vRotation;
		_float3		vScale;
	}BALLOON_DESC;
private:
	CMapElement_Balloon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapElement_Balloon(const CMapElement_Balloon& rhs);
	virtual ~CMapElement_Balloon() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*		m_pDiffuseTextureCom = { nullptr };
	CTexture*		m_pNormalTextureCom = { nullptr };
	CTexture*		m_pMROTextureCom = { nullptr };

	_bool			m_isFloating = {};
	_bool			m_isRotate = {};
	_uint			m_iDiffuseIndex = {};

	_float			m_fTimeAcc = {};
	_float			m_fUsingSurfaceParams = {};

	_float3			m_vOriginPosition = {};

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

	void Rotation();
	void Floating();

public:
	static CMapElement_Balloon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
	HRESULT Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root);
};

NS_END
