#include "Editor_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)

class CAlphaMap final : public CBase
{
private:
	CAlphaMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CAlphaMap() = default;

public:
	void	Update(_float3& vPosition, _uint iColorIndex, _float fValue, _uint iRange);
	void	Update_Land(_float2 vUV, _uint iColorIndex, _float fValue, _uint iRange);
	ID3D11ShaderResourceView* Get_SRV() { return m_pSRV; }

	void Save_ToFile(const _char* pFilePath);
	void Load_ToFile(const _char* pFilePath);
	void Save_DDS(const _char* pFilePath);
private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	ID3D11Texture2D*			m_pTexture2D = { nullptr };
	ID3D11ShaderResourceView*	m_pSRV = { nullptr };
	CGameInstance*				m_pGameInstance = { nullptr };

	_uint						m_iNumPixels = {};
	_uint						m_iNumPixelW = {};
	_uint						m_iNumPixelH = {};

	_float4*					m_pPixels = { nullptr };

private:
	HRESULT	Initialize( _uint iSizeX, _uint iSizeY);

public:
	static CAlphaMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY);
	virtual void Free();
};

NS_END
