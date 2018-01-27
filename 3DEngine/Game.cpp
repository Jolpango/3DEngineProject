#include"Game.h"
#include <cstdlib>

void Game::UpdateBuffer()
{
	struct BUFFER_DATA
	{
		XMMATRIX worldMatrix;
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
	};

	XMMATRIX worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	float y = sinf(pitch);
	float r = cosf(pitch);
	float z = r * cosf(yaw);
	float x = r * sinf(yaw);
	XMVECTOR camPosition = { cameraPosition.x, cameraPosition.y, cameraPosition.z };
	XMVECTOR lookAt = { cameraPosition.x + x, cameraPosition.y + y, cameraPosition.z + z };
	XMVECTOR up = { 0.0f, 1.0f, 0.0f };
	XMMATRIX viewMatrix = XMMatrixLookAtLH(camPosition, lookAt, up);
	XMMATRIX camProjection = XMMatrixPerspectiveFovLH(XM_PI * 0.45f, 640.0f / 480.0f, 0.1f, 20.0f);



	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	camProjection = XMMatrixTranspose(camProjection);

	BUFFER_DATA bufferData = { worldMatrix, viewMatrix, camProjection };

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.ByteWidth = sizeof(BUFFER_DATA);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &bufferData;
	device->CreateBuffer(&bufferDesc, &data, &constantBuffer);
	deviceContext->GSSetConstantBuffers(0, 1, &constantBuffer);
}

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
	this->constantBuffer = nullptr;
	cameraPosition = Vector3(0, 0, -2);
	yaw = 0.0f;
	pitch =  0.0f;
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

void Game::Update(float deltaTime) 
{
	auto kbState = this->keyboard->GetState();

	Vector3 move(0.0f, 0.0f, 0.0f);
	if (kbState.Escape)
		PostQuitMessage(0);
	if (kbState.D)
	{
		move.x += 3;
	}
	if (kbState.A)
	{
		move.x -= 3;
	}
	if (kbState.S)
	{
		move.z -= 3;
	}
	if (kbState.W)
	{
		move.z += 3;
	}

	Quaternion q = Quaternion::CreateFromYawPitchRoll(yaw, -pitch, 0.0f);
	move = Vector3::Transform(move, q);
	move *= deltaTime;
	cameraPosition += move;

	auto mouseState = this->mouse->GetState();

	if (mouseState.positionMode == Mouse::MODE_RELATIVE)
	{
		

		Vector3 delta = Vector3(float(mouseState.x), float(mouseState.y), 0.0f) * deltaTime * 5.0f;

		pitch -= delta.y;
		yaw += delta.x;

		// limit pitch to straight up or straight down
		// with a little fudge-factor to avoid gimbal lock
		float limit = XM_PI / 2.0f - 0.01f;
		pitch = std::fmax(-limit, pitch);
		pitch = std::fmin(+limit, pitch);

		// keep longitude in sane range by wrapping
		if (yaw > XM_PI)
		{
			yaw -= XM_PI * 2.0f;
		}
		else if (yaw < -XM_PI)
		{
			yaw += XM_PI * 2.0f;
		}
	}
	mouse->SetMode(mouseState.leftButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);


	UpdateBuffer();
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