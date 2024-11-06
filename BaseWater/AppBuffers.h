#pragma once
struct SceneState
{
	DirectX::XMFLOAT4X4    mWorld;                         // World matrix
	DirectX::XMFLOAT4X4    mView;                          // View matrix
	DirectX::XMFLOAT4X4    mProjection;                    // Projection matrix
	DirectX::XMFLOAT4X4    mWorldViewProjection;           // WVP matrix
	DirectX::XMFLOAT4X4    mWorldView;                     // WV matrix
	DirectX::XMFLOAT4X4    mInvView;                       // Inverse of view matrix

	DirectX::XMFLOAT4X4    mObject1View;                // VP matrix
	DirectX::XMFLOAT4X4    mObject1WorldView;                       // Inverse of view matrix
	DirectX::XMFLOAT4X4    mObject1WorldViewProjection;                       // Inverse of view matrix

	DirectX::XMFLOAT4X4    mObject2View;                // VP matrix
	DirectX::XMFLOAT4X4    mObject2WorldView;                       // Inverse of view matrix
	DirectX::XMFLOAT4X4    mObject2WorldViewProjection;                       // Inverse of view matrix

	DirectX::XMFLOAT4      vFrustumNearFar;              // Screen resolution
	DirectX::XMFLOAT4      vFrustumParams;              // Screen resolution
	DirectX::XMFLOAT4      viewLightPos;                   //
};

#pragma pack (push, 1)
struct LightData
{
	DirectX::XMFLOAT3 position;
	unsigned int lightType;
	DirectX::XMFLOAT3 direction;
	float falloff;
	DirectX::XMFLOAT3 diffuseColor;
	float angle;
	DirectX::XMFLOAT3 ambientColor;
	unsigned int : 32;
	DirectX::XMFLOAT3 specularColor;
	unsigned int : 32;
};
#pragma pack (pop)

#pragma pack (push, 1)
struct WaterPlane
{
	DirectX::XMFLOAT4 plane;
	unsigned int enable;
	float waterTranslation;
	unsigned int : 32;
	unsigned int : 32;
};
#pragma pack (pop)