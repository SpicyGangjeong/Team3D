#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CKey_Manager final : public CBase
{
private:
	CKey_Manager();
	virtual ~CKey_Manager() = default;

public:

	/* Keyboard */
	// ²Ú ´©¸¦ ¶§
	_bool		Key_Pressing(int _iKey) const;
	// ´­·¶´Ù°¡ ¶ÃÀ» ¶§
	_bool		Key_Up(int _iKey) const;
	// ´©¸¥ Å¸ÀÌ¹Ö µü ÇÑ ¹ø ¸¸
	_bool		Key_Down(int _iKey) const;

	/* Mouse */
	// ²Ú ´©¸¦ ¶§
	_bool		Mouse_Pressing(int _iKey) const;
	// ´­·¶´Ù°¡ ¶ÃÀ» ¶§
	_bool		Mouse_Up(int _iKey) const;
	// ´©¸¥ Å¸ÀÌ¹Ö µü ÇÑ ¹ø ¸¸
	_bool		Mouse_Down(int _iKey) const;

	_bool		Mouse_StartMove() const;
	_bool		Mouse_Moving() const;
	_bool		Mouse_StopMove() const;
	_float3		Get_MouseMove() const;

	void		Update();
private:
	LPDIRECTINPUT8	m_pInputSDK = { nullptr };
	LPDIRECTINPUTDEVICE8 m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8 m_pMouse = { nullptr };


private:
	HANDLE		m_hWnd = {};

	_byte		m_byOldKeyState[DIK_MAX + 1]; // 255 + 1
	_byte		m_byNewKeyState[DIK_MAX + 1];

	DIMOUSESTATE2	m_tOldMouseState = {};
	DIMOUSESTATE2	m_tNewMouseState = {};

private:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);


public:
	static CKey_Manager* Create(HINSTANCE hInstance, HWND hWnd);
	virtual void			Free() override;
};

NS_END