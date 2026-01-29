#pragma once

#include "Base.h"
#include "RenderEventDebugger.h"

NS_BEGIN(Engine)

class CRenderer final : public CBase
{
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	void	Render();
	RENDER	Get_CurrentRenderPass();
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pRenderObject);
	void	Render_PreShadow(const _float4x4& ViewMatrix, const _float4x4& ProjMatrix);
	HRESULT Bind_PreShadowMatrix(class CShader* pShader, const _char* pConstants, D3DTS eType);
	HRESULT Bind_PrevMatrix(class CShader* pShader, const _char* pConstants, D3DTS eType);
	void Set_Environment(_float3 vSSAO, _float fExposure);

#ifdef _DEBUG
private:
	HRESULT Add_DebugComponent(class CComponent* pDebugCom);
#endif
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	list<class CGameObject*>	m_RenderObjects[ENUM_CLASS(RENDER::END)];
	RENDER m_eType = RENDER::END;

#ifdef _DEBUG
	CRenderEventDebugger m_AnnotationHelper = {};
#endif // _DEBUG


#ifdef _DEBUG
private:
	list<class CComponent*>				m_DebugComponents;
#endif
private:
	class CShader* m_pShader = { nullptr };
	class CShader* m_pBlurShader = { nullptr };
	class CShader* m_pWeightBlendShader = { nullptr };
	class CShader* m_pDistortionShader = { nullptr };
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };

private:
	_float4x4					m_ScreenWorldMatrix = {};
	_float4x4					m_ScreenViewMatrix = {};
	_float4x4					m_ScreenProjMatrix = {};

	ID3D11DepthStencilView*		m_pShadowDSV_NEAR = { nullptr };
	ID3D11DepthStencilView*		m_pShadowDSV_MIDDLE = { nullptr };
	ID3D11DepthStencilView*		m_pShadowDSV_FAR = { nullptr };

	_float2 m_vNearShadowResoltion = { (_float)g_iNearShadowWidth,(_float)g_iNearShadowHeight };
	_float2 m_vMiddleShadowResoltion = { (_float)g_iMiddleShadowWidth,(_float)g_iMiddleShadowHeight };
	_float2 m_vFarShadowResoltion = { (_float)g_iFarShadowWidth,(_float)g_iFarShadowHeight };
	_float2 m_vPreShadowResoltion = { (_float)g_iPreShadowWidth,(_float)g_iPreShadowHeight };


	ID3D11DepthStencilView*		m_pPreShadowDSV = { nullptr };
	ID3D11ShaderResourceView*	m_pSSAO_NoiseSRV = { nullptr };
	ID3D11Texture2D*			m_pSSAO_NoiseTexture = { nullptr };

	_float4x4					m_MotionBlurPreViewMatrix = {};
	_float4x4					m_MotionBlurPreProjMatrix = {};

	_float4x4					m_PreShadowView = {};
	_float4x4					m_PreShadowProj = {};

#pragma region TunningParameters
	/* TunningParam  */
	// Variable
	_float m_fLightSpecularMaximum = { 2.f };
	_float m_fNearShadowBias = { 0.05f };
	_float m_fFarShadowBias = { 1.f };
	_float m_fMinShadowBrightness = { 0.47f };

	// MotionBlur 
	_float	m_fMBMaxBlurRadius = { 28.f };
	_float	m_fMBSampleBias = { 0.005f };
	_int	m_iMBMaxSampleCount = { 28 };
	_int	m_iMBSampleCount = { m_iMBMaxSampleCount };
	_int	m_iMBTileSize = { 20 };
	_int	m_iMBType = { 0 };
	_bool	m_bMB = { true }; 

	// ToneMapping
	_int	m_iToneMappingType = { 2 };
	_float	m_fExposure = { 0.7f };

	// Bloom
	_int	m_iBloomEmbossingPass = { 1 };
	_float	m_fBloomThreshold = { 1.26f };
	
	// DOF Environment
	_float	m_fDOF_ENV_CutThreshold = { 0.191f };
	_float	m_fDOF_ENV_FocusDistance = { 38.f };
	_float	m_fDOF_ENV_StartDistance = { 88.f };
	_float	m_fDOF_ENV_MaxEnd = { 237.f };
	_float	m_fDOF_ENV_AmountRadius = { 1.2f };

	// SSAO
	_float	m_fSSAO_Radius	= { 0.543f };
	_float	m_fSSAO_BIAS	= { 0.401f };
	_float	m_fSSAOStrength = { 4.f };

	SSAO_GEOMETRY_HEMISPHERE m_tagSSAOGeometry = {};
	SSAO_GEOMETRYDIRECTIONS_RANDOM_REAL m_tagSSAOGeometryDirections = {};
#pragma endregion

private:
	void Bind_RawValue();
	void Render_Occlusion();
	void Render_Priority();
	void Render_Shadow();
	void Render_NonBlend();
	void Render_Decal();
	void Render_EffectNonBlend();
	void Render_Blur(); 
	void Combine_Blur();
	void Render_SSAO();
	void Render_SSAO_BLUR();
	void Render_LightAcc();
	void Render_Combined();
	void Render_EnvironmentPostProcess();
	void Render_Fog();
	void Render_Effect();
	void Render_WeightBlend();
	void Render_NonLight();
	void Render_Blend();
	void Render_Blur_Mesh();
	void Render_PostProcessing();
	void Render_Distortion();
	void Render_DistortionAcc();
	void Render_Tone_Mapping();
	void Render_UI();
	void Render_UI_Overley();


#ifdef _DEBUG
private:
	void Render_Debug();
#endif

private:
	void	Fill_Geometry(_uint iNumSample);
	HRESULT Ready_ShadowDepthStencilView(_float2 vSize, ID3D11DepthStencilView** pDSV);

private:
	HRESULT Initialize();
public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();
#ifdef _DEBUG
	virtual void Describe_Entitiy();
#endif // _DEBUG

};

NS_END
