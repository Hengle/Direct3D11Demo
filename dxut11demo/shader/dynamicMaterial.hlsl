


interface iBaseMaterial
{
	float3 GetAmbientColor(float2 vTexCoord);
	float3 GetDiffuseColor(float2 vTexCoord);
	int GetSpecularPower();
};


class cBaseMaterial : iBaseMaterial
{
	float3 m_vColor;
	int m_iSpecPower;
	 float3 GetAmbientColor(float2 vTexcoord)
   { 
      return m_vColor;
   }
      
   float3 GetDiffuseColor(float2 vTexcoord)
   { 
      return m_vColor;
   }

   int GetSpecularPower()
   { 
      return m_iSpecPower;
   }
};

class cPlasticMaterial : cBaseMaterial
{  

};

class cPlasticTexturedMaterial :cBaseMaterial
{  
   float3 GetAmbientColor(float2 vTexcoord);

   float3 GetDiffuseColor(float2 vTexcoord);

};

class cPlasticLightingOnlyMaterial : cBaseMaterial
{  
   float3 GetAmbientColor(float2 vTexcoord)
   { 
      return (float3)1.0f;
   }
      
   float3 GetDiffuseColor(float2 vTexcoord)
   { 
      return (float3)1.0f;
   }

};

class cRoughMaterial : cBaseMaterial
{
   int GetSpecularPower()
   { 
      return m_iSpecPower;
   }
};

class cRoughTexturedMaterial : cRoughMaterial
{  
   float3 GetAmbientColor(float2 vTexcoord);

   float3 GetDiffuseColor(float2 vTexcoord);

};


class cRoughLightingOnlyMaterial : cRoughMaterial
{
   float3 GetAmbientColor(float2 vTexcoord)
   { 
      return (float3)1.0f;
   }
      
   float3 GetDiffuseColor(float2 vTexcoord)
   { 
      return (float3)1.0f;
   }

};
