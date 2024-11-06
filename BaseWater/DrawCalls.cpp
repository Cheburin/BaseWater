#include "main.h"

extern GraphicResources * G;

extern SceneState scene_state;

using namespace SimpleMath;

void loadMatrix_VP(Matrix & v, Matrix & p){
	v = Matrix(scene_state.mView).Transpose();
	p = Matrix(scene_state.mProjection).Transpose();
}
void loadMatrix_WP(Matrix & w, Matrix & p){
	w = Matrix(scene_state.mWorld).Transpose();
	p = Matrix(scene_state.mProjection).Transpose();
}
void storeMatrix(Matrix & w, Matrix & wv, Matrix & wvp){
	scene_state.mWorld = w.Transpose();
	scene_state.mWorldView = wv.Transpose();
	scene_state.mWorldViewProjection = wvp.Transpose();
}

void DrawQuad(ID3D11DeviceContext* pd3dImmediateContext, _In_ IEffect* effect,
	_In_opt_ std::function<void __cdecl()> setCustomState){
	effect->Apply(pd3dImmediateContext);
	setCustomState();

	pd3dImmediateContext->IASetInputLayout(nullptr);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pd3dImmediateContext->Draw(4, 0);
}

void bath_set_world_matrix(){
	Matrix w, v, p;
	loadMatrix_VP(v, p);

	w.Translation(Vector3(0.0f, 2.0f, 0.0f));
	Matrix  wv = w * v;
	Matrix wvp = wv * p;

	storeMatrix(w, wv, wvp);
}
void bath_draw(ID3D11DeviceContext* pd3dImmediateContext, IEffect* effect, ID3D11InputLayout* inputLayout, _In_opt_ std::function<void __cdecl()> setCustomState){
	G->bath_model->Draw(pd3dImmediateContext, effect, inputLayout, [=]
	{
		setCustomState();
	});
}

void ground_set_world_matrix(){
	Matrix w, v, p;
	loadMatrix_VP(v, p);

	w.Translation(Vector3(0.0f, 1.0f, 0.0f));
	Matrix  wv = w * v;
	Matrix wvp = wv * p;

	storeMatrix(w, wv, wvp);
}
void ground_draw(ID3D11DeviceContext* pd3dImmediateContext, IEffect* effect, ID3D11InputLayout* inputLayout, _In_opt_ std::function<void __cdecl()> setCustomState){
	G->ground_model->Draw(pd3dImmediateContext, effect, inputLayout, [=]
	{
		setCustomState();
	});
}

void wall_set_world_matrix(){
	Matrix w, v, p;
	loadMatrix_VP(v, p);

	w.Translation(Vector3(0.0f, 6.0f, 8.0f));
	Matrix  wv = w * v;
	Matrix wvp = wv * p;

	storeMatrix(w, wv, wvp);
}
void wall_draw(ID3D11DeviceContext* pd3dImmediateContext, IEffect* effect, ID3D11InputLayout* inputLayout, _In_opt_ std::function<void __cdecl()> setCustomState){
	G->wall_model->Draw(pd3dImmediateContext, effect, inputLayout, [=]
	{
		setCustomState();
	});
}

void water_set_world_matrix(float waterHeight){
	Matrix w, v, p;
	loadMatrix_VP(v, p);

	w = Matrix::CreateScale(4) * Matrix::CreateTranslation(0.0f, waterHeight, 0.0f);
	Matrix  wv = w * v;
	Matrix wvp = wv * p;

	storeMatrix(w, wv, wvp);
}
void water_set_object1_matrix(Matrix v){
	Matrix w, p;
	loadMatrix_WP(w, p);

	Matrix  wvp = w * v * p;

	scene_state.mObject1WorldViewProjection = wvp.Transpose();
}
void water_draw(ID3D11DeviceContext* pd3dImmediateContext, IEffect* effect, ID3D11InputLayout* inputLayout, _In_opt_ std::function<void __cdecl()> setCustomState){
	DrawQuad(pd3dImmediateContext, effect, [=]
	{
		setCustomState();
	});
}