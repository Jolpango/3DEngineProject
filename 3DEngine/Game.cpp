#include"Game.h"

Game::Game(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11VertexShader* vertexShader, ID3D11GeometryShader* geometryShader, ID3D11PixelShader* pixelShader, ID3D11Buffer* vertexBuffer, ID3D11InputLayout* vertexLayout, ID3D11RenderTargetView* backBufferRTV)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->vertexShader = vertexShader;
	this->geometryShader = geometryShader;
	this->pixelShader = pixelShader;
	this->vertexBuffer = vertexBuffer;
	this->vertexLayout = vertexLayout;
	this->backBufferRTV = backBufferRTV;
}

Game::~Game()
{
}

void Game::Initialize(HWND wndHandle)
{
	this->keyboard = std::make_unique<Keyboard>();
	this->mouse = std::make_unique<Mouse>();
	this->mouse->SetWindow(wndHandle);
}

void Game::Update() 
{
	auto kbState = this->keyboard->GetState();
	if (kbState.Escape)
		PostQuitMessage(0);

	auto mouseState = this->mouse->GetState();
}

void Game::Render() 
{
	// clear the back buffer to a deep blue
	float clearColor[] = { 0, 0, 0.1f, 1 };

	// use DeviceContext to talk to the API
	this->deviceContext->ClearRenderTargetView(this->backBufferRTV, clearColor);

	// specifying NULL or nullptr we are disabling that stage
	// in the pipeline
	this->deviceContext->VSSetShader(this->vertexShader, nullptr, 0);
	this->deviceContext->HSSetShader(nullptr, nullptr, 0);
	this->deviceContext->DSSetShader(nullptr, nullptr, 0);
	this->deviceContext->GSSetShader(this->geometryShader, nullptr, 0);
	this->deviceContext->PSSetShader(this->pixelShader, nullptr, 0);

	UINT32 vertexSize = sizeof(float) * 6;
	UINT32 offset = 0;
	// specify which vertex buffer to use next.
	this->deviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &vertexSize, &offset);

	// specify the topology to use when drawing
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// specify the IA Layout (how is data passed)
	this->deviceContext->IASetInputLayout(vertexLayout);

	// issue a draw call of 3 vertices (similar to OpenGL)
	deviceContext->Draw(6, 0);
}