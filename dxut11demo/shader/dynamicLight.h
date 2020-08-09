



interface iBaseLight
{
	float3 IlluminateAmbient(float3 vNormal);
	float3 IlluminateDiffuse(float3 vNormal);
	float3 IlluminateSpecular(float3 vNormal,int specularPower);
};

class cAmbientLight : iBaseLight
{
	float3 m_vLightColor;
	bool m_bEnable;
	float3 IlluminateAmbient(float3 vNormal);
	float3 IlluminateDiffuse(float3 vNormal)
	{
		return (float3)0;
	}
	float3 IlluminateSpecular(float3 vNormal,int specularPower)
	{
		return (float3)0;
	}
};

//���Դ
class cHemiAmbientLight : cAmbientLight
{
	float4 m_vGroundColor;		//�����
	float4 m_vDirUp;			//�ⷽ��Ĭ�ϴ�(0,1,0)��������������
	float3 IlluminateAmbient(float3 vNormal);
	
};


//�����
class cDirectionalLight : cAmbientLight
{
	float4 m_vLightDir;
   
   float3 IlluminateDiffuse( float3 vNormal );

   float3 IlluminateSpecular( float3 vNormal, int specularPower );
};

//����Դ
class cOmniLight : cAmbientLight
{
	float4 m_vLightPosition;
	float radius;
	float3 IlluminateDiffuse(float3 vNormal);
};



class cSpotLight : cAmbientLight
{
   float3   m_vLightPosition;
   float3   m_vLightDir;
};

class cEnvironmentLight : cAmbientLight
{
   float3  IlluminateSpecular( float3 vNormal, int specularPower );  
};



