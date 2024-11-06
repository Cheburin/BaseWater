#include "main.h"

#include "DXUTgui.h"
#include "SDKmisc.h"

HWND DXUTgetWindow();

GraphicResources * G;

SceneState scene_state;

std::unique_ptr<Keyboard> _keyboard;
std::unique_ptr<Mouse> _mouse;

CDXUTDialogResourceManager          g_DialogResourceManager;
CDXUTTextHelper*                    g_pTxtHelper = NULL;

#include <codecvt>

HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* device, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext)
{
	HRESULT hr;

	ID3D11DeviceContext* context = DXUTGetD3D11DeviceContext();

	G = new GraphicResources();
	G->render_states = std::make_unique<CommonStates>(device);
	G->scene_constant_buffer = std::make_unique<ConstantBuffer<SceneState> >(device);

	_keyboard = std::make_unique<Keyboard>();
	_mouse = std::make_unique<Mouse>();
	HWND hwnd = DXUTgetWindow();
	_mouse->SetWindow(hwnd);

	g_DialogResourceManager.OnD3D11CreateDevice(device, context);
	g_pTxtHelper = new CDXUTTextHelper(device, context, &g_DialogResourceManager, 15);

	//effects
	{
		std::map<const WCHAR*, EffectShaderFileDef> shaderDef;
		shaderDef[L"VS"] = { L"Quad.hlsl", L"BILLBOARD", L"vs_5_0" };
		shaderDef[L"PS"] = { L"Quad.hlsl", L"WATER_FRAG", L"ps_5_0" };

		G->water_effect = createHlslEffect(device, shaderDef);
	}
	{
		std::map<const WCHAR*, EffectShaderFileDef> shaderDef;
		shaderDef[L"VS"] = { L"Model.hlsl", L"LIGHTED_MODEL", L"vs_5_0" };
		shaderDef[L"PS"] = { L"Model.hlsl", L"LIGHTED_FRAG", L"ps_5_0" };

		G->model_effect = createHlslEffect(device, shaderDef);
	}

	//models
	{
		G->bath_model = CreateModelMeshPart(device, [=](std::vector<VertexPositionNormalTexture> & _vertices, std::vector<uint16_t> & _indices){
			LoadModel("models\\bath.txt", _vertices, _indices);
		});
		G->ground_model = CreateModelMeshPart(device, [=](std::vector<VertexPositionNormalTexture> & _vertices, std::vector<uint16_t> & _indices){
			LoadModel("models\\ground.txt", _vertices, _indices);
		});
		G->wall_model = CreateModelMeshPart(device, [=](std::vector<VertexPositionNormalTexture> & _vertices, std::vector<uint16_t> & _indices){
			LoadModel("models\\wall.txt", _vertices, _indices);
		});
	}

	//textures
	{
		hr = D3DX11CreateShaderResourceViewFromFile(device, L"models\\ground01.dds", NULL, NULL, G->ground_texture.ReleaseAndGetAddressOf(), NULL);
		hr = D3DX11CreateShaderResourceViewFromFile(device, L"models\\wall01.dds", NULL, NULL, G->wall_texture.ReleaseAndGetAddressOf(), NULL);
		hr = D3DX11CreateShaderResourceViewFromFile(device, L"models\\marble01.dds", NULL, NULL, G->bath_texture.ReleaseAndGetAddressOf(), NULL);
		hr = D3DX11CreateShaderResourceViewFromFile(device, L"models\\water01.dds", NULL, NULL, G->water_texture.ReleaseAndGetAddressOf(), NULL);
	}

	//uniform buffers
	{
		G->light_buffer = std::make_unique<framework::UniformBuffer>();

		if (!G->light_buffer->initDefaultStructured<LightData>(device, (size_t)1))
			return S_FALSE;

		LightData lightData;
		lightData.ambientColor = SimpleMath::Vector3(0.15f, 0.15f, 0.15f);
		lightData.diffuseColor = SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
		lightData.direction = SimpleMath::Vector3(0.0f, -1.0f, 0.5f);

		G->light_buffer->setElement(0, lightData);

		G->light_buffer->applyChanges(context);
		
		//
		
		G->water_plane_buffer = std::make_unique<framework::UniformBuffer>();

		if (!G->water_plane_buffer->initDefaultStructured<WaterPlane>(device, (size_t)1))
			return S_FALSE;
	}

	//input layouts
	{
		G->bath_model->CreateInputLayout(device, G->model_effect.get(), G->model_input_layout.ReleaseAndGetAddressOf());
	}

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice(void* pUserContext)
{

	delete g_pTxtHelper;

	g_DialogResourceManager.OnD3D11DestroyDevice();

	_mouse = 0;

	_keyboard = 0;

	delete G;
}
