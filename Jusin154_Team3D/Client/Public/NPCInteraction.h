#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CNPCInteraction final : public CGameObject
{
private:
	CNPCInteraction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPCInteraction(const CNPCInteraction& rhs);
	virtual ~CNPCInteraction() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;


private:
	HRESULT	Bind_ShaderResources();
	HRESULT	Ready_Components();
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	_float4 ComputeUV(_tchar Alphabat);
	void Set_ImageSizePosition();
	void ActiveOff();
public:
	void NpcInfo(void* pArg);

private:
	CInfoInstance* m_pInfoInstance = { nullptr };

	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CTexture* m_pDiffuse_TextureCom2 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	class CUnit* m_pUnit = { nullptr };

	_float4 m_vFontPosition{};
	_float4 m_vFontPosition1{};

	_float m_fAlpha{};

	_wstring	m_NpcName;

	_float4 m_vPosition{};
	_float4 m_vUV{};
	_float4 m_vNine_Slice{};
	_float4 m_vImagePosi1{};


	_float3 m_fSize{};
	_float3 m_fOrigin_Size{};
	_float2 m_fViewPort{};

	_float m_fFontSize{};
	_bool	m_bActive{};

	NPCINTERACTIONINFO m_pInfo{};
public:
	static CNPCInteraction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // DEBUG


};

NS_END
