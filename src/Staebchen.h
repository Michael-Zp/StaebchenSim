#pragma once

#include "RenderItem.h"
#include "ResetUtils.h"

class Staebchen : public RenderItem
{
public:
	Staebchen(ID3D11Device* device, XMFLOAT4 color);
	~Staebchen();

	void Draw(float deltaTime, ID3D11DeviceContext* context);

	void UpdateCamera(ID3D11DeviceContext* context, XMMATRIX view, XMMATRIX proj, XMFLOAT2 pos, float rot);

private:
	struct StaebchenVertexData
	{
		XMFLOAT3 Position;
		XMFLOAT4 Color;
	};

	struct StaebchenConstantBuffer
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
	};

	StaebchenConstantBuffer mConstantBufferData;
	UINT mIndexCount;

	bool mIsUpdated = false;
};

