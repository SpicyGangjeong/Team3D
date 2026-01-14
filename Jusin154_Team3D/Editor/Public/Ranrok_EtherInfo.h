#pragma once

#include "Editor_Define.h"
#include "PartObject.h"

NS_BEGIN(Editor)

class CRanrok_EtherInfo final : public CGameObject
{
public:
	typedef struct tagTimeInfo
	{
		_float2		vAnimIndex = {};
		_float2     vAnimTime = { 0.f , 1.f };
		_float2		vLifeTime = {0.f , 1.f};
		_float2		vDiffuseUVMoveTime = { 0.f , 1.f };
		_float2		vMaskingUVMoveTime = { 0.f , 1.f };
		_float2		vNoiseUVMoveTime = { 0.f , 1.f };
		_float2		vDistortionUVMoveTime = { 0.f , 1.f };
		_float2		vDissolveUVMoveTime = { 0.f , 1.f };
	}TIME_INFO;
private:
	CRanrok_EtherInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRanrok_EtherInfo(const CRanrok_EtherInfo& Prototype);
	virtual ~CRanrok_EtherInfo() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Load_Info(const _char* pFilePath, LEVEL eLevel);
	HRESULT Save_Info(const _char* pPath);

	_bool isBloom() { return m_Ether_Info.isBloom; }
	_bool isBlur() { return m_Ether_Info.isBlur; }
	RENDER Get_RenderOrder() { return m_Ether_Info.eRenderOrder; }
public:
	void Update_Time(_float fTimeDelta);
	void Edit_Ether();
	HRESULT Bind_Ether_ShaderResources(class CShader* pShader);
	HRESULT Ether_Begin(class CShader* pShader);
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	



	EFFECT_INFO m_Ether_Info = {};
	TIME_INFO   m_ErherTimeInfo = {};

	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pNoise_TextureCom = { nullptr };
	CTexture* m_pMasking_TextureCom = { nullptr };
	CTexture* m_pDissolve_TextureCom = { nullptr };
	CTexture* m_pEmissive_TextureCom = { nullptr };
	CTexture* m_pDistortion_TextureCom = { nullptr };

	_string		m_strDiffuseName = {};
	_string		m_strNoiseName = {};
	_string		m_strMaskingName = {};
	_string		m_strDissolveName = {};
	_string     m_strEmissiveName = {};
	_string     m_strDistortionName = {};

	_string     m_strPath = {};
public:
	static CRanrok_EtherInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif
};

NS_END
