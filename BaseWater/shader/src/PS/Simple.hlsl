float4 LIGHTED_FRAG(
    in ClipPosPosNormalTex2dClipDist0 i
):SV_TARGET
{ 
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;
	
	textureColor = textureMap.Sample( linearSampler, i.tex );

	color = float4(lights[0].ambientColor, 1);
	
	lightDir = -lights[0].direction;

	lightIntensity = saturate(dot(normalize(i.normal), lightDir));
	
	if(lightIntensity > 0.0f)
		color += (float4(lights[0].diffuseColor, 1) * lightIntensity);

	color = saturate(color);
	
	color = color * textureColor;
	
	return color;
}

float4 WATER_FRAG(
   in ClipPosTex2dTex4d i
):SV_TARGET
{
   //return float4(i.tex2d.y,i.tex2d.y,i.tex2d.y,1);
   /////
   // Move the position the water normal is sampled from to simulate moving water.	
   i.tex2d.y += waterPlanes[0].waterTranslation;
   /////

   float4 reflProj = i.tex4d / i.tex4d.w;

   float2 reflUV = float2(reflProj.x * 0.5 + 0.5, reflProj.y * -0.5 + 0.5);
  
   float2 refrUV = float2(i.clip_pos.x / g_vFrustumParams.x, i.clip_pos.y / g_vFrustumParams.y);

   /////
   // Sample the normal from the normal map texture.
   float4 normalMap = textureMap.Sample(linearSampler, i.tex2d);

   // Expand the range of the normal from (0,1) to (-1,+1).
   float3 normal = (normalMap.xyz * 2.0f) - float3(1.0,1.0,1.0);   

   // Re-position the texture coordinate sampling position by the normal map value to simulate the rippling wave effect.
   reflUV = reflUV + (normal.xy * 0.01 /*reflectRefractScale*/);
   refrUV = refrUV + (normal.xy * 0.01 /*reflectRefractScale*/);   
   /////

   float3 reflColor = reflectionMap.Load( int2(reflUV.x * g_vFrustumParams.x, reflUV.y * g_vFrustumParams.y), 0 ).rgb;
   
   float3 refrColor = refractionMap.Load( int2(refrUV.x * g_vFrustumParams.x, refrUV.y * g_vFrustumParams.y), 0 ).rgb;//*float3(0.5,0.5,0);

   float3 color = lerp(reflColor, refrColor, 0.6f);

   return float4(color,1);
}
