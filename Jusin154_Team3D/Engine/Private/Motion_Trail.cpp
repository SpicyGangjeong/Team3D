#include "pch.h"
#include "Motion_Trail.h" 
#include "GameInstance.h" 
#include "Mesh.h" 
#include "Bone.h"

CMotion_Trail::CMotion_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	CComponent(pDevice, pContext)
{
}

CMotion_Trail::CMotion_Trail(const CMotion_Trail& rhs)
	: CComponent(rhs),
	m_pClearBuffer(rhs.m_pClearBuffer),
	m_CaptureInfos(rhs.m_CaptureInfos),
	m_RemainingInfos(rhs.m_RemainingInfos),
	m_fMaxCaptureLifeTime(rhs.m_fMaxCaptureLifeTime),
	m_iMaximumCapture(rhs.m_iMaximumCapture),
	m_iNumBones(rhs.m_iMaximumCapture)
{
}

HRESULT CMotion_Trail::Update_Capture(_float fDeltaTime)
{
	list<CAPTUREINFO*>::iterator iter = m_CaptureInfos->begin();
	for (; iter != m_CaptureInfos->end();)
	{
		CAPTUREINFO* pInfo = (*iter);
		pInfo->vCaptureLifeTime.x += fDeltaTime;
		if (m_fMaxCaptureLifeTime < pInfo->vCaptureLifeTime.x) {
			if (FAILED(ClearInfos(*pInfo))) {
				return E_FAIL;
			}
			m_RemainingInfos->push(pInfo);
			iter = m_CaptureInfos->erase(iter);
			continue;
		}
		else {
			iter++;
		}
	}
	return S_OK;
}

HRESULT CMotion_Trail::Clear_Captures()
{
	list<CAPTUREINFO*>::iterator iter = m_CaptureInfos->begin();
	for (; iter != m_CaptureInfos->end();)
	{
		CAPTUREINFO* pInfo = (*iter);
		if (FAILED(ClearInfos(*pInfo))) {
			return E_FAIL;
		}
		m_RemainingInfos->push(pInfo);
		iter = m_CaptureInfos->erase(iter);
	}
	return S_OK;
}

HRESULT CMotion_Trail::Render(CShader* pShader)
{
	_float	fRatio = { 1.f };
	CAPTUREINFO* pInfo = { nullptr };
	list<CAPTUREINFO*>::iterator iter = m_CaptureInfos->begin();
	for (; iter != m_CaptureInfos->end(); ++iter)
	{
		pInfo = (*iter);
		fRatio = (pInfo->vCaptureLifeTime.x / pInfo->vCaptureLifeTime.y);
		if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &pInfo->WorldMatrix))) {
			return E_FAIL;
		}

		if (FAILED(pShader->Bind_RawValue("g_vMotionTrail_LifeTime", &pInfo->vCaptureLifeTime , sizeof(_float2))))
		{
			return E_FAIL;
		}

		//if (FAILED(pShader->Bind_RawValue("g_LifeTime", &fRatio, sizeof(_float)))) {
		//	return E_FAIL;
		//}

		m_funcRender(pInfo->pBoneMatrixSRV);
	}
	return S_OK;
}

HRESULT CMotion_Trail::Capture_Model(ID3D11Buffer* pBoneBuffer, const _float4x4& matCurrentWorld)
{
	CAPTUREINFO* pInfo = { nullptr };
	if (m_RemainingInfos->empty()) {
		pInfo = m_CaptureInfos->front();
		if (FAILED(ClearInfos(*pInfo))) {
			return E_FAIL;
		}
		m_CaptureInfos->pop_front();
	}
	else {
		pInfo = m_RemainingInfos->front();
		m_RemainingInfos->pop();
	}
	m_pContext->CopyResource(pInfo->pBoneMatrixBuffer, pBoneBuffer);
	pInfo->WorldMatrix = matCurrentWorld;
	m_CaptureInfos->emplace_back(pInfo);

	return S_OK;
}

HRESULT CMotion_Trail::ClearInfos(CAPTUREINFO& Info)
{
	Info.vCaptureLifeTime.x = 0.f;
	Info.vCaptureLifeTime.y = m_fMaxCaptureLifeTime;
	m_pContext->CopyResource(Info.pBoneMatrixBuffer, m_pClearBuffer);
	return S_OK;
}

HRESULT CMotion_Trail::Initialize_Prototype(MODELCAPTURE_DESC* pDesc)
{
	m_fMaxCaptureLifeTime	= pDesc->fMaxCaptureLifeTime;
	m_iMaximumCapture		= pDesc->iMaximumCapture;
	m_iNumBones				= pDesc->iNumBones;
	m_CaptureInfos = new list<CAPTUREINFO*>;
	m_RemainingInfos = new queue<CAPTUREINFO*>;
	vector<CAPTUREINFO*> Infos = {};
	for (_uint i = 0; i < m_iMaximumCapture; ++i) {
		CAPTUREINFO* pInfo = new CAPTUREINFO();
		m_RemainingInfos->push(pInfo);
		Infos.push_back(pInfo);
	}
	CreateVBs(Infos);
	return S_OK;
}

HRESULT CMotion_Trail::Initialize(void* pArg)
{
	MOTIONTRAIL_RENDERFUNC* pDesc = static_cast<MOTIONTRAIL_RENDERFUNC*>(pArg);
	if (nullptr == pDesc->funcRenderCall) {
		return E_FAIL;
	}
	m_funcRender = pDesc->funcRenderCall;
	return S_OK;
}

HRESULT CMotion_Trail::CreateVBs(vector<CAPTUREINFO*>& Infos)
{
	_uint iCount = (_uint)m_iNumBones;
	_uint iStride = sizeof(BONE_DESC);

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = iStride * iCount;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.StructureByteStride = iStride;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc1 = {};
	srvDesc1.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc1.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc1.Buffer.FirstElement = 0;
	srvDesc1.Buffer.NumElements = (_uint)m_iNumBones;
	
	_matrix IdentityMatirx = XMMatrixIdentity();
	HRESULT hr = m_pDevice->CreateBuffer(&desc, nullptr, &m_pClearBuffer);
	if (FAILED(hr)) {
		return E_FAIL;
	}

	for (_uint i = 0; i < Infos.size(); ++i) {
		CAPTUREINFO* pInfo = Infos[i];
		pInfo->vCaptureLifeTime.y = m_fMaxCaptureLifeTime;
		XMStoreFloat4x4(&pInfo->WorldMatrix, IdentityMatirx);

		HRESULT hr = m_pDevice->CreateBuffer(&desc, nullptr, &pInfo->pBoneMatrixBuffer);
		if (FAILED(hr)) {
			return E_FAIL;
		}

		if (FAILED(m_pDevice->CreateShaderResourceView(pInfo->pBoneMatrixBuffer, &srvDesc1, &pInfo->pBoneMatrixSRV))) {
			return E_FAIL;
		}

	}
	return S_OK;
}

CMotion_Trail* CMotion_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELCAPTURE_DESC* pDesc)
{
	CMotion_Trail* pInstance = new CMotion_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pDesc)))
	{
		MSG_BOX("Failed to Created : CMotion_Trail");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CMotion_Trail* CMotion_Trail::Clone(void* pArg, class CGameObject* pOwner)
{
	CMotion_Trail* pInstance = new CMotion_Trail(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMotion_Trail");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMotion_Trail::Free()
{
	__super::Free();

	if (false == m_bCloned) {
		list<CAPTUREINFO*>::iterator iter = m_CaptureInfos->begin();
		for (; iter != m_CaptureInfos->end();) {
			SAFE_RELEASE((*iter)->pBoneMatrixBuffer);
			SAFE_RELEASE((*iter)->pBoneMatrixSRV);
			Safe_Delete((*iter));
			iter = m_CaptureInfos->erase(iter);
		}
		while (!(m_RemainingInfos->empty()))
		{
			CAPTUREINFO* pInfos = m_RemainingInfos->front();
			m_RemainingInfos->pop();
			SAFE_RELEASE(pInfos->pBoneMatrixBuffer);
			SAFE_RELEASE(pInfos->pBoneMatrixSRV);
			Safe_Delete(pInfos);
		}
		Safe_Delete(m_CaptureInfos);
		Safe_Delete(m_RemainingInfos);
		SAFE_RELEASE(m_pClearBuffer);
	}
}

#ifdef _DEBUG
void CMotion_Trail::Describe_Entity()
{

	if (GUI::TreeNode("Motion Trail"))
	{

		GUI::DragFloat("m_fMaxCaptureLifeTime", &m_fMaxCaptureLifeTime);

		_int iMaximumCapture = m_iMaximumCapture;
		if (GUI::DragInt("m_iMaximumCapture", &iMaximumCapture))
		{
			m_iMaximumCapture = iMaximumCapture;
		}


		GUI::TreePop();
	}
}
#endif // _DEBUG


