#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Terrain;
class CTexture;
NS_END

NS_BEGIN(Editor)

class CTerrain final : public CGameObject
{
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
	CTexture*				m_pDiffuseTextureCom = { nullptr };
	CTexture*				m_pNormalTextureCom = { nullptr };
	CTexture*				m_pMROTextureCom = { nullptr };
	CTexture*				m_pMaskTextureCom = { nullptr };

	_float					m_fUsingSurfaceParams = {};
	_float					m_fHeightRatio = {1.f};
	_float3					m_vPickingPosition = {};
	_float3					m_vRotation = {};
	_float					m_fCullingRadius = { 0.2f };

	_uint					m_iTextureIndex = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
