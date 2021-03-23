#ifndef _DEFAULTSHADER_H_
#define _DEFAULTSHADER_H_

#include "../DXFramework/BaseShader.h"

using namespace std;
using namespace DirectX;


class DefaultShader : public BaseShader
{

public:
	DefaultShader(ID3D11Device* device, HWND hwnd);
	~DefaultShader();
	void SetTexture(ID3D11ShaderResourceView* texture);
	void SetColour(float r, float g, float b);
	virtual void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sample_state_;
	ID3D11ShaderResourceView* texture_;

};

#endif