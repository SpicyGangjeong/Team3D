#include "pch.h"
#include "Key_Manager.h"

CKey_Manager::CKey_Manager()
{
	ZeroMemory(m_byOldKeyState, sizeof(m_byOldKeyState));
	ZeroMemory(m_byNewKeyState, sizeof(m_byNewKeyState));
}


bool CKey_Manager::Key_Pressing(int _iKey) const
{
	// 현재프레임에 눌리고 있다면 Pressing
	if ((m_byNewKeyState[_iKey] & 0x80)) {
		return true;
	}
	return false;
}

bool CKey_Manager::Key_Up(int _iKey) const
{
	// 이전프레임에 눌렸고 현재프레임에 안눌렸다면 Up
	if ((m_byOldKeyState[_iKey] & 0x80) && !(m_byNewKeyState[_iKey] & 0x80)) {
		return true;
	}
	return false;
}

bool CKey_Manager::Key_Down(int _iKey) const
{
	// 이전프레임에 안눌렸고 현재프레임에 눌렸다면 Down
	if (!(m_byOldKeyState[_iKey] & 0x80) && (m_byNewKeyState[_iKey] & 0x80)) {
		return true;
	}
	return false;
}

_bool CKey_Manager::Mouse_Pressing(int _iKey) const
{
	// 현재프레임에 눌리고 있다면 Pressing
	if ((m_tNewMouseState.rgbButtons[_iKey] & 0x80)) {
		return true;
	}
	return false;
}

_bool CKey_Manager::Mouse_Up(int _iKey) const
{
	// 이전프레임에 눌렸고 현재프레임에 안눌렸다면 Up
	if ((m_tOldMouseState.rgbButtons[_iKey] & 0x80) && !(m_tNewMouseState.rgbButtons[_iKey] & 0x80)) {
		return true;
	}
	return false;
}

_bool CKey_Manager::Mouse_Down(int _iKey) const
{
	// 이전프레임에 눌렸고 현재프레임에 안눌렸다면 Down
	if (!(m_tOldMouseState.rgbButtons[_iKey] & 0x80) && (m_tNewMouseState.rgbButtons[_iKey] & 0x80)) {
		return true;
	}
	return false;
}

_bool CKey_Manager::Mouse_StartMove() const
{
	_int iOldMomentum = m_tOldMouseState.lX * m_tOldMouseState.lX
		+ m_tOldMouseState.lY * m_tOldMouseState.lY
		+ m_tOldMouseState.lZ * m_tOldMouseState.lZ;
	_int iNewMomentum = m_tNewMouseState.lX * m_tNewMouseState.lX
		+ m_tNewMouseState.lY * m_tNewMouseState.lY
		+ m_tNewMouseState.lZ * m_tNewMouseState.lZ;
	// 전에 안움직였고 지금 움직이면 Start
	if (iOldMomentum == 0 && iNewMomentum > 0) {
		return true;
	}
	return false;
}

_bool CKey_Manager::Mouse_Moving() const
{
	// 현재프레임에 움직이고 있다면 Moving
	_int iNewMomentum = m_tNewMouseState.lX * m_tNewMouseState.lX
		+ m_tNewMouseState.lY * m_tNewMouseState.lY
		+ m_tNewMouseState.lZ * m_tNewMouseState.lZ;
	if (iNewMomentum > 0) {
		return true;
	}
	return false;
}

_bool CKey_Manager::Mouse_StopMove() const
{
	_int iOldMomentum = m_tOldMouseState.lX * m_tOldMouseState.lX
		+ m_tOldMouseState.lY * m_tOldMouseState.lY
		+ m_tOldMouseState.lZ * m_tOldMouseState.lZ;
	_int iNewMomentum = m_tNewMouseState.lX * m_tNewMouseState.lX
		+ m_tNewMouseState.lY * m_tNewMouseState.lY
		+ m_tNewMouseState.lZ * m_tNewMouseState.lZ;
	// 전에 움직였고 지금은 안움직이면 Stop
	if (iOldMomentum > 0 && iNewMomentum == 0) {
		return true;
	}
	return false;
}

_float3 CKey_Manager::Get_MouseMove() const
{
	return _float3((_float)m_tNewMouseState.lX, (_float)m_tNewMouseState.lY, (_float)m_tNewMouseState.lZ);
}

void CKey_Manager::Update()
{
	if (GetForegroundWindow() == m_hWnd) {
		// 이전 상태 저장
		memcpy_s(m_byOldKeyState, sizeof(m_byOldKeyState), m_byNewKeyState, sizeof(m_byNewKeyState));
		memcpy_s(&m_tOldMouseState, sizeof(m_tOldMouseState), &m_tNewMouseState, sizeof(m_tNewMouseState));

		// 장치 상태 읽기
		HRESULT hrKeyboard = m_pKeyBoard->GetDeviceState(DIK_MAX + 1, m_byNewKeyState);
		if (FAILED(hrKeyboard)) {
			m_pKeyBoard->Acquire(); // 다시 획득
		}

		HRESULT hrMouse = m_pMouse->GetDeviceState(sizeof(m_tNewMouseState), &m_tNewMouseState);
		if (FAILED(hrMouse)) {
			m_pMouse->Acquire(); // 다시 획득
		}
	}
	else {
		// 비활성화 상태라면 그냥 무시
		ZeroMemory(m_byNewKeyState, sizeof(m_byNewKeyState));
		ZeroMemory(&m_tNewMouseState, sizeof(m_tNewMouseState));
	}
}

HRESULT CKey_Manager::Initialize(HINSTANCE hInst, HWND hWnd)
{
	m_hWnd = hWnd;

	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, NULL))) {
		return E_FAIL;
	}


	// keyboard
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr))) {
		return E_FAIL;
	}

	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	m_pKeyBoard->Acquire();


	// Mouse
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr))) {
		return E_FAIL;
	}

	m_pMouse->SetDataFormat(&c_dfDIMouse2);

	m_pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	m_pMouse->Acquire();

	return S_OK;
}

CKey_Manager* CKey_Manager::Create(HINSTANCE hInstance, HWND hWnd)
{
	CKey_Manager* pInstance = new CKey_Manager();

	if (FAILED(pInstance->Initialize(hInstance, hWnd)))
	{
		MSG_BOX("Failed to Created : CKey_Manager");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CKey_Manager::Free()
{
	__super::Free();
	SAFE_RELEASE(m_pKeyBoard);
	SAFE_RELEASE(m_pMouse);
	SAFE_RELEASE(m_pInputSDK);
}