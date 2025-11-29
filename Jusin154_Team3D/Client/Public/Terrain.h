#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Terrain;
class CTexture;
NS_END

NS_BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	typedef struct TerrainDesc
	{
		_float3		vPosition;
		_wstring	strDiffuseTextureTag;
		_wstring	strNormalTextureTag;
		_wstring	strMROTextureTag;
		_wstring	strAlphaMapTextureTag;
	}TERRAIN_DESC;
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& Prototype);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CVIBuffer_Terrain*		m_pVIBufferCom = { nullptr };
	CRigidBody_Static*		m_pRigidBody = { nullptr };
	CTexture*				m_pDiffuseTextureCom = { nullptr };
	CTexture*				m_pNormalTextureCom = { nullptr };
	CTexture*				m_pMROTextureCom = { nullptr };
	CTexture*				m_pAlphaMapTextureCom = { nullptr };

	_float3					m_vDRN = { 16.f, 16.f, 16.f };

	_float					m_fUsingSurfaceParams = {};
	_float3					m_vPickingPosition = {};

	_uint					m_iTextureIndex = {};

#ifdef _DEBUG
	_bool					m_bWasWireFrame = { false };
	_float					m_fCullingRadius = { 0.2f };
#endif // _DEBUG

private:
	virtual HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
