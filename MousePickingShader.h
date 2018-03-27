#ifndef MOUSEPICKINGSHADER_H
#define MOUSEPICKINGSHADER_H
#include "Application.h"

class MousePickingShader
{
private:
	__declspec(align(16))
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	ID3D11Buffer* m_vertexBuffer;
	ID3D11InputLayout* m_vertexLayout;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* _pPixelShader;
	ID3D11Buffer* _pMatrixBuffer;

	bool initializeShader(ID3D11Device* pDev, HWND hwnd, ID3D11Buffer* &vertexBuffer);
	void ReleaseShader();
	void outputErrorMessage(ID3D10Blob* blob, HWND hwnd, WCHAR* file);
	bool setShaderParameters(ID3D11DeviceContext* pDevCon, XMMATRIX world, XMMATRIX view, XMMATRIX projection);
	void renderShader(ID3D11DeviceContext* pDevCon, UINT stride);

public:
	MousePickingShader();
	virtual~MousePickingShader();

	ID3D11Buffer*& getVertexBuffer();

	bool initialize(ID3D11Device* pDev, HWND hwnd, ID3D11Buffer* &vertexBuffer);
	bool render(ID3D11DeviceContext* pDevCon, UINT stride, XMMATRIX world, XMMATRIX view, XMMATRIX projection);
	void release();
};

#endif