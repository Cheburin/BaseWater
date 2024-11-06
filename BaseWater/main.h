#include "GeometricPrimitive.h"
#include "Effects.h"
#include "DirectXHelpers.h"
#include "Model.h"
#include "CommonStates.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "SimpleMath.h"

#include "DirectXMath.h"

#include "DXUT.h"

#include <wrl.h>

#include <map>
#include <algorithm>
#include <array>
#include <memory>
#include <assert.h>
#include <malloc.h>
#include <Exception>

#include "ConstantBuffer.h"

#include "AppBuffers.h"

#include "resourceview.h"

#include "UniformBuffer.h"

using namespace DirectX;

struct EffectShaderFileDef{
	WCHAR * name;
	WCHAR * entry_point;
	WCHAR * shader_ver;
};
class IPostProcess
{
public:
	virtual ~IPostProcess() { }

	virtual void __cdecl Process(_In_ ID3D11DeviceContext* deviceContext, _In_opt_ std::function<void __cdecl()> setCustomState = nullptr) = 0;
};

inline ID3D11RenderTargetView** renderTargetViewToArray(ID3D11RenderTargetView* rtv1, ID3D11RenderTargetView* rtv2 = 0, ID3D11RenderTargetView* rtv3 = 0){
	static ID3D11RenderTargetView* rtvs[10];
	rtvs[0] = rtv1;
	rtvs[1] = rtv2;
	rtvs[2] = rtv3;
	return rtvs;
};
inline ID3D11ShaderResourceView** shaderResourceViewToArray(ID3D11ShaderResourceView* rtv1, ID3D11ShaderResourceView* rtv2 = 0, ID3D11ShaderResourceView* rtv3 = 0, ID3D11ShaderResourceView* rtv4 = 0){
	static ID3D11ShaderResourceView* srvs[10];
	srvs[0] = rtv1;
	srvs[1] = rtv2;
	srvs[2] = rtv3;
	srvs[3] = rtv4;
	return srvs;
};
inline ID3D11Buffer** constantBuffersToArray(DirectX::ConstantBuffer<SceneState> &cb){
	static ID3D11Buffer* cbs[10];
	cbs[0] = cb.GetBuffer();
	return cbs;
};
inline ID3D11SamplerState** samplerStateToArray(ID3D11SamplerState* ss1, ID3D11SamplerState* ss2 = 0){
	static ID3D11SamplerState* sss[10];
	sss[0] = ss1;
	sss[1] = ss2;
	return sss;
};

namespace Camera{
	void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
}
std::unique_ptr<DirectX::IEffect> createHlslEffect(ID3D11Device* device, std::map<const WCHAR*, EffectShaderFileDef>& fileDef);

bool LoadModel(char* filename, std::vector<VertexPositionNormalTexture> & _vertices, std::vector<uint16_t> & _indices);
std::unique_ptr<DirectX::ModelMeshPart> CreateModelMeshPart(ID3D11Device* device, std::function<void(std::vector<VertexPositionNormalTexture> & _vertices, std::vector<uint16_t> & _indices)> createGeometry);

void bath_set_world_matrix();
void bath_draw(ID3D11DeviceContext* pd3dImmediateContext, IEffect* effect, ID3D11InputLayout* inputLayout, _In_opt_ std::function<void __cdecl()> setCustomState = nullptr);

void ground_set_world_matrix();
void ground_draw(ID3D11DeviceContext* pd3dImmediateContext, IEffect* effect, ID3D11InputLayout* inputLayout, _In_opt_ std::function<void __cdecl()> setCustomState = nullptr);

void wall_set_world_matrix();
void wall_draw(ID3D11DeviceContext* pd3dImmediateContext, IEffect* effect, ID3D11InputLayout* inputLayout, _In_opt_ std::function<void __cdecl()> setCustomState = nullptr);

void water_set_world_matrix(float waterHeight); 
void water_set_object1_matrix(SimpleMath::Matrix v);
void water_draw(ID3D11DeviceContext* pd3dImmediateContext, IEffect* effect, ID3D11InputLayout* inputLayout, _In_opt_ std::function<void __cdecl()> setCustomState = nullptr);

class GraphicResources {
public:
	std::unique_ptr<CommonStates> render_states;

	std::unique_ptr<DirectX::IEffect> water_effect;

	std::unique_ptr<DirectX::IEffect> model_effect;

	std::unique_ptr<DirectX::ConstantBuffer<SceneState> > scene_constant_buffer;

	std::unique_ptr<DirectX::ModelMeshPart> bath_model;

	std::unique_ptr<DirectX::ModelMeshPart> ground_model;

	std::unique_ptr<DirectX::ModelMeshPart> wall_model;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bath_texture;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ground_texture;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> wall_texture;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> water_texture;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> model_input_layout;

	std::unique_ptr<framework::UniformBuffer> light_buffer;

	std::unique_ptr<framework::UniformBuffer> water_plane_buffer;
};

class SwapChainGraphicResources {
public:
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> > depthStencilSRV;
	std::vector<Microsoft::WRL::ComPtr<ID3D11DepthStencilView> > depthStencilV;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D> > depthStencilT;

	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> > colorLayerSRV;
	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView> > colorLayerV;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D> > colorLayerT;
};