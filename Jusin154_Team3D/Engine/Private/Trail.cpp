#include "pch.h"
#include "Trail.h"


CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CTrail::CTrail(const CTrail& rhs)
	: CVIBuffer(rhs)
{
}



HRESULT CTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrail::Initialize(void* pArg)
{
	TRAIL_DESC* pDesc = static_cast<TRAIL_DESC*>(pArg);

	if (pDesc == nullptr)
		return E_FAIL;

	m_TrailDesc = *pDesc;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = 256;
	m_iVertexStride = sizeof(VTXPOSTEX);

	m_iNumIndices = (m_iNumVertices / 2 - 1) * 6;
	m_iIndexStride = 4;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pVertices = new VTXPOSTEX[m_iNumVertices];
	m_pOldPosition = new _float3[m_iNumVertices];

	if (FAILED(Create_VB()))
		return E_FAIL;

	if (FAILED(Create_IB()))
		return E_FAIL;
		
	return S_OK;
}

HRESULT CTrail::Create_VB()
{

	if (m_pVB != nullptr)
		SAFE_RELEASE(m_pVB);

	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices]{};

	D3D11_SUBRESOURCE_DATA InitialVBDate{};
	InitialVBDate.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBDate, &m_pVB))) {
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CTrail::Create_IB()
{
	if (m_pIB != nullptr)
		SAFE_RELEASE(m_pIB);

	m_iNumIndices = (m_iNumVertices / 2 - 1) * 6;

	D3D11_BUFFER_DESC IBDesc{};
	IBDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;


	_uint* pIndices = new _uint[m_iNumIndices]{};

	_uint iIndex = { 0 };

	for (_uint i = 0; i < m_iNumVertices / 2 - 1; i++)
	{
		_uint iStartIndex = i * 2;

		_uint Indicies[4] = {
			iStartIndex ,
			iStartIndex + 1,
			iStartIndex + 2,
			iStartIndex + 3
		};

		pIndices[iIndex++] = Indicies[1];
		pIndices[iIndex++] = Indicies[2];
		pIndices[iIndex++] = Indicies[0];

		pIndices[iIndex++] = Indicies[1];
		pIndices[iIndex++] = Indicies[3];
		pIndices[iIndex++] = Indicies[2];
	}

	D3D11_SUBRESOURCE_DATA InitialIBDate{};
	InitialIBDate.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBDate, &m_pIB))) {
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);

	return S_OK;
}

void CTrail::Trail_Update(_float fDeltaTime, _fmatrix WorldMatrix)
{
	

	_matrix WorldMat = {};

	WorldMat.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMat.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMat.r[2] = XMVector3Normalize(WorldMatrix.r[2]);
	WorldMat.r[3] = WorldMatrix.r[3];

	_vector vLow = XMVector3TransformCoord(XMLoadFloat3(&m_TrailDesc.vLow), WorldMat);
	_vector vHigh = XMVector3TransformCoord(XMLoadFloat3(&m_TrailDesc.vHigh), WorldMat);


	//한칸씩 밀기
	for (_uint i = 0; i < 3; i++)
	{
		m_PreLow[i] = m_PreLow[i + 1];
		m_PreHigh[i] = m_PreHigh[i + 1];
	}


	XMStoreFloat3(&m_PreLow[3], vLow);
	XMStoreFloat3(&m_PreHigh[3], vHigh);

	if (m_iNumCount < 12)
	{
		XMStoreFloat3(&m_pVertices[m_iNumCount++].vPosition, vLow);
		XMStoreFloat3(&m_pVertices[m_iNumCount++].vPosition, vHigh); 

		//카운트가 적으면 시도하지 않음
		return;
	}

	//시작점 끝점 내점 두개를 이용하여 보간할 것임

	_vector m_PreLowVector[4] = { XMLoadFloat3(&m_PreLow[0]) , XMLoadFloat3(&m_PreLow[1]) ,  XMLoadFloat3(&m_PreLow[2]),  XMLoadFloat3(&m_PreLow[3]) };
	_vector m_PreHighVector[4] = { XMLoadFloat3(&m_PreHigh[0]) , XMLoadFloat3(&m_PreHigh[1]),XMLoadFloat3(&m_PreHigh[2]) , XMLoadFloat3(&m_PreHigh[3]) };

	for (size_t i = 0; i < 9; i++)
	{

		_float iRatio =  (_float)i / 8;
		_vector vLerpLow = XMVectorCatmullRom(m_PreLowVector[0], m_PreLowVector[1], m_PreLowVector[2], m_PreLowVector[3], iRatio); // LoW
		_vector vLerpHigh = XMVectorCatmullRom(m_PreHighVector[0], m_PreHighVector[1], m_PreHighVector[2], m_PreHighVector[3], iRatio);

		XMStoreFloat3(&m_pVertices[m_iNumCount++].vPosition, vLerpLow);
		XMStoreFloat3(&m_pVertices[m_iNumCount++].vPosition, vLerpHigh);

		if (m_iNumCount >= m_iNumVertices)
		{
			memmove(m_pVertices, m_pVertices + 2, sizeof(VTXPOSTEX) * (m_iNumCount - 2));

			m_iNumCount -= 2;
		}

	}

	for (_uint i = 0; i < m_iNumCount; i += 2)
	{

		_float u = ((_float)i / (m_iNumCount - 2));

		if (m_iNumCount - 2 <= 0)
			u = 0;

		m_pVertices[i].vTexcoord = _float2(u, 0); // Low

		m_pVertices[i + 1].vTexcoord = _float2(u, 1); // High


	}


	D3D11_MAPPED_SUBRESOURCE		VBResource{};

	if (SUCCEEDED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &VBResource)))
	{

		memcpy(VBResource.pData, m_pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

		VTXPOSTEX* pDebug = static_cast<VTXPOSTEX*>(VBResource.pData);
		
		m_pContext->Unmap(m_pVB, 0);
	}

}

void CTrail::Rope_Trail_Update(_fmatrix WorldMatrix, _float fTimeDelta, _float fDamping, _float fLength , _float fMass , _fmatrix EndWorldMatrix)
{

	if (m_isFix == false)
	{
		if (m_iNumCount > 2)
		{
			memmove(m_pVertices + 2, m_pVertices, sizeof(VTXPOSTEX) * (m_iNumCount - 2));
			memmove(m_pOldPosition + 2, m_pOldPosition, sizeof(_float3) * (m_iNumCount - 2));
		}

		if (m_iNumCount >= m_iNumVertices)
			m_iNumCount -= 2;
	}

	m_pOldPosition[0] = m_pVertices[0].vPosition;
	m_pOldPosition[1] = m_pVertices[1].vPosition;


	_matrix WorldMat = {};

 	WorldMat.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMat.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMat.r[2] = XMVector3Normalize(WorldMatrix.r[2]);
	WorldMat.r[3] = WorldMatrix.r[3];

	_vector vLow = XMVector3TransformCoord(XMLoadFloat3(&m_TrailDesc.vLow), WorldMat);
	_vector vHigh = XMVector3TransformCoord(XMLoadFloat3(&m_TrailDesc.vHigh), WorldMat);

	/* 시작 시 고정점 업데이트 */
	XMStoreFloat3(&m_pVertices[0].vPosition, vLow);
	XMStoreFloat3(&m_pVertices[1].vPosition, vHigh);

	if (m_isFix == false)
		m_iNumCount += 2;

	_matrix EndWorldMat = {};

	EndWorldMat.r[0] = XMVector3Normalize(EndWorldMatrix.r[0]);
	EndWorldMat.r[1] = XMVector3Normalize(EndWorldMatrix.r[1]);
	EndWorldMat.r[2] = XMVector3Normalize(EndWorldMatrix.r[2]);
	EndWorldMat.r[3] = EndWorldMatrix.r[3];

	_vector vEndLow = XMVector3TransformCoord(XMLoadFloat3(&m_TrailDesc.vLow), EndWorldMat);
	_vector vEndHigh = XMVector3TransformCoord(XMLoadFloat3(&m_TrailDesc.vHigh), EndWorldMat);

	XMStoreFloat3(&m_pVertices[m_iNumCount - 2].vPosition, vEndLow);
	XMStoreFloat3(&m_pVertices[m_iNumCount - 1].vPosition, vEndHigh);

	m_pOldPosition[m_iNumCount - 2] = m_pVertices[m_iNumCount - 2].vPosition;
	m_pOldPosition[m_iNumCount - 1] = m_pVertices[m_iNumCount - 1].vPosition;

	for (_uint i = 2; i < m_iNumCount; i++) /* 고정점은 연산에서 제외 0 , 1*/
	{

		_vector vCurrentPos = XMLoadFloat3(&m_pVertices[i].vPosition);
		vCurrentPos = XMVectorSetW(vCurrentPos, 1.f);


		_vector vOldPos = XMLoadFloat3(&m_pOldPosition[i]);
		vOldPos = XMVectorSetW(vOldPos, 1.f);

		if (XMVectorGetX(XMVector3Length(vOldPos)) <= 0)
			continue;

		_vector vVelocity = (vCurrentPos - vOldPos) * fDamping;

		_vector vGravity = XMVectorSet(0.0f, m_fGravity, 0.0f, 0.0f);

		// Verlet 공식: Next = Curr + Vel + Acc * dt * dt
		_vector vNextPos = vCurrentPos + vVelocity + vGravity * (fTimeDelta * fTimeDelta);

		// 과거 위치 갱신
		XMStoreFloat3(&m_pOldPosition[i], XMLoadFloat3(&m_pVertices[i].vPosition));

		// 현재 위치 갱신
		XMStoreFloat3(&m_pVertices[i].vPosition, vNextPos);

	}

	for (int i = 0; i < 20; ++i) {
		for (_uint k = 0; k < m_iNumCount - 2; k += 2) {

			/* Low , High */
			_vector vFirst[2] = { XMLoadFloat3(&m_pVertices[k].vPosition) , XMLoadFloat3(&m_pVertices[k + 1].vPosition) };
			_vector vNext[2] = { XMLoadFloat3(&m_pVertices[k + 2].vPosition) , XMLoadFloat3(&m_pVertices[k + 3].vPosition) };


			_vector vDeltaLow = vNext[0] - vFirst[0];
			_vector vDeltaHigh = vNext[1] - vFirst[1];


			_float fLowDist = XMVectorGetX(XMVector3Length(vDeltaLow));
			_float fHighDist = XMVectorGetX(XMVector3Length(vDeltaHigh));

			// 거리가 0이면 오류 방지
			if (fLowDist < FLT_EPSILON5) continue;
			if (fHighDist < FLT_EPSILON5) continue;

			// 늘어나거나 줄어든 비율 계산 (Difference)
			_float fLowDiff = (fLength - fLowDist) / fLowDist;
			_float fHighDiff = (fLength - fHighDist) / fHighDist;

			// 각 점을 절반씩 밀거나 당겨서  거리 맞춤

			_vector vLowOffset = vDeltaLow * (fMass * fLowDiff);
			_vector vHighOffset = vDeltaHigh * (fMass * fHighDiff);

			// k!=0일 때만 vFirst를 움직임

 			if (k != 0)
			{
				vFirst[0] -= vLowOffset;
				vFirst[1] -= vHighOffset;
			}

			// vNext는 모든 경우에 움직임
			vNext[0] += vLowOffset;
			vNext[1] += vHighOffset;


			// k=0일 때 vFirst (손잡이)가 움직이지 않은 몫을 vNext에게 전부 몰아줌

			if (k == 0)
			{
				// vFirst가 움직여야 할 만큼 vNext에 추가 보정
				vNext[0] += vLowOffset;
				vNext[1] += vHighOffset;
			}

			if (k == m_iNumCount - 4)
			{

				vFirst[0] -= vLowOffset;
				vFirst[1] -= vHighOffset;
			}



			if (k != 0)
			{
				XMStoreFloat3(&m_pVertices[k].vPosition, vFirst[0]);
				XMStoreFloat3(&m_pVertices[k + 1].vPosition, vFirst[1]);
			}

			// vNext 저장
			XMStoreFloat3(&m_pVertices[k + 2].vPosition, vNext[0]);
			XMStoreFloat3(&m_pVertices[k + 3].vPosition, vNext[1]);
		}
	}

	XMStoreFloat3(&m_pVertices[m_iNumCount - 2].vPosition, vEndLow);
	XMStoreFloat3(&m_pVertices[m_iNumCount - 1].vPosition, vEndHigh);

	for (_uint i = 0; i < m_iNumCount; i += 2)
	{

		_float u = ((_float)i / (m_iNumCount - 2));

		if (m_iNumCount - 2 <= 0)
			u = 0;

		m_pVertices[i].vTexcoord = _float2(u, 0); // Low

		m_pVertices[i + 1].vTexcoord = _float2(u, 1); // High


	}


	D3D11_MAPPED_SUBRESOURCE		VBResource{};

	if (SUCCEEDED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &VBResource)))
	{

		memcpy(VBResource.pData, m_pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

		VTXPOSTEX* pDebug = static_cast<VTXPOSTEX*>(VBResource.pData);

		m_pContext->Unmap(m_pVB, 0);
	}
}

void CTrail::Reset_Trail()
{
	ZeroMemory(m_pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);
	ZeroMemory(m_pOldPosition, sizeof(_float3) * m_iNumVertices);
	ZeroMemory(&m_PreHigh, sizeof(_vector) * 2);
	ZeroMemory(&m_PreLow, sizeof(_vector) * 2);

	m_iNumCount = 0;
	m_fAccTime = 0.f;
	m_isFix = false;
 }

HRESULT CTrail::ReStructVB(_uint iNumVertices)
{
	m_iNumVertices = iNumVertices;

	if (FAILED(Create_VB()))
		return E_FAIL;

	if (FAILED(Create_IB()))
		return E_FAIL;

	Safe_Delete_Array(m_pVertices);
	Safe_Delete_Array(m_pOldPosition);

	m_pOldPosition = new _float3[m_iNumVertices];
	m_pVertices = new VTXPOSTEX[m_iNumVertices]{};
	
	return S_OK;
}

#ifdef _DEBUG
HRESULT CTrail::Save_Trail(HANDLE hFile)
{
	DWORD	dwByte(0);

	if (!WriteFile(hFile, &m_TrailDesc, sizeof(TRAIL_DESC), &dwByte, nullptr)) {
		return E_FAIL;
	}

	return S_OK;
}
#endif

HRESULT CTrail::Load_Trail(TRAIL_DESC TrailDesc)
{
	m_TrailDesc = TrailDesc;
	return S_OK;
}

HRESULT CTrail::Load_Trail(HANDLE hFile)
{
	DWORD	dwByte(0);

	if (!ReadFile(hFile, &m_TrailDesc, sizeof(TRAIL_DESC), &dwByte, nullptr)) {
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CTrail::Render()
{
	if (m_iNumCount < 4)
		return S_OK;

	m_pContext->DrawIndexed( ((m_iNumCount / 2) - 1) * 6, 0, 0);

	return S_OK;
}

CTrail* CTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrail* pInstance = new CTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrail");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CTrail* CTrail::Clone(void* pArg, CGameObject* pOwner)
{
	CTrail* pInstance = new CTrail(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrail");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTrail::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pVertices);
	Safe_Delete_Array(m_pOldPosition);
}
#ifdef _DEBUG

void CTrail::Describe_Entity()
{

	if (GUI::TreeNode("Trail Component")) {

		GUI::InputFloat3("Low", (_float*)&m_TrailDesc.vLow);
		GUI::InputFloat3("High", (_float*)&m_TrailDesc.vHigh);

		GUI::DragFloat("Gravity", &m_fGravity);

		GUI::Checkbox("Fix", &m_isFix);

		GUI::TreePop();
	}

}

#endif // _DEBUG
