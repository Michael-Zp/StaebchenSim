#include "Staebchen.h"


Staebchen::Staebchen(ID3D11Device* device, XMFLOAT4 color)
{
	GeometryGenerator::MeshData meshData;
	GeometryGenerator generator;

	generator.CreateBox(1, 0.05, 0.05, meshData);

	std::vector<StaebchenVertexData> vertexData;

	for (int i = 0; i < meshData.Vertices.size(); i++)
	{
		vertexData.push_back({
			meshData.Vertices[i].Position,
			color
			});
	}

	D3D11_BUFFER_DESC vertexDesc;
	vertexDesc.ByteWidth = sizeof(StaebchenVertexData) * vertexData.size();
	vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.CPUAccessFlags = 0;
	vertexDesc.MiscFlags = 0;
	vertexDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexSubData;
	vertexSubData.pSysMem = vertexData.data();

	HR(device->CreateBuffer(&vertexDesc, &vertexSubData, &mVertexBuffer));


	mIndexCount = meshData.Indices.size();
	D3D11_BUFFER_DESC indexDesc;
	indexDesc.ByteWidth = sizeof(UINT) * mIndexCount;
	indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.CPUAccessFlags = 0;
	indexDesc.MiscFlags = 0;
	indexDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexSubData;
	indexSubData.pSysMem = meshData.Indices.data();

	HR(device->CreateBuffer(&indexDesc, &indexSubData, &mIndexBuffer));


	D3D11_BUFFER_DESC constantBuffer;
	constantBuffer.ByteWidth = sizeof(StaebchenConstantBuffer);
	constantBuffer.Usage = D3D11_USAGE_DYNAMIC;
	constantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBuffer.MiscFlags = 0;
	constantBuffer.StructureByteStride = 0;

	HR(device->CreateBuffer(&constantBuffer, NULL, &mConstantBuffer));


	mVertexShader = new VertexShader(L"./Shader/vpHairBase.hlsl", "HairBaseVS", true);
	mVertexShader->addVertexDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 0);
	mVertexShader->addVertexDesc("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, 0);
	mVertexShader->prepare(device);

	mPixelShader = new PixelShader(L"./Shader/vpHairBase.hlsl", "HairBasePS", true);
	mPixelShader->prepare(device);
}


Staebchen::~Staebchen()
{

}

void Staebchen::Draw(float deltaTime, ID3D11DeviceContext* context)
{
	if (!mIsUpdated)
	{
		DebugBreak();
	}

	mVertexShader->activateInputLayout(context);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	mVertexShader->activate(context);
	mPixelShader->activate(context);

	UINT stride = sizeof(StaebchenVertexData);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->VSSetConstantBuffers(0, 1, &mConstantBuffer);


	context->DrawIndexed(mIndexCount, 0, 0);

	ResetUtils::ResetVertexShader(context);
	ResetUtils::ResetPixelShader(context);
	ResetUtils::ResetVertexBuffer(context);
	ResetUtils::ResetIndexBuffer(context);
	ResetUtils::ResetAllConstantBuffers(context);

	mIsUpdated = false;
}

void Staebchen::UpdateCamera(ID3D11DeviceContext* context, XMMATRIX view, XMMATRIX proj, XMFLOAT2 pos, float rot)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HR(context->Map(mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	StaebchenConstantBuffer* matrix = (StaebchenConstantBuffer*)mappedResource.pData;
	matrix->view = XMMatrixTranspose(view);
	matrix->proj = XMMatrixTranspose(proj);
	matrix->world = XMMatrixTranspose(XMMatrixRotationZ(rot) * XMMatrixTranslation(pos.x, pos.y, 0.0));

	context->Unmap(mConstantBuffer, 0);

	mIsUpdated = true;
}
