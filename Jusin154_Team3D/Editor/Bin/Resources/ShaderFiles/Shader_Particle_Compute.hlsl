#include "Engine_Shader_Functions.hlsli"

struct Particle
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;

    float2 vLifeTime;
};

struct ParticleValue
{
    float fSpeed;
    float fRotaionSpeed;

    float4 vOriginRight;
    float4 vOriginUp;
    float4 vOriginLook;
    float4 vOriginTranslation;
    
    float2 vMaskingUVMoveTime;
    float2 vDiffuseUVMoveTime;
    float2 vDistortionUVMoveTime;
    float2 vNoiseUVMoveTime;
    float2 vDissolveUVMoveTime;
    float2 vAniTime;

    
    float2 vAniIndex;
    float  fGravity;
    
    float3 vSinAmount;   
    float3 vDeltaAngle;
    float3 vDeltaAxisAngle;
    
    float  fDrag;
    float3 vPivot;
    
    float  fSizeDrag;
    float3 vDeltaSize;
    float2 vDelay;

};


StructuredBuffer<Particle> g_ParticleBufferInput : register(t0);
StructuredBuffer<ParticleValue> g_ParticleValueBufferInput : register(t1);

RWStructuredBuffer<Particle> g_VBInstanceOutput : register(u0);
RWStructuredBuffer<ParticleValue> g_ParticleValueOutput : register(u1);

cbuffer g_ConstantBuffer : register(b0) // b0 << 이 숫자와 컨스턴트 쉐이더 바인딩할때 인덱스가 동일해야함
{
    row_major matrix WorldMatrix;
  
    bool isLoop; // 상수 버퍼에서 불값은 4바이트로 처리되기 때문에 반드시 클라에서는 int형으로 선언해라
    bool isDrop;
    bool isMoveForward;
    bool isSinWave;
    
    bool isTurn;
    bool isAxisTurn;
    bool isPivotMove;
    bool isSizeMove;
    
    bool isNoWorld;
    bool isPadding0;
    bool isPadding1;
    bool isPadding2;

    float fTimeDelta;
    float fSizeLerpOption; // 반드시 상수버퍼는 16바이트 배수로 만들어져야 한다.
    float fMoveLerpOption;
    float fPadding3;
}

//내가 몇개의 스레드를 사용할 것인지 지정하는데
//GPU 칩의 개수를 X * Y * Z의 개수 만큼 사용하겠다는 의미이다.
[numthreads(256, 1, 1)]
void CS_MAIN(
  uint3 DispatchThreadID : SV_DispatchThreadID)
{
    int iIndex = DispatchThreadID.x; // 배열 인덱스 
    
    Particle particle = g_ParticleBufferInput[iIndex];
    
    ParticleValue particleValue = g_ParticleValueBufferInput[iIndex];
    
    particleValue.vDelay.x += fTimeDelta;
    
    if (particleValue.vDelay.x < particleValue.vDelay.y)
    {
      
        
        g_VBInstanceOutput[iIndex] = particle;
        g_ParticleValueOutput[iIndex] = particleValue;
        
        return;
    }
    
    if (particle.vLifeTime.x <= FLT_EPSILON5)
    {
        if (isNoWorld)
        {
            row_major float4x4 CurMat = { particle.vRight, particle.vUp, particle.vLook, particle.vTranslation };
        
       
            CurMat = mul(CurMat, WorldMatrix);
        
            particle.vRight = CurMat[0].xyzw;
            particle.vUp = CurMat[1].xyzw;
            particle.vLook = CurMat[2].xyzw;
            particle.vTranslation = CurMat[3].xyzw;
        }
    }
    
    // 라이프타임 움직임
    particle.vLifeTime.x += fTimeDelta;
    
    if (particle.vLifeTime.x >= particle.vLifeTime.y)
    {
        if (isLoop == false)
        {
            return ;
        }
        //초기화
        particle.vRight = particleValue.vOriginRight;
        particle.vUp = particleValue.vOriginUp;
        particle.vLook = particleValue.vOriginLook;
        particle.vTranslation = particleValue.vOriginTranslation;
        
        particle.vLifeTime.x = 0.f;
        particleValue.vAniIndex.x = 0.f;
        particleValue.vDelay.x = 0.f;
        

        
        g_VBInstanceOutput[iIndex] = particle;
        g_ParticleValueOutput[iIndex] = particleValue;
        
        return ;
    }
  
   
    if (isMoveForward)
    {
        float fTime = (particle.vLifeTime.x / particle.vLifeTime.y);

        float fRatio = 1 - (1 - fTime * fTime);
        
        float fDrag = particleValue.fDrag;
        
        if (particleValue.fDrag < FLT_EPSILON5)
            fDrag = 1;
        
        float4 vVelocity = vector(normalize(particle.vLook.xyz) * particleValue.fSpeed, 0.f);
    
        particle.vTranslation += vVelocity * fTimeDelta * fDrag;
    }
    
    if (isPivotMove)
    {
        float fTime = (particle.vLifeTime.x / particle.vLifeTime.y);
        
        float3 vDir = normalize(particleValue.vPivot - particleValue.vOriginTranslation.xyz);
        
        float fDrag = particleValue.fDrag;
        
        if (fSizeLerpOption != 0.f)
            fDrag = SelectLerpUV(float2(particleValue.fSizeDrag, 0.f), fTime, fMoveLerpOption).x;
        
        if (particleValue.fDrag < FLT_EPSILON5)
            fDrag = 1;
        
        float4 vVelocity = vector(vDir * particleValue.fSpeed , 0.f);
    
        particle.vTranslation += vVelocity * fTimeDelta * fDrag;
        
    }
   
    if (isDrop == true)
    {
        particle.vTranslation.y -= 0.5f * particle.vLifeTime.x * particleValue.fGravity;
    }
    
    /*  사인 웨이브 */
    if(isSinWave == true)
    {
        particle.vTranslation += vector( sin(particle.vLifeTime.x * 3.141592 * 2.f) * particleValue.vSinAmount, 0.f);
    }
    
    if (isTurn == true)
    {
        /* ROTATE */
        float4x4 RotateXMat = RotateX(particleValue.vDeltaAngle.x * fTimeDelta);
        float4x4 RotateYMat = RotateY(particleValue.vDeltaAngle.y * fTimeDelta);
        float4x4 RotateZMat = RotateZ(particleValue.vDeltaAngle.z * fTimeDelta);
       
        
        float4x4 CombinedRotMat = mul(RotateZMat, mul(RotateYMat, RotateXMat));
        
        float4x4 CurRotatemat = {particle.vRight, particle.vUp, particle.vLook, vector(0.f , 0.f, 0.f, 1.f)};
        
        
       
        CurRotatemat = mul(CombinedRotMat, CurRotatemat);
        
        particle.vRight = CurRotatemat[0].xyzw;
        particle.vUp = CurRotatemat[1].xyzw;
        particle.vLook = CurRotatemat[2].xyzw;
        
    }
    
    if (isAxisTurn)
    {
        float4x4 RotateAxisRightMat = RotateAxis(particle.vRight, particleValue.vDeltaAxisAngle.x * fTimeDelta);
        float4x4 RotateAxisUpMat = RotateAxis(particle.vUp, particleValue.vDeltaAxisAngle.y * fTimeDelta);
        float4x4 RotateAxisLookMat = RotateAxis(particle.vLook, particleValue.vDeltaAxisAngle.z * fTimeDelta);
        
         
        float4x4 CombinedRotMat = mul(RotateAxisLookMat, mul(RotateAxisUpMat, RotateAxisRightMat));
        
        float4x4 CurRotatemat = { particle.vRight, particle.vUp, particle.vLook, vector(0.f, 0.f, 0.f, 1.f) };
        
       
        CurRotatemat = mul(CurRotatemat ,CombinedRotMat);
        
        particle.vRight = CurRotatemat[0].xyzw;
        particle.vUp = CurRotatemat[1].xyzw;
        particle.vLook = CurRotatemat[2].xyzw;


    }
    
    if (isSizeMove == true)
    {
        float3 CurLength =
        {
            length(particle.vRight),
            length(particle.vUp),
            length(particle.vLook),
        };
        
        float fTime = (particle.vLifeTime.x / particle.vLifeTime.y);
        
        float fDrag = particleValue.fSizeDrag;
        
        if (fSizeLerpOption != 0.f)
            fDrag = SelectLerpUV(float2(particleValue.fSizeDrag, 0.f), fTime, fSizeLerpOption).x;
        
        if (particleValue.fSizeDrag < FLT_EPSILON5)
            fDrag = 1;
       
        
        CurLength += particleValue.vDeltaSize * fTimeDelta * fDrag;
        
        if (CurLength.x < FLT_EPSILON5)
            CurLength.x = FLT_EPSILON5;
        
        if (CurLength.y < FLT_EPSILON5)
            CurLength.y = FLT_EPSILON5;
        
        if (CurLength.z < FLT_EPSILON5)
            CurLength.z = FLT_EPSILON5;
     
        particle.vRight = normalize(particle.vRight) * CurLength.x;
        particle.vUp = normalize(particle.vUp) * CurLength.y;
        particle.vLook = normalize(particle.vLook) * CurLength.z;
        
    }
    
       
    //애니메이션 속도 , 인덱스
    
    particleValue.vAniTime.x += fTimeDelta;
    
    if (particleValue.vAniTime.x >= particleValue.vAniTime.y)
    {
        particleValue.vAniTime.x = 0.f;
        
        particleValue.vAniIndex.x += 1.f;
        
        if (particleValue.vAniIndex.x > particleValue.vAniIndex.y) // 애니메이션에 끝에 다다랐다면
        {
            particleValue.vAniIndex.x = 0.f; // 인덱스 초기화

        }

    }
    
    //마스킹 UV 움직임 시간 

    particleValue.vMaskingUVMoveTime.x += fTimeDelta;
    
    if (particleValue.vMaskingUVMoveTime.x >= particleValue.vMaskingUVMoveTime.y)
    {
        particleValue.vMaskingUVMoveTime.x = 0.f;
    }

    
    //디퓨즈 UV 움직임 
    
    particleValue.vDiffuseUVMoveTime.x += fTimeDelta;
    
    if (particleValue.vDiffuseUVMoveTime.x >= particleValue.vDiffuseUVMoveTime.y)
    {
        particleValue.vDiffuseUVMoveTime.x = 0.f;
    }
        
    //노이즈 움직임
    particleValue.vNoiseUVMoveTime.x += fTimeDelta;
    
    if (particleValue.vNoiseUVMoveTime.x >= particleValue.vNoiseUVMoveTime.y)
    {
        particleValue.vNoiseUVMoveTime.x = 0.f;
    }
    
    particleValue.vDistortionUVMoveTime.x += fTimeDelta;
    
    if (particleValue.vDistortionUVMoveTime.x >= particleValue.vDistortionUVMoveTime.y)
    {
        particleValue.vDistortionUVMoveTime.x = 0.f;
    }
    
    particleValue.vDissolveUVMoveTime.x += fTimeDelta;
    
    if (particleValue.vDissolveUVMoveTime.x >= particleValue.vDissolveUVMoveTime.y)
    {
        particleValue.vDissolveUVMoveTime.x = 0.f;
    }
        
    //아웃풋 대입
    
    g_VBInstanceOutput[iIndex] = particle;
    g_ParticleValueOutput[iIndex] = particleValue;

}
