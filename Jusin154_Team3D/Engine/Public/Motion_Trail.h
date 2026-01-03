#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMotion_Trail final : public CComponent
{
public:
	typedef struct tagCaptureInfo
	{
		ID3D11Buffer*				pBoneMatrixBuffer = { nullptr };
		ID3D11ShaderResourceView*	pBoneMatrixSRV = { nullptr };
		_float4x4					WorldMatrix			= {  };
		_float2						vCaptureLifeTime	= { 0.f, 2.f };
	}CAPTUREINFO, * LPCAPTUREINFO;

	typedef struct tagModelCapturedDesc
	{
		_float					fMaxCaptureLifeTime = {};
		_uint					iMaximumCapture = { 10 };
		_uint					iNumBones = {};
	}MODELCAPTURE_DESC;

	typedef struct tagMotionTrailRenderFunc {
		function<void(ID3D11ShaderResourceView*)> funcRenderCall = { nullptr };
	}MOTIONTRAIL_RENDERFUNC;
private:
	CMotion_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMotion_Trail(const CMotion_Trail& rhs);
	   virtual ~CMotion_Trail() = default;

public:
	HRESULT Update_Capture(_float fDeltaTime); // 카피리소스 씀. 절대로 dispatch 전에 호출할 것
	HRESULT Clear_Captures();
	HRESULT Render(class CShader* pShader);
	HRESULT Capture_Model(ID3D11Buffer* pBoneBuffer, const _float4x4& TransformMatrix);

private:
	list<CAPTUREINFO*>*		m_CaptureInfos			= { nullptr };
	queue<CAPTUREINFO*>*	m_RemainingInfos		= { nullptr };

	_float					m_fMaxCaptureLifeTime	= { 2.f };
	_uint					m_iMaximumCapture		= { 10 };
	_uint					m_iNumBones				= { 0 };

	ID3D11Buffer*			m_pClearBuffer = { nullptr };
	function<void(ID3D11ShaderResourceView*)> m_funcRender = {};
private:
	HRESULT ClearInfos(CAPTUREINFO& Info);
	HRESULT Initialize_Prototype(MODELCAPTURE_DESC* pDesc);
	HRESULT Initialize(void* pArg) override;
	HRESULT CreateVBs(vector<CAPTUREINFO*>& Infos);

public:
	static CMotion_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELCAPTURE_DESC* pDesc);
	virtual CMotion_Trail* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;

	void Describe_Entity() override;
};

NS_END
