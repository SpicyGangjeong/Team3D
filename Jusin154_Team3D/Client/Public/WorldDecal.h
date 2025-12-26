#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CWorldDecal final : public CGameObject
{
public:
	typedef struct tagWorldDecalDesc
	{
		_float3				vPosition = {};
		_float3				vRotation = {};
		_float3				vScale = { 1.f, 1.f, 1.f };
		_float				fUVTiling = { 1.f };
		_float2				vUVSpeed = {};
		_float4				vMaskRed = {};
		_float4				vMaskGreen = {};
		_float4				vMaskBlue = {};
	}WORLD_DECAL_DESC;

private:
	CWorldDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWorldDecal(const CWorldDecal& rhs);
	virtual ~CWorldDecal() = default;

public:
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };

	CTexture*			m_pMaskTextureCom = { nullptr };
	CTexture*			m_pNormalTextureCom = { nullptr };
	CTexture*			m_pSurfaceTextureCom = { nullptr };
	CTexture*			m_pFadeTextureCom = { nullptr };
	CTexture*			m_pDiffuseMaskTextureCom = { nullptr };

	_float				m_fUVTiling = { 1.f };
	_float				m_fUsingSurfaceParams = {};

	_float				m_fWinSizeX = {};
	_float				m_fWinSizeY = {};

	_float2				m_fTimeAcc = {};
	_float2				m_vUVSpeed = {};

	_float4				m_vMaskRed = {};
	_float4				m_vMaskGreen = {};
	_float4				m_vMaskBlue = {};

	_float4x4			m_WorldMatrixInv = {};

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CWorldDecal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
