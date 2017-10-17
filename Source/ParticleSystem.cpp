
#include "ParticleSystem.h"
#include "ParticleCamera.h"
#include "CSGravity.h"
#include <D3D11.h>
#include <random>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define DXTRACE_MSG(x)

using namespace DirectX;

namespace {
	DXGI_FORMAT bitmapformats[] = {
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8G8_UNORM,
#ifdef USE_SRGB_TEXTURE
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
#else
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
#endif
	};
}


ParticleSystem::ParticleSystem(Input* input, LPCWSTR csFilePath, float quadLength, float velocityTranslate, float velocityRotate, int maxParticles) : D3D11Init(),
_input(input), _colorMap(0), _velocityTranslate(velocityTranslate), _velocityRotate(velocityRotate), _drawInformation(false),
_maxParticles(maxParticles), _alphaBlendState(), _quadLength(quadLength) {
	_gravity = new CSGravity(csFilePath, maxParticles);

}

ParticleSystem::~ParticleSystem() {}

bool ParticleSystem::initialize(HINSTANCE hInstance, HWND hWnd, int screenWidth, int screenHeight, int initRadius, bool enableDepthBuffer, bool windowed) {
	_camera = new ParticleCamera(screenWidth, screenHeight);
	_particleShader = new ParticleShader();

	bool initResult = D3D11Init::initialize(hInstance, hWnd, enableDepthBuffer, windowed);

	if (!initResult) {
		return initResult;
	}

	bool loadResult = loadContent(screenWidth, screenHeight, initRadius);

	if (!loadResult) {
		return loadResult;
	}

	return true;
}

HRESULT ParticleSystem::loadTexture(LPCWSTR name, ID3D11ShaderResourceView *& _out)
{
	HRESULT result = 0;

	char pString[4096];
	if (0 >= WideCharToMultiByte(CP_UTF8, 0, name, -1, pString, 4096, NULL, NULL))
		return E_INVALIDARG;

	FILE* fp = nullptr;
	if (0 != fopen_s(&fp, pString, "rb"))
		return E_INVALIDARG;

	int w = 0, h = 0, ch = 0;
	UCHAR *data = stbi_load_from_file(fp, &w, &h, &ch, 0);

	if (!data)
	{
		return E_INVALIDARG;
	}

	if (ch == 3) {

		UCHAR* dst = new UCHAR[w * h * 4];
		UCHAR* src = (UCHAR*)data;

		size_t x = w;
		size_t y = h;

		size_t stride_src = x * 3;
		size_t stride_dst = x * 4;

		while (y--) {
			while (x--) {
				dst[4 * x + 0] = src[3 * x + 0];
				dst[4 * x + 1] = src[3 * x + 1];
				dst[4 * x + 2] = src[3 * x + 2];
				dst[4 * x + 3] = 0xff;
			}
			src += stride_src;
			dst += stride_dst;
			x = w;
		}


		delete[] data;
		data = dst;
	}

	DXGI_FORMAT format = bitmapformats[ch % 5];

	int miplevels = floor(log2f(w));
	if (miplevels > 16)
		return E_INVALIDARG;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = w;
	textureDesc.Height = w;
	textureDesc.MipLevels = miplevels;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	
	D3D11_SUBRESOURCE_DATA pData[16]; // 2^16 max size, sry

	pData[0].pSysMem = data;
	pData[0].SysMemSlicePitch = 0;
	pData[0].SysMemPitch = w * ch;

	size_t s = w * h * ch * 1;
	size_t ww = w;

	for (int i = 1; i < miplevels; i++) {
		s = s / 2;
		ww = ww / 2;
		unsigned char *data = new unsigned char[s];
		pData[i].pSysMem = data;
		pData[i].SysMemPitch = ww * 1;
		pData[i].SysMemSlicePitch = 0;
	}

	ID3D11Texture2D *ppTex = nullptr;
	result = _device->CreateTexture2D(&textureDesc, pData, &ppTex);

	if (FAILED(result)) 
		return result;

	// SRV

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	shaderResourceViewDesc.Texture1D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture1D.MipLevels = -1;

	result = _device->CreateShaderResourceView(ppTex, &shaderResourceViewDesc, &_out);

	if (FAILED(result))
		return result;

	delete[] data;

	return S_OK;
}

bool ParticleSystem::loadContent(int screenWidth, int screenHeight, int initRadius)
{

#if 0
	///////////////////////////////////////Creating font object for drawing text//////////////////////////////////////
	IFW1Factory* pFW1Factory;
	FW1CreateFactory(FW1_VERSION, &pFW1Factory);

	pFW1Factory->CreateFontWrapper(_device, L"Arial", &_fontWrapper);
	pFW1Factory->Release();
#endif 

	///////////////////////////////////////Creating projection matrix/////////////////////////////////////////////////
	XMMATRIX projMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)(screenWidth) / (float)(screenHeight), 0.01f, 1000.0f);
	projMatrix = XMMatrixTranspose(projMatrix);

	///////////////////////////////////////Loading Texture File for the Particle//////////////////////////////////////
	HRESULT result = loadTexture(L".\\particle.png", _colorMap);

	if (FAILED(result)) {
		MessageBox(NULL, "Error loading texture!", "Error", 0);
		DXTRACE_MSG("Failed to load the texture image!");
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	blendDesc.IndependentBlendEnable = false;
	blendDesc.AlphaToCoverageEnable = true;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	_device->CreateBlendState(&blendDesc, &_alphaBlendState);

	///////////////////////////////////////Loading the computer shader/////////////////////////////////////////////////
	////////////////////////////////////////////and the particles////////////////////(/////////////////////////////////

	return (loadParticles(initRadius) && _particleShader->initializeShader(_device, projMatrix, _quadLength));
}

bool ParticleSystem::loadParticles(int initRadius) {
	Particle* particles = new Particle[_maxParticles];
	float rndX, rndY, rndZ, length;
	std::tr1::mt19937 eng;
	std::tr1::uniform_real_distribution<float> dist(-initRadius, initRadius);

	for (int i = 0; i < _maxParticles; ++i) {
		rndX = dist(eng);
		rndY = dist(eng);
		rndZ = dist(eng);
		particles[i]._currPosition = XMFLOAT3(rndX, rndY, rndZ);
		particles[i]._prevPosition = XMFLOAT3(rndX, rndY, rndZ);
	}

	bool gravityInitializeResult = _gravity->initialize(_device, particles, _maxParticles);
	delete[] particles;

	return gravityInitializeResult;
}

bool ParticleSystem::unloadContent() {

	if (_colorMap)		_colorMap->Release();
	if (_alphaBlendState)_alphaBlendState->Release();
	//if(_fontWrapper) _fontWrapper->Release();
	delete _camera;
	delete _particleShader;
	delete _gravity;
	delete _input;

	return true;
}

void ParticleSystem::update(double frameTimeDiff, double time) {
	float x = 0;
	float y = 0;
	float z = 0;
	float p = 0;
	float yaw = 0;
	static bool tabHold = false;

	_input->updateInput();

	BYTE* keys = _input->getKeyboardState();

#if 0

	if (keys[DIK_END]) {
		_camera->resetCamera();
	}

	if (!keys[DIK_TAB]) {
		tabHold = false;
	}
	else if (keys[DIK_TAB] && !tabHold) {
		if (_drawInformation) {
			_drawInformation = false;
			tabHold = true;
		}
		else {
			_drawInformation = true;
			tabHold = true;
		}
	}

	if (keys[DIK_D]) {
		x += _velocityTranslate *  frameTimeDiff;
		_camera->moveX(x);
	}

	if (keys[DIK_A]) {
		x -= _velocityTranslate *  frameTimeDiff;
		_camera->moveX(x);
	}

	if (keys[DIK_R]) {
		y += _velocityTranslate *  frameTimeDiff;
		_camera->moveY(y);
	}

	if (keys[DIK_F]) {
		y -= _velocityTranslate *  frameTimeDiff;
		_camera->moveY(y);
	}

	if (keys[DIK_W]) {
		z += _velocityTranslate *  frameTimeDiff;
		_camera->moveZ(z);
	}

	if (keys[DIK_S]) {
		z -= _velocityTranslate *  frameTimeDiff;
		_camera->moveZ(z);
	}

#endif

	reinterpret_cast<ParticleCamera*>(_camera)->renderFreeLookCamera(_input->getMouseXAbsolute(), _input->getMouseYAbsolute(), _input->getMouseZAbsolute());

	XMMATRIX viewMatrix = _camera->getViewMatrix();
	viewMatrix = XMMatrixTranspose(viewMatrix);

	XMFLOAT3 attractor = XMFLOAT3(0, 0, 0);

	if (!_input->getRightMouseClick()) {
		p += _input->getMouseYRelative() * _velocityRotate *  frameTimeDiff;
		_camera->pitch(p);

		yaw += _input->getMouseXRelative() * _velocityRotate * frameTimeDiff;
		_camera->yaw(yaw);
	}
	else {
		if (_input->getLeftMouseClick()) {
			attractor = reinterpret_cast<ParticleCamera*>(_camera)->getAttractor();
		}
	}

	_gravity->updateConstantBuffers(_devContext, static_cast<float>(frameTimeDiff), attractor);
	_particleShader->updateShader(_devContext, viewMatrix, _camera->getPosition(), time);

}

void ParticleSystem::render() {
	if (_devContext == 0) return;

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float blendFactor[4] = { 1.25f, 1.25f, 1.25f, 0.0f };

	_devContext->OMSetBlendState(_alphaBlendState, blendFactor, 0xFFFFFFFF);
	_devContext->ClearRenderTargetView(_backBufferTarget, clearColor);
	_devContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	_particleShader->setUpShader(_devContext);

	_gravity->update(_devContext);

	_devContext->PSSetShaderResources(0, 1, &_colorMap);

	_devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	_devContext->Draw(_maxParticles, 0);
	//_fontWrapper->DrawString(_devContext, _fps.c_str(), 16.0f, 0.0f, 0.0f, 0xff0099ff, FW1_RESTORESTATE);

	if (_drawInformation) {
		drawInformation();
	}

	_swapChain->Present(0, 0);
}

void ParticleSystem::drawInformation() {
	int  mousex = _input->getMouseXAbsolute();
	int mousey = -1 * _input->getMouseYAbsolute();
	int mousez = _input->getMouseZAbsolute();

	float  camxf = _camera->getPosition().x;
	float camyf = _camera->getPosition().y;
	float  camzf = _camera->getPosition().z;


	XMFLOAT3 attractor = reinterpret_cast<ParticleCamera*>(_camera)->getAttractor();

	float  attxf = attractor.x;
	float attyf = attractor.y;
	float  attzf = attractor.z;


	std::wstring mx, my, cam, att;
	std::wostringstream* printString = new std::wostringstream();
	*printString << "MouseCoor: " << mousex << "  " << mousey << "  " << mousez;
	mx = (*printString).str();	delete printString;

	printString = new std::wostringstream();
	*printString << "CamCoor: ";
	*printString << camxf << "  " << camyf << "  " << camzf;
	cam = (*printString).str(); delete printString;

	printString = new std::wostringstream();
	*printString << "AttCoor: ";
	*printString << attxf << "  " << attyf << "  " << attzf;
	att = (*printString).str(); delete printString;

	//_fontWrapper->DrawString(_devContext, mx.c_str(), NULL, 16.0f, 0.0f, 16.0f, 0xff0099ff, FW1_RESTORESTATE);
	//_fontWrapper->DrawString(_devContext, cam.c_str(), NULL, 16.0f, 0.0f, 32.0f, 0xff0099ff, FW1_RESTORESTATE);
	//_fontWrapper->DrawString(_devContext, att.c_str(), NULL, 16.0f, 0.0f, 48.0f, 0xff0099ff, FW1_RESTORESTATE);
}

void ParticleSystem::setFPSToDraw(int fps) {
	std::wostringstream printString;
	printString << "fps: " << fps;
	_fps = printString.str();

}
