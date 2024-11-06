#include "main.h"

#include "DXUTgui.h"
#include "SDKmisc.h"

extern GraphicResources * G;

extern SwapChainGraphicResources * SCG;

extern SceneState scene_state;

extern CDXUTTextHelper*                    g_pTxtHelper;

extern float waterTranslation;

ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

float waterHeight = 2.75f;

inline void set_scene_state(ID3D11DeviceContext* context){
	G->scene_constant_buffer->SetData(context, scene_state);
};

inline void set_water_plane(ID3D11DeviceContext* context, int enable, SimpleMath::Vector4& waterPlane = SimpleMath::Vector4()){
	WaterPlane p;
	p.plane = enable == 1 ? waterPlane : SimpleMath::Vector4(0,1,0,0);
	p.enable = enable;

	G->water_plane_buffer->setElement(0, p);
	G->water_plane_buffer->applyChanges(context);
};

inline void set_water_translation(ID3D11DeviceContext* context, float translation){
	WaterPlane p;
	p.waterTranslation = translation;

	G->water_plane_buffer->setElement(0, p);
	G->water_plane_buffer->applyChanges(context);
};

inline SimpleMath::Matrix get_view(){
	return SimpleMath::Matrix(scene_state.mView).Transpose();
};

inline void set_view(SimpleMath::Matrix & view){
	scene_state.mView = view.Transpose();
};

inline SimpleMath::Matrix make_reflection_view(DirectX::XMFLOAT4 plane){
	auto reflection = DirectX::XMMatrixReflect(XMLoadFloat4(&plane));

	auto invView = XMMatrixTranspose(XMLoadFloat4x4(&scene_state.mInvView));

	DirectX::XMMATRIX reflection_inv_view;
	reflection_inv_view.r[0] = -1 * XMVector4Transform(invView.r[0], reflection);
	reflection_inv_view.r[1] = XMVector4Transform(invView.r[1], reflection);
	reflection_inv_view.r[2] = XMVector4Transform(invView.r[2], reflection);
	reflection_inv_view.r[3] = XMVector4Transform(invView.r[3], reflection);

	auto reflection_view = DirectX::XMMatrixInverse(0, reflection_inv_view);

	return reflection_view;
};

void RenderText()
{
	g_pTxtHelper->Begin();
	g_pTxtHelper->SetInsertionPos(2, 0);
	g_pTxtHelper->SetForegroundColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	g_pTxtHelper->DrawTextLine(DXUTGetFrameStats(true && DXUTIsVsyncEnabled()));
	g_pTxtHelper->DrawTextLine(DXUTGetDeviceStats());

	g_pTxtHelper->End();
}

void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	Camera::OnFrameMove(fTime, fElapsedTime, pUserContext);
}

void renderSceneWithOutWater(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);
void renderWater(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, SimpleMath::Matrix & reflection_view);

void clearAndSetRenderTarget(ID3D11DeviceContext* context, float ClearColor[], ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV){

	context->ClearRenderTargetView(pRTV, ClearColor);
	context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->OMSetRenderTargets(1, renderTargetViewToArray(pRTV), pDSV);
}

void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* context,
	double fTime, float fElapsedTime, void* pUserContext)
{
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	auto waterPlane = SimpleMath::Vector4(0.0f, -1.0f, 0.0f, waterHeight + 0.0f);

	auto camera_view = get_view();

	auto reflection_view = make_reflection_view(waterPlane);

	auto refraction_view = camera_view;

	{
		clearAndSetRenderTarget(context, clearColor, SCG->colorLayerV[0].Get(), DXUTGetD3D11DepthStencilView());

		set_water_plane(context, 1, -1 * waterPlane);

		set_view(reflection_view);

		renderSceneWithOutWater(pd3dDevice, context);
	}
	{
		clearAndSetRenderTarget(context, clearColor, SCG->colorLayerV[1].Get(), DXUTGetD3D11DepthStencilView());

		set_water_plane(context, 1, 1 * waterPlane);

		set_view(refraction_view);

		renderSceneWithOutWater(pd3dDevice, context);
	}
	{
		context->PSSetShaderResources(0, 5, null);
	}
	{
		clearAndSetRenderTarget(context, clearColor, DXUTGetD3D11RenderTargetView(), DXUTGetD3D11DepthStencilView());

		set_water_plane(context, 0);

		set_view(camera_view);

		renderSceneWithOutWater(pd3dDevice, context);

		renderWater(pd3dDevice, context, reflection_view);
	}
	{
		context->PSSetShaderResources(0, 5, null);
	}

	RenderText();
}

void renderWater(ID3D11Device* pd3dDevice, ID3D11DeviceContext* context, SimpleMath::Matrix & reflection_view)
{
	/////
	context->VSSetConstantBuffers(0, 1, constantBuffersToArray(*(G->scene_constant_buffer)));

	context->PSSetConstantBuffers(0, 1, constantBuffersToArray(*(G->scene_constant_buffer)));

	context->PSSetSamplers(0, 1, samplerStateToArray(G->render_states->LinearWrap()));
	/////

	if (true){
		water_set_world_matrix(waterHeight);

		water_set_object1_matrix(reflection_view);

		set_water_translation(context, waterTranslation);

		set_scene_state(context);

		water_draw(context, G->water_effect.get(), 0, [=]{
			context->PSSetShaderResources(1, 4, shaderResourceViewToArray(G->water_plane_buffer->getView().asShaderView(), G->water_texture.Get(), SCG->colorLayerSRV[0].Get(), SCG->colorLayerSRV[1].Get()));

			context->OMSetBlendState(G->render_states->Opaque(), Colors::Black, 0xFFFFFFFF);
			context->RSSetState(G->render_states->CullCounterClockwise());
			context->OMSetDepthStencilState(G->render_states->DepthDefault(), 0);
		});
	}
}

void renderSceneWithOutWater(ID3D11Device* pd3dDevice, ID3D11DeviceContext* context)
{
	/////
	context->VSSetConstantBuffers(0, 1, constantBuffersToArray(*(G->scene_constant_buffer)));

	context->PSSetConstantBuffers(0, 1, constantBuffersToArray(*(G->scene_constant_buffer)));

	context->VSSetShaderResources(1, 1, shaderResourceViewToArray(G->water_plane_buffer->getView().asShaderView()));

	context->PSSetShaderResources(0, 1, shaderResourceViewToArray(G->light_buffer->getView().asShaderView()));

	context->PSSetSamplers(0, 1, samplerStateToArray(G->render_states->AnisotropicClamp()));
	/////

	if (true){
		bath_set_world_matrix();

		set_scene_state(context);

		bath_draw(context, G->model_effect.get(), G->model_input_layout.Get(), [=]{
			context->PSSetShaderResources(2, 1, shaderResourceViewToArray(G->bath_texture.Get()));

			context->OMSetBlendState(G->render_states->Opaque(), Colors::Black, 0xFFFFFFFF);
			context->RSSetState(G->render_states->CullCounterClockwise());
			context->OMSetDepthStencilState(G->render_states->DepthDefault(), 0);
		});
	}
	if (true){
		ground_set_world_matrix();

		set_scene_state(context);

		ground_draw(context, G->model_effect.get(), G->model_input_layout.Get(), [=]{
			context->PSSetShaderResources(2, 1, shaderResourceViewToArray(G->ground_texture.Get()));

			context->OMSetBlendState(G->render_states->Opaque(), Colors::Black, 0xFFFFFFFF);
			context->RSSetState(G->render_states->CullCounterClockwise());
			context->OMSetDepthStencilState(G->render_states->DepthDefault(), 0);
		});
	}
	if (true){
		wall_set_world_matrix();

		set_scene_state(context);

		wall_draw(context, G->model_effect.get(), G->model_input_layout.Get(), [=]{
			context->PSSetShaderResources(2, 1, shaderResourceViewToArray(G->wall_texture.Get()));

			context->OMSetBlendState(G->render_states->Opaque(), Colors::Black, 0xFFFFFFFF);
			context->RSSetState(G->render_states->CullCounterClockwise());
			context->OMSetDepthStencilState(G->render_states->DepthDefault(), 0);
		});
	}
}