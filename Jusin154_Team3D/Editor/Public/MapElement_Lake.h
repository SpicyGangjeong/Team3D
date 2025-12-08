#pragma once

#include "Editor_Define.h"
#include "MapElement.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Editor)

class CMapElement_Lake final : public CMapElement
{
public:
	typedef struct tagMapObejct_Lake_Desc : public MAPOBJECT_LOD_DESC
	{
		vector<_wstring>		SurfaceModelPrototypeTags;
	}MAPOBJECT_LAKE_DESC;

private:
	CMapElement_Lake(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapElement_Lake(const CMapElement_Lake& rhs);
	virtual ~CMapElement_Lake() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*		m_pNormalTextureCom = { nullptr };
	CTexture*		m_pNormalLargeTextureCom = { nullptr };
	CTexture*		m_pNormalSubTextureCom = { nullptr };
	CTexture*		m_pCausticsTextureCom = { nullptr };
	CTexture*		m_pNoiseTextureCom = { nullptr };
	CTexture*		m_pCubeMapTextureCom = { nullptr };

	_float			m_fRefractionStrength = {0.1f};
	_float			m_fRefractionPow = {5.f};
	_float          m_fTimeAcc = {};
	_float          m_fUVValue = {0.05f};
	_float          m_fUVValue1 = {10.f};
	_float          m_fUVValue2 = {20.f};
	_float          m_fUVValue3 = {10.f};

	_float          m_fUVSpeed1 = { 0.01f };
	_float          m_fUVSpeed2 = { 0.001f };
	_float          m_fUVSpeed3 = { 0.01f };

	_float4			m_vRefractionColor = {};
	_float4			m_vSurfaceColor = {};
	
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CMapElement_Lake* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
