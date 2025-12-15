#pragma once

#include "Editor_Define.h"
#include "EffectObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CTrail;
class CTransform;
NS_END

NS_BEGIN(Editor)

class CTrailObject final : public CPartObject
{
public:
	typedef struct tagTrailInfo
	{
		EFFECT_TYPE eEffectType = { EFFECT_TYPE::TRAIL };
		/* 디퓨즈 */
		_bool		isDiffuse = {};
		_float		fDiffuseAlpha = { 1.f };
		_float4		vColor = { 0.f ,0.f ,0.f ,1.f };

		/* 마스크 */
		_bool       isMask = {};
		_float		fSoftMaskEdge = {};
		_float		fSoftMask = {};

		/* 노이즈 */
		_bool       isNoise = {};
		_bool		isNoiseColor = { false };
		_bool		isNoiseAlpha = { false };
		_float      fNoiseStrength = {};

		/* 디스 토션 */
		_bool		isDistortion = {};
		_float2     vDistortionTime = {};
		_float2		vDiffuseDistortioUVAmount = {};
		_float2		vMaskDistortionUVAmount = {};
		_float		fDistortionIntensity = {};

		/* 이미시브 */
		_float4		vEmissive = { 0.f ,0.f ,0.f ,0.f };

		_float		fEmissiveStrength = { 0.f };
		_float		fSoftenExp = { 1.31429f };
		_float		fSoftStrength = {};
		_float		fCoreBoost = {};
		_float		fRadius = {};


		/* 블러 */
		_bool		isBlur = {};
		_bool		isOnlyBlur = { false };
		_float		fBlurIntensity = {};
		_int		iBlurWeight = {};

		/* 블룸 */
		_bool       isBloom = {};

		_bool       isBloomDissolve = {};
		_bool       isBloomReverseDissolve = {};

		_float		fBloomStrength = {};

		BLOOM_TYPE  eBloomType = {};
		_float2     vBloomTime = {};

		_int	    iNumVertex = { 256 };


		RENDER		eRenderOrder = { RENDER::EFFECT };
		SHADER_PASS_POSTEX eShaderPass = { SHADER_PASS_POSTEX::TRAIL };

		_float      fDamping = { 0.5f };
		_float      fRopeLength = { 0.1f };
		_float		fMass = { 0.5f };

		_float3 vPadding1 = {};
		_float3 vPadding2 = {};

		_float  fPadding1 = {};
		_float  fPadding2 = {};
		_float  fPadding3 = {};
			    
		_bool   isPadding0 = {};
		_bool   isPadding1 = {};
		_bool   isPadding2 = {};

	}TRAIL_INFO;
private:
	CTrailObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrailObject(const CTrailObject& rhs);
	virtual ~CTrailObject() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	void	     Set_Target(class CTransform* pTargetTransform);
	void         Trail_Update(_fmatrix WorldMat, _float fTimeDelta);
	void         Rope_Trail_Update(_fmatrix WorldMat, _fmatrix EndWorldMat, _float fTimeDelta);
public:
#ifdef _DEBUG
	HRESULT Save_Trail(const _char* pPath);
#endif
	HRESULT Load_Trail(const _char* pPath, LEVEL eLevel);
private:
	HRESULT Bind_ShaderResources() override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Blur() override;
	virtual HRESULT Render_Bloom() override;

public:
	static CTrailObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;

private:
	CTexture*	m_pDiffuse_TextureCom = { nullptr };
	CTexture*	m_pNoise_TextureCom = { nullptr };
	CTexture*	m_pMasking_TextureCom = { nullptr };
	CTexture*	m_pDistortion_TextureCom = { nullptr };

	CShader*	m_pShaderCom = { nullptr };
	CTrail*		m_pTrailCom = { nullptr };

	_string		m_strTrailDiffuseName = {};
	_string		m_strTrailNoiseName = {};
	_string		m_strTrailMaskingName = {};
	_string     m_strTrailDistortionName = {};

	_string		m_strPath = {};
	_string     m_strName = {};

	TRAIL_INFO  m_TrailInfo = {};
	
	_float4x4*  m_pParantsMatrix = {};


};

NS_END
