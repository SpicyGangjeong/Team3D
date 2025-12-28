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

		_float		fAsymmetryParameter;
		_float		padding0;
		_float		padding1;
		_float		padding2;
	}CS_VOLUME_CONSTANT_DESC;
		

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
	_float* Get_DepthPackExponentPtr() { return &m_fDepthPackExponent; }

	void		Setting_Volumetirc(_float fDensity , _float fLightIntensity, _float fAsymmetryParameter , _float fDepthPackExponent);
private:
	class CGameInstance*		m_pGameInstance = { nullptr };
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	ID3D11Texture3D*			m_pTexture3D = { nullptr };
	ID3D11Texture3D*			m_pAccumulateTexture3D = { nullptr };

	ID3D11ShaderResourceView* 	m_pVolumeSRV = {nullptr};
	ID3D11UnorderedAccessView*  m_pVolumeUAV = { nullptr };

	ID3D11ShaderResourceView* 	m_pAccVolumeSRV = {nullptr};
	ID3D11UnorderedAccessView*  m_pAccVolumeUAV = { nullptr };

	ID3D11Buffer*				m_pConstantBuffer = { nullptr };
	ID3D11Buffer*				m_pLightInfoBuffer = { nullptr };

	ID3D11ComputeShader*		m_pCSVolumeCalculate = { nullptr };
	ID3DBlob*					m_pCSBlobCalculate = { nullptr };

	ID3D11ComputeShader*		m_pCSVolumeAccumulate = { nullptr };
	ID3DBlob*					m_pCSBlobAccumulate = { nullptr };

	_float						m_fDensity = { 0.5f };
	_float						m_fLightIntensity = { 1.f };
	_float						m_fAsymmetryParameter = { 0.2f };
	_float						m_fDepthPackExponent = { 2.f };

#ifdef _DEBUG
	class CRenderEventDebugger	m_AnnotationHelper = {};

#endif // _DEBUG


private:
	HRESULT		CreateComputeShader();
	HRESULT		CreateBlob();
	HRESULT		CreateBuffers();
	HRESULT		CreateResourceViews();

public:
	static CVolumetric* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();

#ifdef _DEBUG
	void Describe_Entity();
#endif // _DEBUG

};

NS_END
