#pragma once

#include "Client_Define.h"
#include "MapElement.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)

class CMapElement_Lake final : public CMapElement
{
public:
	typedef struct tagMapObejct_Lake_Desc : public MAPELEMENT_DESC
	{
		_float					fRadius;
		_float					fTimeSpeed;
		_float					fRefractionStrength;
		_float					fRefractionPow;
		_float					fUVValue1;
		_float					fUVValue2;
		_float					fUVValue3;
		_float2					vUVSpeed;
		_float2					vLargeUVSpeed;
		_float2					vSubUVSpeed3;
		_float4					vRefractionColor;
		_float4					vSurfaceColor;
		vector<_wstring>		ShallowModelPrototypeTags;
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
	CTexture*		m_pRefractionTextureCom = { nullptr };

	_float			m_fRadius = {};

	_float			m_fRefractionStrength = { 0.18f };
	_float			m_fRefractionPow = { 10.f };
	_float          m_fTimeAcc = {};
	_float          m_fTimeSpeed = { 0.01f };

	_float          m_fUVValue1 = { 30.f };
	_float          m_fUVValue2 = { 20.f };
	_float          m_fUVValue3 = { 10.f };

	_float2         m_vUVSpeed = {};
	_float2         m_vLargeUVSpeed = {};
	_float2         m_vSubUVSpeed3 = {};

	_float4			m_vRefractionColor = {};
	_float4			m_vSurfaceColor = {};

	vector<CModel*> m_pShallowModels = {};
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CMapElement_Lake* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
