#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CFont final : public CBase
{
private:
	CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFont() = default;

public:
	HRESULT Initialize(const _tchar* pFontFilePath);
	HRESULT Render(const _tchar* pText, const _float2& vPosition, _fvector vColor, _float vScale);
	HRESULT Render_Rotate(const _tchar* pText, const _float2& vPosition, _fvector vColor, _float Rotation, _float vScale);
	HRESULT Perspective_Render(_matrix View, _matrix Proj, const _tchar* pText, const _fvector& vPosition, _fvector vColor, _float vScale);
	_float FontSizeX(const _tchar* pText);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	SpriteBatch* m_pBatch = { nullptr };
	SpriteFont* m_pFont = { nullptr };

public:
	static CFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath);
	virtual void Free() override;
};

NS_END
