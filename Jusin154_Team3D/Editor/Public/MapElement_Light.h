#pragma once

#include "Editor_Define.h"
#include "MapElement.h"

NS_BEGIN(Engine)
class CLight;
class CTexture;
NS_END

NS_BEGIN(Editor)

class CMapElement_Light : public CMapElement
{
public:
	typedef struct MapElement_Light_Desc : MAPOBJECT_LOD_DESC
	{
		_bool			isPow;
		_uint			iGlassMeshIndex;
		_float			fBloomStregth;
		_float			fRange;
		_float4			vPosOffset;
		_float4			vDiffuse;
		_float4			vAmbient;
		_float4			vSpecular;
	}MAPELEMENT_LIGHT_DESC;

private:
	CMapElement_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapElement_Light(const CMapElement_Light& rhs);
	virtual ~CMapElement_Light() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Render_Bloom()override;
	void Toggle_Light();

private:
	CLight*			m_pLightCom = { nullptr };
	CTexture*		m_pEmissiveTextureCom = { nullptr };
	CTexture*		m_pMaskTextureCom = { nullptr };
	CTexture*		m_pGlassTextureCom = { nullptr };

	_bool			m_isLightOn = { true };
	_bool			m_isLightAdded = { false };
	_uint			m_iGlassMeshIndex = {};
	_float			m_fBloomStrength = {1.f};
	_float			m_fGlassRatio = { 0.5f };

	_float			m_LightAdded_Distance = { 5000.f };

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;


public:
	static CMapElement_Light* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
	virtual HRESULT Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root);
};

NS_END
