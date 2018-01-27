#ifndef GAME_H
#define GAME_H

#include <memory>
#include "Pch.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
class Game
{
private:
	ID3D11Device * device;
	ID3D11DeviceContext* deviceContext;
	ID3D11VertexShader* vertexShader;
	ID3D11GeometryShader* geometryShader;
	ID3D11PixelShader* pixelShader;
	ID3D11Buffer* vertexBuffer;
	ID3D11InputLayout* vertexLayout;
	ID3D11RenderTargetView* backBufferRTV;
	ID3D11Buffer* constantBuffer;

	std::unique_ptr<DirectX::Keyboard> keyboard;
	std::unique_ptr<DirectX::Mouse> mouse;

	Vector3 cameraPosition;
	float pitch, yaw;

	void UpdateBuffer();

public:
	Game(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11VertexShader* vertexShader, ID3D11GeometryShader* geometryShader, ID3D11PixelShader* pixelShader, ID3D11Buffer* vertexBuffer, ID3D11InputLayout* vertexLayout, ID3D11RenderTargetView* backBufferRTV);
	~Game();

	void Initialize(HWND wndHandle);
	void Update(float deltaTime);
	void Render();
};

#endif // !GAME_H
