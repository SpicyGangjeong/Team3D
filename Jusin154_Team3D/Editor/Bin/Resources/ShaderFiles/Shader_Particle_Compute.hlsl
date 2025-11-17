
float PI = 3.141592;

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
    float2 vAniTime;
    
    float2 vAniIndex;
    float  fGravity;
    
    float3 vSinAmount;
};


StructuredBuffer<Particle> g_ParticleBufferInput : register(t0);
StructuredBuffer<ParticleValue> g_ParticleValueBufferInput : register(t1);

RWStructuredBuffer<Particle> g_VBInstanceOutput : register(u0);
RWStructuredBuffer<ParticleValue> g_ParticleValueOutput : register(u1);

cbuffer g_ConstantBuffer : register(b0) // b0 << 이 숫자와 컨스턴트 쉐이더 바인딩할때 인덱스가 동일해야함
{
    row_major matrix CamViewInvMatrix;
  
    bool isLoop; // 상수 버퍼에서 불값은 4바이트로 처리되기 때문에 반드시 클라에서는 int형으로 선언해라
    bool isDrop;
    bool isMoveForward;
    bool isSinWave;

    float fTimeDelta;
    float fPadding1; // 반드시 상수버퍼는 16바이트 배수로 만들어져야 한다.
    float fPadding2;
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
    
    // 라이프타임 움직임    
    particle.vLifeTime.x += fTimeDelta;
    
    if (particle.vLifeTime.x >= particle.vLifeTime.y)
    {
        if (isLoop == false)
        {
         
            
            return;
        }
        
        //초기화
        particle.vRight = particleValue.vOriginRight;
        particle.vUp = particleValue.vOriginUp;
        particle.vLook = particleValue.vOriginLook;
        particle.vTranslation = particleValue.vOriginTranslation;
        
        particle.vLifeTime.x = 0.f;
       
    }
  
   
    if (isMoveForward)
    {
        float4 vVelocity = vector(normalize(particleValue.vOriginLook.xyz) * particleValue.fSpeed, 1.f);
    
        particle.vTranslation += vVelocity * fTimeDelta;
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
        
    
    //아웃풋 대입
    
    g_VBInstanceOutput[iIndex] = particle;
    g_ParticleValueOutput[iIndex] = particleValue;

}
