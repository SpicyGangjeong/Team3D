#pragma once

#include "Base.h"
#include "RenderEventDebugger.h"
NS_BEGIN(Engine)

class CVolumetric final : public CBase
{
	typedef struct tagCSVolumeConstantDesc
	{
		XMUINT3		VolumeSize;
		_float		fDepthPackExponent;	// 깊이 비선형 분포 지수 2.0 정도
		
		_float		fTemporalAccum;		// Temporal 사용 여부 off :0 /On: 1 
		_float		fFrameIndex;
		_float		fNear;
		_float		fFar;
		
		_float3		vCameraPosition;
		_float		fDensity;

		_float		fLightIntensity;
		_float3		vLightDirection;

		_float4		vLightColor;

		_float4x4	InvProjMatrix;
		_float4x4	InvViewMatrix;

		_float4x4	ShadowViewMatrix;
		_float4x4	ShadowViewProjMatrix;

		_float		fAsymmetryParameter;
		_uint		iNumLight;
		_float		fHeightOffset;
		_float		fBaseHeight;
	}CS_VOLUME_CONSTANT_DESC;
		
	typedef struct tagCSLightDesc
	{
		_float3		vDirection;
		_float		fRange;
		_float4		vPosition;
		_float4		vDiffuse;
	}CS_LIGHT_DESC;

private:
	CVolumetric(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVolumetric() = default;

public:
	HRESULT		Initialize();
	void		Update();
	void		Cumpute_Volume();
	void		Accumulate_Volume();
	HRESULT		Dispatch();

	ID3D11ShaderResourceView* Get_VolumeSRV() { return m_pAccVolumeSRV; }
	_float*		Get_DepthPackExponentPtr() { return &m_fDepthPackExponent; }

	void		Setting_Volumetirc(_float fDensity , _float fLightIntensity, _float fAsymmetryParameter , _float fDepthPackExponent, _float fHeightOffset);
private:
	class CGameInstance*		m_pGameInstance = { nullptr };
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	ID3D11Texture3D*			m_pTexture3D = { nullptr };
	ID3D11Texture3D*			m_pAccumulateTexture3D = { nullptr };

	ID3D11ShaderResourceView*	m_pNoiseTexture = { nullptr };

	ID3D11ShaderResourceView* 	m_pVolumeSRV = {nullptr};
	ID3D11UnorderedAccessView*  m_pVolumeUAV = { nullptr };

	ID3D11ShaderResourceView* 	m_pAccVolumeSRV = {nullptr};
	ID3D11UnorderedAccessView*  m_pAccVolumeUAV = { nullptr };

	ID3D11Buffer*				m_pConstantBuffer = { nullptr };
	ID3D11Buffer*				m_pLightInfoBuffer = { nullptr };
	ID3D11ShaderResourceView*	 m_pLightInfoSRV = { nullptr };

	ID3D11ComputeShader*		m_pCSVolumeCalculate = { nullptr };
	ID3DBlob*					m_pCSBlobCalculate = { nullptr };

	ID3D11ComputeShader*		m_pCSVolumeAccumulate = { nullptr };
	ID3DBlob*					m_pCSBlobAccumulate = { nullptr };

	ID3D11SamplerState*			m_pSamplerState = { nullptr };

	_float						m_fDensity = { 0.953f };				// 밀도
	_float						m_fLightIntensity = { 0.01f };			// 광원 강도
	_float						m_fAsymmetryParameter = { 0.5344f };	//  이방성 계수 값이 클 수록 들어오는 방향으로 나가는 빛의 양이 많아짐
	_float						m_fDepthPackExponent = { 1.f };			// 깊이 비선형 분포 지수
	_float						m_fHeightOffset = { 0.f };
	_float						m_fBaseHeight = { 0.f };
	_uint						m_iNumLights = { 1 };
	_uint						m_iMaxNumLights = { 20 };

	_float4x4					m_ShadowViewMatrix = {};
	_float4x4					m_ShadowViewProjMatrix = {};

#ifdef _DEBUG
	class CRenderEventDebugger	m_AnnotationHelper = {};

#endif // _DEBUG


private:
	HRESULT		CreateComputeShader();
	HRESULT		CreateBlob();
	HRESULT		CreateBuffers();
	HRESULT		CreateResourceViews();
	HRESULT		Create_SamplerState();

public:
	static CVolumetric* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();

#ifdef _DEBUG
	void Describe_Entity();
#endif // _DEBUG

};

NS_END
