// Developed By Wrexa Technologies 2023


#include "Wrexa_Screen_Capture.h"

//#include "Wrexa_Player_Controller.h"
#include "Wrexa_Instance.h"

#pragma optimize("", off)
#ifdef _WIN32
#ifdef USE_DX_SCREEN_CAP
Dev::Dev() {
	static const D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	static const D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_1
	};

	for (const auto& driverType : driverTypes) {
		const auto hr = D3D11CreateDevice(
			nullptr,
			driverType,
			nullptr,
			0,
			featureLevels,
			static_cast<UINT>(std::size(featureLevels)),
			D3D11_SDK_VERSION,
			&device,
			&featureLevel,
			&deviceContext
		);
		if (SUCCEEDED(hr)) {
			break;
		}
		if (device)
		{
			device->Release();
			device = nullptr;
		}
		if (deviceContext)
		{
			deviceContext->Release();
			deviceContext = nullptr;
		}
	}
}

Dev::~Dev() {
	if (device != nullptr)
	{
		device->Release();
		device = nullptr;
	}
	if (deviceContext != nullptr)
	{
		deviceContext->Release();
		deviceContext = nullptr;
	}
}

OutputDuplication::OutputDuplication()
{
	if (outputDuplication != nullptr)
	{
		outputDuplication->Release();
		outputDuplication = nullptr;
	}
}

OutputDuplication::OutputDuplication(ID3D11Device* device, int monitorIndex)
{
	HRESULT hr;

	//IDXGIDevice* dxgiDevice;
	hr = device->QueryInterface(
		__uuidof(dxgiDevice),
		reinterpret_cast<void**>(&dxgiDevice)
	);
	if (FAILED(hr)) 
	{
		return;
	}

	//IDXGIAdapter* dxgiAdapter;
	hr = dxgiDevice->GetParent(
		__uuidof(dxgiAdapter),
		reinterpret_cast<void**>(&dxgiAdapter)
	);
	if (FAILED(hr)) 
	{
		return;
	}

	// Enumerate available outputs
	//IDXGIOutput* dxgiOutput;
	//std::vector<IDXGIOutput*> outputs;
	for (UINT i = 0; dxgiAdapter->EnumOutputs(i, &dxgiOutput) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		outputs.push_back(dxgiOutput);
	}

	// Select the desired output
	if (monitorIndex >= outputs.size()) 
	{ 
		return;
	}
	//IDXGIOutput1* dxgiOutput1;
	hr = outputs[monitorIndex]->QueryInterface(
		__uuidof(IDXGIOutput1),
		reinterpret_cast<void**>(&dxgiOutput1)
	);
	if (FAILED(hr)) 
	{ 
		return;
	}

	// Create output duplication
	hr = dxgiOutput1->DuplicateOutput(device, &outputDuplication);
	if (FAILED(hr)) 
	{ 
		return;
	}
}

OutputDuplication& OutputDuplication::operator=(OutputDuplication&& other)
{
    if (this != &other) 
	{        
        // Move resources from 'other' to this object
        outputDuplication = other.outputDuplication;
        
        // Make sure 'other' no longer owns any resources
        other.outputDuplication = nullptr;
    }
    return *this;
}


OutputDuplication::~OutputDuplication()
{
	// Release the output duplication object
	if (outputDuplication != nullptr)
	{
		outputDuplication->Release();
		outputDuplication = nullptr;
	}

	// Release the output objects in the vector
	for (auto output : outputs)
	{
		if (output != nullptr)
		{
			output->Release();
			output = nullptr;
		}
	}
	outputs.clear();

	// Release the dxgiOutput1 object
	if (dxgiOutput1 != nullptr)
	{
		dxgiOutput1->Release();
		dxgiOutput1 = nullptr;
	}

	// Release the dxgiAdapter object
	if (dxgiAdapter != nullptr)
	{
		dxgiAdapter->Release();
		dxgiAdapter = nullptr;
	}

	// Release the dxgiDevice object
	if (dxgiDevice != nullptr)
	{
		dxgiDevice->Release();
		dxgiDevice = nullptr;
	}
}

AcquiredDesktopImage::AcquiredDesktopImage(IDXGIOutputDuplication* outputDuplication) 
{
	//IDXGIResource* desktopResource = nullptr;
	HRESULT hr = E_FAIL;
	for (int i = 0; i < 5; ++i) 
	{
		SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
		DXGI_OUTDUPL_FRAME_INFO fi{};
		const int timeoutMsec = 10; // milliseconds
		hr = outputDuplication->AcquireNextFrame(timeoutMsec, &fi, &desktopResource);
		if (SUCCEEDED(hr) && (fi.LastPresentTime.QuadPart == 0)) 
		{
			// If AcquireNextFrame() returns S_OK and
			// fi.LastPresentTime.QuadPart == 0, it means
			// AcquireNextFrame() didn't acquire next frame yet.
			// We must wait next frame sync timing to retrieve
			// actual frame data.
			//
			// Since method is successfully completed,
			// we need to release the resource and frame explicitly.
			desktopResource->Release();
			outputDuplication->ReleaseFrame();
			//Sleep(1000);
			continue;
		}
		else 
		{
			break;
		}
	}
	if (FAILED(hr)) 
	{
		if (desktopResource != nullptr)
		{
			desktopResource->Release();
			desktopResource = nullptr;
		}
		return;
	}

	hr = desktopResource->QueryInterface(
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&acquiredDesktopImage)
	);
	//if (desktopResource != nullptr)
	//{
	//	desktopResource->Release();
	//	desktopResource = nullptr;
	//}
	SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
	if (FAILED(hr))
	{ 
		return;
	}
}

AcquiredDesktopImage::~AcquiredDesktopImage() 
{
	if (acquiredDesktopImage != nullptr)
	{
		acquiredDesktopImage->Release();
		acquiredDesktopImage = nullptr;
	}
	if (desktopResource != nullptr)
	{
		desktopResource->Release();
		desktopResource = nullptr;
	}
}

Singleton_DirectX_Screen_Capture& Singleton_DirectX_Screen_Capture::GetInstance()
{
	static Singleton_DirectX_Screen_Capture inst;
	return inst;
}

bool Singleton_DirectX_Screen_Capture::Get_Duplicated_Output(int Monitor_Index, OutputDuplication** Return_Val)
{
	(*Return_Val) = nullptr;
	if ((Singleton_DirectX_Screen_Capture::GetInstance().DX_Duplicated_Output_Map.find(Monitor_Index)) != (Singleton_DirectX_Screen_Capture::GetInstance().DX_Duplicated_Output_Map.end()))
	{
		(*Return_Val) = Singleton_DirectX_Screen_Capture::GetInstance().DX_Duplicated_Output_Map.at(Monitor_Index);
		return true;
	}
	return false;
}

void Singleton_DirectX_Screen_Capture::Add_New_Duplicated_Output(int Monitor_Index, OutputDuplication* To_Add_Val)
{
	if (DX_Duplicated_Output_Map.find(Monitor_Index) != DX_Duplicated_Output_Map.end())
	{
		delete (DX_Duplicated_Output_Map.at(Monitor_Index));
	}
	else
	{
		if (Monitor_Index > Largest_Monitor_Index)
		{
			Largest_Monitor_Index = Monitor_Index;
		}
	}

	DX_Duplicated_Output_Map[Monitor_Index] = To_Add_Val;
}

void Singleton_DirectX_Screen_Capture::Remove_Duplicated_Output(int Monitor_Index)
{
	if (DX_Duplicated_Output_Map.find(Monitor_Index) != DX_Duplicated_Output_Map.end())
	{
		delete (DX_Duplicated_Output_Map.at(Monitor_Index));
		DX_Duplicated_Output_Map[Monitor_Index] = nullptr;
		if (Monitor_Index == Largest_Monitor_Index)
		{
			if (Largest_Monitor_Index > 0)
			{
				Largest_Monitor_Index = Largest_Monitor_Index - 1;
			}
		}
	}
}

void Singleton_DirectX_Screen_Capture::Add_Duplicated_Output(int Monitor_Index, OutputDuplication* To_Add_Val)
{
	Singleton_DirectX_Screen_Capture::GetInstance().Add_New_Duplicated_Output(Monitor_Index, To_Add_Val);
	//if (GetInstance().DX_Duplicated_Output_Map.find(Monitor_Index) == GetInstance().DX_Duplicated_Output_Map.end())
	//{
	//    GetInstance().Add_New_Duplicated_Output(Monitor_Index, To_Add_Val);
	//}
	//else
	//{
	//
	//}
}

OutputDuplication* Singleton_DirectX_Screen_Capture::Return_Duplicated_Output_Add_If_Does_Not_Exist(int Monitor_Index, ID3D11Device* device)
{
	bool Should_Reconstruct = true;

	OutputDuplication* DX_Duplicated_Output_ptr = nullptr;
	if (!(Singleton_DirectX_Screen_Capture::Get_Duplicated_Output(Monitor_Index, &DX_Duplicated_Output_ptr)))
	{
		DX_Duplicated_Output_ptr = new OutputDuplication(device, Monitor_Index);
		if (DX_Duplicated_Output_ptr == nullptr)
		{
			return nullptr;
		}
		else
		{
			if (DX_Duplicated_Output_ptr->outputDuplication == nullptr)
			{
				delete DX_Duplicated_Output_ptr;
				DX_Duplicated_Output_ptr = nullptr;
				return nullptr;
			}
		}

		Singleton_DirectX_Screen_Capture::Add_Duplicated_Output(Monitor_Index, DX_Duplicated_Output_ptr);
		Should_Reconstruct = false;
	}
	else
	{
		if (DX_Duplicated_Output_ptr != nullptr)
		{
			if (DX_Duplicated_Output_ptr->outputDuplication != nullptr)
			{
				Should_Reconstruct = false;
			}
		}

		if (Should_Reconstruct)
		{
			DX_Duplicated_Output_ptr = nullptr;
			DX_Duplicated_Output_ptr = new OutputDuplication(device, Monitor_Index);
			if (DX_Duplicated_Output_ptr == nullptr)
			{
				return nullptr;
			}
			else
			{
				if (DX_Duplicated_Output_ptr->outputDuplication == nullptr)
				{
					delete DX_Duplicated_Output_ptr;
					DX_Duplicated_Output_ptr = nullptr;
					return nullptr;
				}
			}

			Singleton_DirectX_Screen_Capture::Add_Duplicated_Output(Monitor_Index, DX_Duplicated_Output_ptr);
		}
	}

	return DX_Duplicated_Output_ptr;
}

void Singleton_DirectX_Screen_Capture::Remove_Duplicated_Output_If_Exists(int Monitor_Index)
{
	Singleton_DirectX_Screen_Capture::GetInstance().Remove_Duplicated_Output(Monitor_Index);
}

Singleton_DirectX_Screen_Capture::~Singleton_DirectX_Screen_Capture()
{
	for (int i = Largest_Monitor_Index; i >= 0; --i)
	{
		if (DX_Duplicated_Output_Map.find(i) != DX_Duplicated_Output_Map.end())
		{
			delete (DX_Duplicated_Output_Map.at(i));
			DX_Duplicated_Output_Map[i] = nullptr;
			DX_Duplicated_Output_Map.erase(i);
		}
	}
	DX_Duplicated_Output_Map.clear();
	Largest_Monitor_Index = 0;
}

DX_Screen_Cap_Struct::DX_Screen_Cap_Struct()
{
	DX_Duplicated_Output_ptr = nullptr;
	tex2dStaging = nullptr;
}

bool DX_Screen_Cap_Struct::Initialize(int Monitor_Index)
{
	Free_Frame();
	if (DX_Duplicated_Output_ptr != nullptr)
	{
		delete DX_Duplicated_Output_ptr;
		DX_Duplicated_Output_ptr = nullptr;
	}

	//DX_Device = Dev();
	ID3D11Device* device = DX_Device.device;
	ID3D11DeviceContext* deviceContext = DX_Device.deviceContext;
	if (device == nullptr)
	{
		return false;
	}

	DX_Duplicated_Output_ptr = new OutputDuplication(device, Monitor_Index);
	if (DX_Duplicated_Output_ptr == nullptr)
	{
		return false;
	}
	else
	{
		if (DX_Duplicated_Output_ptr->outputDuplication == nullptr)
		{
			delete DX_Duplicated_Output_ptr;
			DX_Duplicated_Output_ptr = nullptr;
			return false;
		}
	}

	Capturing_Monitor_Index = Monitor_Index;
	return true;
}

DX_Screen_Cap_Struct::DX_Screen_Cap_Struct(int Monitor_Index, bool &Is_Successful)
{
	tex2dStaging = nullptr;
	Is_Successful = Initialize(Monitor_Index);// Return Value Ignored	
}

DX_Screen_Cap_Struct& DX_Screen_Cap_Struct::DX_Screen_Cap_Struct::operator=(DX_Screen_Cap_Struct&& other)
{
	if (this != &other)
	{
		// First Delete Old Resource If It Exists
		Free_Frame();
		if (DX_Duplicated_Output_ptr != nullptr)
		{
			delete DX_Duplicated_Output_ptr;
			DX_Duplicated_Output_ptr = nullptr;
		}

		// Move resources from 'other' to this object
		DX_Duplicated_Output_ptr = other.DX_Duplicated_Output_ptr;
		tex2dStaging = other.tex2dStaging;
		DX_Device.device = other.DX_Device.device;
		DX_Device.deviceContext = other.DX_Device.deviceContext;
		DX_Device.featureLevel = other.DX_Device.featureLevel;
		Capturing_Monitor_Index = other.Capturing_Monitor_Index;

		// Make sure 'other' no longer owns any resources
		other.DX_Duplicated_Output_ptr = nullptr;
		other.tex2dStaging = nullptr;
		other.DX_Device.device = nullptr;
		other.DX_Device.deviceContext = nullptr;
		//other.Capturing_Monitor_Index = 0;
	}
	return *this;
}

ID3D11ComputeShader* CreateComputeShader(ID3D11Device* device, const char Resize_Code[], const char* entryPoint)
{
	ID3D10Blob* shaderBlob = nullptr;
	ID3D10Blob* errorBlob = nullptr;

	HRESULT hr = D3DCompile(
		Resize_Code,
		strlen(Resize_Code),
		nullptr,
		nullptr,
		nullptr,
		entryPoint,
		"cs_5_0",
		0,
		0,
		&shaderBlob,
		&errorBlob
	);

	if (FAILED(hr))
	{
		if (errorBlob != nullptr)
		{
			char* error = (char*)errorBlob->GetBufferPointer();
			OutputDebugStringA(error);
			errorBlob->Release();
		}

		if (shaderBlob != nullptr)
		{
			shaderBlob->Release();
		}

		return nullptr;
	}

	ID3D11ComputeShader* computeShader = nullptr;
	hr = device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &computeShader);

	shaderBlob->Release();

	if (FAILED(hr))
	{
		return nullptr;
	}

	return computeShader;
}

bool DX_Screen_Cap_Struct::Get_Frame(int& Return_Width, int& Return_Height, int Resized_Width, int Resized_Height, unsigned char** RGBA_Buffer)
{
	ID3D11Device* device = DX_Device.device;
	ID3D11DeviceContext* deviceContext = DX_Device.deviceContext;

	//SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
	Free_Frame();
	SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);

	AcquiredDesktopImage adi(DX_Duplicated_Output_ptr->outputDuplication);
	if (adi.acquiredDesktopImage == nullptr)
	{
		return false;
	}

	DXGI_OUTDUPL_DESC duplDesc;
	DX_Duplicated_Output_ptr->outputDuplication->GetDesc(&duplDesc);
	const auto f = static_cast<int>(duplDesc.ModeDesc.Format);
	const auto goodFormat = f == DXGI_FORMAT_B8G8R8A8_UNORM
		|| f == DXGI_FORMAT_B8G8R8X8_UNORM
		|| f == DXGI_FORMAT_B8G8R8A8_TYPELESS
		|| f == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
		|| f == DXGI_FORMAT_B8G8R8X8_TYPELESS
		|| f == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
	if (!goodFormat)
	{
		//Singleton_DirectX_Screen_Capture::Remove_Duplicated_Output_If_Exists(Capturing_Monitor_Index);
		return false;
	}

	Return_Width = duplDesc.ModeDesc.Width;
	Return_Height = duplDesc.ModeDesc.Height;

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = duplDesc.ModeDesc.Width;
	desc.Height = duplDesc.ModeDesc.Height;
	desc.Format = duplDesc.ModeDesc.Format;
	desc.ArraySize = 1;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.MipLevels = 1;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	
	SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
	HRESULT hr = device->CreateTexture2D(&desc, nullptr, &tex2dStaging);
	if (FAILED(hr))
	{
		return false;
	}
	if (tex2dStaging == nullptr)
	{
		return false;
	}

	deviceContext->CopyResource(tex2dStaging, adi.acquiredDesktopImage);

	tex2dStaging->GetDesc(&desc);

	//int New_width = static_cast<int>(desc.Width);
	//int New_height = static_cast<int>(desc.Height);
	//Return_Width = New_width;
	//Return_Height = New_height;

	Return_Width = static_cast<int>(desc.Width);
	Return_Height = static_cast<int>(desc.Height);

	hr = deviceContext->Map(
		tex2dStaging,
		D3D11CalcSubresource(0, 0, 0),
		D3D11_MAP_READ,
		0,
		&res
	);
	if (FAILED(hr))
	{
		Free_Frame();
		return false;
	}

	// Directly Accessing the DX buffer
	(*RGBA_Buffer) = (unsigned char*)(res.pData);

	// This Makes a Copy Of it
	//memcpy((*RGBA_Buffer), res.pData, Return_Width * Return_Height * 4);
	//deviceContext->Unmap(tex2dStaging, 0);
	//tex2dStaging->Release();
	//DX_Duplicated_Output_ptr->outputDuplication->ReleaseFrame();

	return true;
}

bool DX_Screen_Cap_Struct::Get_Frame_Not_Working(int& Return_Width, int& Return_Height, int Resized_Width, int Resized_Height, unsigned char** RGBA_Buffer)
{
	const char Resize_Code[] = "Texture2D<float4> inputTexture : register(t0);\
								SamplerState inputSampler : register(s0);\
								RWTexture2D<float4> outputTexture : register(u0);\
								cbuffer ConstantBuffer : register(b0)\
								{\
									float2 inverseInputSize;\
									float2 inverseOutputSize;\
								};\
								[numthreads(16, 16, 1)]\
								void main(uint3 dispatchThreadId : SV_DispatchThreadID)\
								{\
									float2 texCoord = (dispatchThreadId.xy + 0.5f) * inverseOutputSize;\
									float4 color = inputTexture.Sample(inputSampler, texCoord * inverseInputSize);\
									outputTexture[dispatchThreadId.xy] = color;\
								}";


	ID3D11Device* device = DX_Device.device;
	ID3D11DeviceContext* deviceContext = DX_Device.deviceContext;

	// Create the compute shader object
	ID3D11ComputeShader* computeShader = CreateComputeShader(device, Resize_Code, "main");

	if (computeShader == nullptr)
	{
		return false;
	}

	SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
	AcquiredDesktopImage adi(DX_Duplicated_Output_ptr->outputDuplication);
	if (adi.acquiredDesktopImage == nullptr)
	{
		computeShader->Release();
		return false;
	}

	DXGI_OUTDUPL_DESC duplDesc;
	DX_Duplicated_Output_ptr->outputDuplication->GetDesc(&duplDesc);
	const auto f = static_cast<int>(duplDesc.ModeDesc.Format);
	const auto goodFormat = f == DXGI_FORMAT_B8G8R8A8_UNORM
		|| f == DXGI_FORMAT_B8G8R8X8_UNORM
		|| f == DXGI_FORMAT_B8G8R8A8_TYPELESS
		|| f == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
		|| f == DXGI_FORMAT_B8G8R8X8_TYPELESS
		|| f == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
	if (!goodFormat)
	{
		//Singleton_DirectX_Screen_Capture::Remove_Duplicated_Output_If_Exists(Capturing_Monitor_Index);
		computeShader->Release();
		return false;
	}
	if ((Resized_Height <= 0) || (Resized_Height <= 0))
	{
		Resized_Width = duplDesc.ModeDesc.Width;
		Resized_Height = duplDesc.ModeDesc.Height;
	}
	else
	{
		Return_Width = Resized_Width;
		Return_Height = Resized_Height;
	}

	// Create a new texture with the desired dimensions
	ID3D11Texture2D* resizedTexture = nullptr;
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = duplDesc.ModeDesc.Width;
	desc.Height = duplDesc.ModeDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

	HRESULT hr = device->CreateTexture2D(&desc, nullptr, &resizedTexture);
	if (FAILED(hr))
	{
		computeShader->Release();
		return false;
	}

	// Create a UAV for the resized texture
	ID3D11UnorderedAccessView* resizedUAV = nullptr;
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = desc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	hr = device->CreateUnorderedAccessView(resizedTexture, &uavDesc, &resizedUAV);
	if (FAILED(hr))
	{
		resizedTexture->Release();
		computeShader->Release();
		return false;
	}

	deviceContext->CopyResource(resizedTexture, adi.acquiredDesktopImage);

	// Create shader resource view for the resized texture
	ID3D11ShaderResourceView* resizedSRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(resizedTexture, &srvDesc, &resizedSRV);

	// Set the compute shader and resources
	deviceContext->CSSetShader(computeShader, nullptr, 0);
	deviceContext->CSSetShaderResources(0, 1, &resizedSRV);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &resizedUAV, nullptr);

	// Dispatch the compute shader
	deviceContext->Dispatch((UINT)ceil(duplDesc.ModeDesc.Width / 8.0f), (UINT)ceil(duplDesc.ModeDesc.Height / 8.0f), 1);

	//D3D11_TEXTURE2D_DESC desc{};
	desc.Width = duplDesc.ModeDesc.Width;
	desc.Height = duplDesc.ModeDesc.Height;
	desc.Format = duplDesc.ModeDesc.Format;
	desc.ArraySize = 1;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.MipLevels = 1;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;

	SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
	hr = device->CreateTexture2D(&desc, nullptr, &tex2dStaging);
	if (FAILED(hr))
	{
		resizedTexture->Release();
		resizedUAV->Release();
		computeShader->Release();
		return false;
	}
	if (tex2dStaging == nullptr)
	{
		resizedTexture->Release();
		resizedUAV->Release();
		return false;
	}

	// Copy the resized texture to the staging resource
	deviceContext->CopyResource(tex2dStaging, resizedTexture);
	tex2dStaging->GetDesc(&desc);

	// Cleanup resources
	resizedTexture->Release();
	resizedUAV->Release();
	computeShader->Release();


	hr = deviceContext->Map(
		tex2dStaging,
		D3D11CalcSubresource(0, 0, 0),
		D3D11_MAP_READ,
		0,
		&res
	);
	if (FAILED(hr))
	{
		computeShader->Release();
		Free_Frame();
		return false;
	}

	// Directly Accessing the DX buffer
	(*RGBA_Buffer) = (unsigned char*)(res.pData);

	return true;
}

void DX_Screen_Cap_Struct::Free_Frame()
{
	if (DX_Device.deviceContext != nullptr)
	{
		if (tex2dStaging != nullptr)
		{
			DX_Device.deviceContext->Unmap(tex2dStaging, 0);
			tex2dStaging->Release();
			tex2dStaging = nullptr;

			if (DX_Duplicated_Output_ptr != nullptr)
			{
				if (DX_Duplicated_Output_ptr->outputDuplication != nullptr)
				{
					DX_Duplicated_Output_ptr->outputDuplication->ReleaseFrame();
				}
			}
		}
	}

	res.DepthPitch = 0;
	res.pData = nullptr;
	res.RowPitch = 0;
}

DX_Screen_Cap_Struct::~DX_Screen_Cap_Struct()
{
	Free_Frame();
	if (DX_Duplicated_Output_ptr != nullptr)
	{
		delete DX_Duplicated_Output_ptr;
		DX_Duplicated_Output_ptr = nullptr;
	}
}

Singleton_DirectX_Screen_Capture_Direct_Access_Buffer& Singleton_DirectX_Screen_Capture_Direct_Access_Buffer::GetInstance()
{
	static Singleton_DirectX_Screen_Capture_Direct_Access_Buffer inst;
	return inst;
}

bool Singleton_DirectX_Screen_Capture_Direct_Access_Buffer::Get_Frame(int Monitor_Index, int& Return_Width, int& Return_Height, int Resized_Width, int Resized_Height, unsigned char** RGBA_Buffer)
{
	bool Is_Successful = false;

	Singleton_DirectX_Screen_Capture_Direct_Access_Buffer& inst = Singleton_DirectX_Screen_Capture_Direct_Access_Buffer::GetInstance();

	DX_Screen_Cap_Struct* DX_Screen_Cap_Context = nullptr;

	if (((inst.Screen_Capture_Context_Map.find(Monitor_Index)) == (inst.Screen_Capture_Context_Map.end())))
	{
		DX_Screen_Cap_Context = new DX_Screen_Cap_Struct(Monitor_Index, Is_Successful);

		if (DX_Screen_Cap_Context == nullptr)
		{
			return false;
		}

		if (!Is_Successful)
		{
			delete DX_Screen_Cap_Context;
			DX_Screen_Cap_Context = nullptr;
			return false;
		}

		(inst.Screen_Capture_Context_Map[Monitor_Index]) = DX_Screen_Cap_Context;

		if (inst.Largest_Monitor_Index < Monitor_Index)
		{
			inst.Largest_Monitor_Index = Monitor_Index;
		}
	}
	else
	{
		DX_Screen_Cap_Context = (inst.Screen_Capture_Context_Map[Monitor_Index]);
		if (DX_Screen_Cap_Context == nullptr)
		{
			DX_Screen_Cap_Context = new DX_Screen_Cap_Struct(Monitor_Index, Is_Successful);

			if (DX_Screen_Cap_Context == nullptr)
			{
				(inst.Screen_Capture_Context_Map.erase(Monitor_Index));
				return false;
			}

			if (!Is_Successful)
			{
				delete DX_Screen_Cap_Context;
				DX_Screen_Cap_Context = nullptr;

				(inst.Screen_Capture_Context_Map.erase(Monitor_Index));
				return false;
			}

			(inst.Screen_Capture_Context_Map[Monitor_Index]) = DX_Screen_Cap_Context;
		}
	}	

	if (!(DX_Screen_Cap_Context->Get_Frame(Return_Width, Return_Height, Resized_Width, Resized_Height, RGBA_Buffer)))
	{
		// Reinitializes
		if (DX_Screen_Cap_Context->Initialize(Monitor_Index))
		{
			if (DX_Screen_Cap_Context->Get_Frame(Return_Width, Return_Height, Resized_Width, Resized_Height, RGBA_Buffer))
			{
				return true;
			}
		}
	}
	else
	{
		return true;
	}

	delete (inst.Screen_Capture_Context_Map[Monitor_Index]);
	(inst.Screen_Capture_Context_Map[Monitor_Index]) = nullptr;
	(inst.Screen_Capture_Context_Map.erase(Monitor_Index));

	if (inst.Largest_Monitor_Index == Monitor_Index)
	{
		if (inst.Largest_Monitor_Index > 0)
		{
			inst.Largest_Monitor_Index = Monitor_Index - 1;
		}
	}
	return false;
}
void Singleton_DirectX_Screen_Capture_Direct_Access_Buffer::Remove_Screen_Capture_Context(int Monitor_Index)
{
	Singleton_DirectX_Screen_Capture_Direct_Access_Buffer& inst = Singleton_DirectX_Screen_Capture_Direct_Access_Buffer::GetInstance();
	if (((inst.Screen_Capture_Context_Map.find(Monitor_Index)) != (inst.Screen_Capture_Context_Map.end())))
	{
		if ((inst.Screen_Capture_Context_Map[Monitor_Index]) != nullptr)
		{
			delete (inst.Screen_Capture_Context_Map[Monitor_Index]);
			(inst.Screen_Capture_Context_Map[Monitor_Index]) = nullptr;
		}
		(inst.Screen_Capture_Context_Map.erase(Monitor_Index));
	}
}

Singleton_DirectX_Screen_Capture_Direct_Access_Buffer::~Singleton_DirectX_Screen_Capture_Direct_Access_Buffer()
{
	for (int i = Largest_Monitor_Index; i >= 0; --i)
	{
		if (Screen_Capture_Context_Map.find(i) != Screen_Capture_Context_Map.end())
		{
			if (Screen_Capture_Context_Map[i] != nullptr)
			{
				delete Screen_Capture_Context_Map[i];
				Screen_Capture_Context_Map[i] = nullptr;
			}
			Screen_Capture_Context_Map.erase(i);
		}
	}
	Screen_Capture_Context_Map.clear();
	Largest_Monitor_Index = 0;
}

void Singleton_DirectX_Screen_Capture_Direct_Access_Buffer::Stop_Externally()
{
	Singleton_DirectX_Screen_Capture_Direct_Access_Buffer& SingleTon_Ref = GetInstance();

	for (int i = SingleTon_Ref.Largest_Monitor_Index; i >= 0; --i)
	{
		if (SingleTon_Ref.Screen_Capture_Context_Map.find(i) != SingleTon_Ref.Screen_Capture_Context_Map.end())
		{
			if (SingleTon_Ref.Screen_Capture_Context_Map[i] != nullptr)
			{
				delete SingleTon_Ref.Screen_Capture_Context_Map[i];
				SingleTon_Ref.Screen_Capture_Context_Map[i] = nullptr;
			}
			SingleTon_Ref.Screen_Capture_Context_Map.erase(i);
		}
	}
	SingleTon_Ref.Screen_Capture_Context_Map.clear();
	SingleTon_Ref.Largest_Monitor_Index = 0;
}

bool UWrexa_Screen_Capture::captureDesktop_DX(int Monitor_To_Capture, int& width, int& height, unsigned char** RGBA_Buffer)
{
	SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
	Dev dev;
	ID3D11Device* device = dev.device;
	ID3D11DeviceContext* deviceContext = dev.deviceContext;
	if (device == nullptr)
	{
		return false;
	}

	// Create tex2dStaging which represents duplication image of desktop.
	ID3D11Texture2D* tex2dStaging = nullptr;
	{
		OutputDuplication od(device, Monitor_To_Capture);
		IDXGIOutputDuplication* outputDuplication = od.outputDuplication;
		if (outputDuplication == nullptr)
		{
			return false;
		}

		SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
		AcquiredDesktopImage adi(outputDuplication);
		ID3D11Texture2D* acquiredDesktopImage = adi.acquiredDesktopImage;
		if (acquiredDesktopImage == nullptr)
		{
			return false;
		}

		DXGI_OUTDUPL_DESC duplDesc;
		outputDuplication->GetDesc(&duplDesc);

		const auto f = static_cast<int>(duplDesc.ModeDesc.Format);
		const auto goodFormat = f == DXGI_FORMAT_B8G8R8A8_UNORM
			|| f == DXGI_FORMAT_B8G8R8X8_UNORM
			|| f == DXGI_FORMAT_B8G8R8A8_TYPELESS
			|| f == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
			|| f == DXGI_FORMAT_B8G8R8X8_TYPELESS
			|| f == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
		if (!goodFormat)
		{
			return false;
		}

		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = duplDesc.ModeDesc.Width;
		desc.Height = duplDesc.ModeDesc.Height;
		desc.Format = duplDesc.ModeDesc.Format;
		desc.ArraySize = 1;
		desc.BindFlags = 0;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.MipLevels = 1;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;
		SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
		const auto hr = device->CreateTexture2D(&desc, nullptr, &tex2dStaging);
		if (FAILED(hr))
		{
			return false;
		}
		if (tex2dStaging == nullptr)
		{
			return false;
		}

		SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
		deviceContext->CopyResource(tex2dStaging, acquiredDesktopImage);

		if (outputDuplication)
		{
			outputDuplication->Release();
			outputDuplication = nullptr;
			od.outputDuplication = nullptr;
		}
	}

	// Lock tex2dStaging and copy its content from GPU to CPU memory.
	//Image image;

	D3D11_TEXTURE2D_DESC desc;
	tex2dStaging->GetDesc(&desc);

	D3D11_MAPPED_SUBRESOURCE res;
	const auto hr = deviceContext->Map(
		tex2dStaging,
		D3D11CalcSubresource(0, 0, 0),
		D3D11_MAP_READ,
		0,
		&res
	);
	if (FAILED(hr))
	{
		return false;
	}
	//image.width = static_cast<int>(desc.Width);
	//image.height = static_cast<int>(desc.Height);
	//image.rowPitch = res.RowPitch;
	//image.bytes.resize(image.rowPitch * image.height);
	//memcpy(image.bytes.data(), res.pData, image.bytes.size());
	//deviceContext->Unmap(tex2dStaging, 0);

	//return image;

	// Get the screen dimensions
	int New_width = static_cast<int>(desc.Width);
	int New_height = static_cast<int>(desc.Height);

	if ((*RGBA_Buffer) == nullptr)
	{
		(*RGBA_Buffer) = new unsigned char[New_width * New_height * 4];
	}
	else
	{
		if ((width != New_width) || (height != New_height))
		{
			delete (*RGBA_Buffer);
			(*RGBA_Buffer) = nullptr;
			(*RGBA_Buffer) = new unsigned char[New_width * New_height * 4];
		}
	}
	width = New_width;
	height = New_height;
	memcpy((*RGBA_Buffer), res.pData, New_width * New_height * 4);
	deviceContext->Unmap(tex2dStaging, 0);

	SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
	return true;
}
bool UWrexa_Screen_Capture::captureDesktop_DX_REUSE(int Monitor_To_Capture, int& width, int& height, unsigned char** RGBA_Buffer)
{
	SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
	Dev dev;
	ID3D11Device* device = dev.device;
	ID3D11DeviceContext* deviceContext = dev.deviceContext;
	if (device == nullptr)
	{
		return false;
	}

	// Create tex2dStaging which represents duplication image of desktop.
	ID3D11Texture2D* tex2dStaging = nullptr;
	OutputDuplication* DX_Duplicated_Output_ptr = Singleton_DirectX_Screen_Capture::Return_Duplicated_Output_Add_If_Does_Not_Exist(Monitor_To_Capture, device);
	{
		//bool Is_New_Or_Changed = false;
		if (DX_Duplicated_Output_ptr == nullptr)
		{
			return false;
			//Is_New_Or_Changed = true;
		}

		SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
		AcquiredDesktopImage adi(DX_Duplicated_Output_ptr->outputDuplication);
		ID3D11Texture2D* acquiredDesktopImage = adi.acquiredDesktopImage;
		if (acquiredDesktopImage == nullptr)
		{
			return false;
		}		

		DXGI_OUTDUPL_DESC duplDesc;
		DX_Duplicated_Output_ptr->outputDuplication->GetDesc(&duplDesc);
		const auto f = static_cast<int>(duplDesc.ModeDesc.Format);
		const auto goodFormat = f == DXGI_FORMAT_B8G8R8A8_UNORM
			|| f == DXGI_FORMAT_B8G8R8X8_UNORM
			|| f == DXGI_FORMAT_B8G8R8A8_TYPELESS
			|| f == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
			|| f == DXGI_FORMAT_B8G8R8X8_TYPELESS
			|| f == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
		if (!goodFormat)
		{
			Singleton_DirectX_Screen_Capture::Remove_Duplicated_Output_If_Exists(Monitor_To_Capture);
			return false;
		}

		//if (Is_New_Or_Changed)
		//{
			Current_Description.Width = duplDesc.ModeDesc.Width;
			Current_Description.Height = duplDesc.ModeDesc.Height;
			Current_Description.Format = duplDesc.ModeDesc.Format;
			Current_Description.ArraySize = 1;
			Current_Description.BindFlags = 0;
			Current_Description.MiscFlags = 0;
			Current_Description.SampleDesc.Count = 1;
			Current_Description.SampleDesc.Quality = 0;
			Current_Description.MipLevels = 1;
			Current_Description.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			Current_Description.Usage = D3D11_USAGE_STAGING;
		//}
			SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
		const auto hr = device->CreateTexture2D(&Current_Description, nullptr, &tex2dStaging);
		if (FAILED(hr))
		{
			Singleton_DirectX_Screen_Capture::Remove_Duplicated_Output_If_Exists(Monitor_To_Capture);
			return false;
		}
		if (tex2dStaging == nullptr)
		{
			Singleton_DirectX_Screen_Capture::Remove_Duplicated_Output_If_Exists(Monitor_To_Capture);
			return false;
		}

		SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
		deviceContext->CopyResource(tex2dStaging, acquiredDesktopImage);

		//if (DX_Duplicated_Output)
		//{
		//	//DX_Duplicated_Output->outputDuplication->Release();			
		//}
		//DX_Duplicated_Output->outputDuplication->ReleaseFrame();
	}

	// Lock tex2dStaging and copy its content from GPU to CPU memory.
	//Image image;

	D3D11_TEXTURE2D_DESC desc;
	tex2dStaging->GetDesc(&desc);

	D3D11_MAPPED_SUBRESOURCE res;
	const auto hr = deviceContext->Map(
		tex2dStaging,
		D3D11CalcSubresource(0, 0, 0),
		D3D11_MAP_READ,
		0,
		&res
	);
	if (FAILED(hr))
	{
		Singleton_DirectX_Screen_Capture::Remove_Duplicated_Output_If_Exists(Monitor_To_Capture);
		return false;
	}
	//image.width = static_cast<int>(desc.Width);
	//image.height = static_cast<int>(desc.Height);
	//image.rowPitch = res.RowPitch;
	//image.bytes.resize(image.rowPitch * image.height);
	//memcpy(image.bytes.data(), res.pData, image.bytes.size());
	//deviceContext->Unmap(tex2dStaging, 0);

	//return image;

	// Get the screen dimensions
	int New_width = static_cast<int>(desc.Width);
	int New_height = static_cast<int>(desc.Height);

	if ((*RGBA_Buffer) == nullptr)
	{
		(*RGBA_Buffer) = new unsigned char[New_width * New_height * 4];
	}
	else
	{
		if ((width != New_width) || (height != New_height))
		{
			delete (*RGBA_Buffer);
			(*RGBA_Buffer) = nullptr;
			(*RGBA_Buffer) = new unsigned char[New_width * New_height * 4];
		}
	}
	width = New_width;
	height = New_height;
	memcpy((*RGBA_Buffer), res.pData, New_width * New_height * 4);
	deviceContext->Unmap(tex2dStaging, 0);
	tex2dStaging->Release();
	DX_Duplicated_Output_ptr->outputDuplication->ReleaseFrame();

	SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
	return true;
}
bool UWrexa_Screen_Capture::captureDesktop_DX_REUSE_Unsafe(int Monitor_To_Capture, int& Return_Width, int& Return_Height, int Resized_Width, int Resized_Height, unsigned char** RGBA_Buffer)
{
	return Singleton_DirectX_Screen_Capture_Direct_Access_Buffer::GetInstance().Get_Frame(Monitor_To_Capture, Return_Width, Return_Height, Resized_Width, Resized_Height, RGBA_Buffer);
}

#endif
#endif

void UWrexa_Screen_Capture::BeginDestory()
{
	Super::BeginDestroy();
	if (Image_Raw_Buffer != nullptr)
	{
		delete Image_Raw_Buffer;
		Image_Raw_Buffer = nullptr;
	}

#ifdef _WIN32
#ifdef USE_DX_SCREEN_CAP
	//if (DX_Duplicated_Output != nullptr)
	//{
	//	//DX_Duplicated_Output->outputDuplication->Release();
	//	delete DX_Duplicated_Output;
	//	DX_Duplicated_Output = nullptr;
	//}
#endif
#endif // _WIN32   

	//Stop_Screen_Capture();
}

// Don't Forget to Free Data Using delete Keyword
bool UWrexa_Screen_Capture::Capture_Screen_API(int Monitor_To_Capture, int& width, int& height, unsigned char** RGBA_Buffer)
{
#ifndef ENABLE_SCREEN_CAPTURE
	return false;
#else
	// Check The Total Number Of Monitors
	if (Monitor_To_Capture >= Get_Total_Number_Of_Monitors_API())
	{
		return false;
	}

#ifdef _WIN32
#ifdef USE_DX_SCREEN_CAP
	bool ReturnVal = captureDesktop_DX(Monitor_To_Capture, width, height, RGBA_Buffer);
	return ReturnVal;
#else
	std::vector<HMONITOR> monitors;
	if (!(EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&monitors))))
	{
		return false;
	}

	HMONITOR hMonitor = monitors[Monitor_To_Capture];

	// Get the monitor info
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(monitorInfo);
	GetMonitorInfo(hMonitor, &monitorInfo);

	// Get the screen dimensions
	int New_width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
	int New_height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

	if ((*RGBA_Buffer) == nullptr)
	{
		(*RGBA_Buffer) = new unsigned char[New_width * New_height * 4];
	}
	else
	{
		if ((width != New_width) || (height != New_height))
		{
			delete (*RGBA_Buffer);
			(*RGBA_Buffer) = nullptr;
			(*RGBA_Buffer) = new unsigned char[New_width * New_height * 4];
		}
	}
	width = New_width;
	height = New_height;

	// Create a device context for the entire screen
	HDC hdcScreen = GetDC(NULL);

	// Create a memory device context and bitmap to store the screen capture
	HDC hdcMemDC = CreateCompatibleDC(hdcScreen);
	HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMemDC, hbmScreen);

	// Capture the screen and store it in the bitmap
	BitBlt(hdcMemDC, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);
	//(*RGBA_Buffer) = new unsigned char[width * height * 4];
	BITMAPINFOHEADER bi = { sizeof(BITMAPINFOHEADER), width, -height, 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
	GetDIBits(hdcMemDC, hbmScreen, 0, height, (*RGBA_Buffer), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	// Clean up
	SelectObject(hdcMemDC, hbmOld);
	DeleteDC(hdcMemDC);
	ReleaseDC(NULL, hdcScreen);

	return true;
#endif
#elif __linux__

	char fb_path[64];
	sprintf(fb_path, "/dev/fb%d", Monitor_To_Capture);

	int fb_fd = 0;
	struct fb_var_screeninfo screen_info;
	unsigned char* fb_ptr = nullptr;

	// Open the framebuffer device
	fb_fd = open(fb_path, O_RDWR);
	if (fb_fd == -1)
	{
		return false;
	}
	if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &screen_info) == -1)
	{
		close(fb_fd);
		return false;
	}

	width = screen_info.xres_virtual;
	height = screen_info.yres_virtual;

	fb_ptr = (unsigned char*)mmap(0, (width * height * 4),
		PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	if (fb_ptr == MAP_FAILED)
	{
		close(fb_fd);
		return false;
	}

	(*RGBA_Buffer) = new unsigned char[(width * height * 4)];
	memcpy((*RGBA_Buffer), fb_ptr, (width * height * 4));

	// Cleanup
	munmap(fb_ptr, (width * height * 4));
	close(fb_fd);

	return true;
#else
	return false;
#endif
#endif
}

bool UWrexa_Screen_Capture::Capture_Screen_API_REUSE(int Monitor_To_Capture, int& width, int& height, unsigned char** RGBA_Buffer)
{
	// Check The Total Number Of Monitors
	if (Monitor_To_Capture >= Get_Total_Number_Of_Monitors_API())
	{
		return false;
	}
#ifndef ENABLE_SCREEN_CAPTURE
	return false;
#else
#ifdef _WIN32
#ifdef USE_DX_SCREEN_CAP
	//bool ReturnVal = captureDesktop_DX(Monitor_To_Capture, width, height, RGBA_Buffer);
	//return ReturnVal;
	return captureDesktop_DX_REUSE(Monitor_To_Capture, width, height, RGBA_Buffer);
#else
	std::vector<HMONITOR> monitors;
	if (!(EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&monitors))))
	{
		return false;
	}

	HMONITOR hMonitor = monitors[Monitor_To_Capture];

	// Get the monitor info
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(monitorInfo);
	GetMonitorInfo(hMonitor, &monitorInfo);

	// Get the screen dimensions
	int New_width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
	int New_height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

	if ((*RGBA_Buffer) == nullptr)
	{
		(*RGBA_Buffer) = new unsigned char[New_width * New_height * 4];
	}
	else
	{
		if ((width != New_width) || (height != New_height))
		{
			delete (*RGBA_Buffer);
			(*RGBA_Buffer) = nullptr;
			(*RGBA_Buffer) = new unsigned char[New_width * New_height * 4];
		}
	}
	width = New_width;
	height = New_height;

	// Create a device context for the entire screen
	HDC hdcScreen = GetDC(NULL);

	// Create a memory device context and bitmap to store the screen capture
	HDC hdcMemDC = CreateCompatibleDC(hdcScreen);
	HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMemDC, hbmScreen);

	// Capture the screen and store it in the bitmap
	BitBlt(hdcMemDC, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);
	//(*RGBA_Buffer) = new unsigned char[width * height * 4];
	BITMAPINFOHEADER bi = { sizeof(BITMAPINFOHEADER), width, -height, 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
	GetDIBits(hdcMemDC, hbmScreen, 0, height, (*RGBA_Buffer), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	// Clean up
	SelectObject(hdcMemDC, hbmOld);
	DeleteDC(hdcMemDC);
	ReleaseDC(NULL, hdcScreen);

	return true;
#endif
#elif __linux__

	char fb_path[64];
	sprintf(fb_path, "/dev/fb%d", Monitor_To_Capture);

	int fb_fd = 0;
	struct fb_var_screeninfo screen_info;
	unsigned char* fb_ptr = nullptr;

	// Open the framebuffer device
	fb_fd = open(fb_path, O_RDWR);
	if (fb_fd == -1)
	{
		return false;
	}
	if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &screen_info) == -1)
	{
		close(fb_fd);
		return false;
	}

	width = screen_info.xres_virtual;
	height = screen_info.yres_virtual;

	fb_ptr = (unsigned char*)mmap(0, (width * height * 4),
		PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	if (fb_ptr == MAP_FAILED)
	{
		close(fb_fd);
		return false;
	}

	(*RGBA_Buffer) = new unsigned char[(width * height * 4)];
	memcpy((*RGBA_Buffer), fb_ptr, (width * height * 4));

	// Cleanup
	munmap(fb_ptr, (width * height * 4));
	close(fb_fd);

	return true;
#else
	return false;
#endif
#endif
}

bool UWrexa_Screen_Capture::Start_Screen_Capture_Internal_API(const int& Monitor_Index, const int& CallBack_Frequency_Milliseconds, const int& Offset_Width, const int& Offset_Height)
{
	Current_Monitor_Index = Monitor_Index;
	if (Image_Raw_Buffer != nullptr)
	{
		delete Image_Raw_Buffer;
		Image_Raw_Buffer = nullptr;
	}
	return true;
	//return Capture_Screen_API_REUSE(Monitor_Index, Screen_Capture_Width, Screen_Capture_Height, (&((unsigned char*)Image_Raw_Buffer)));
}

bool UWrexa_Screen_Capture::Is_Screen_Capture_Allowed_By_The_Operating_System_API()
{
	FString My_Log_String = "screen_capture_lite_shared Checking if Screen Capture is Enabled";
#ifdef _WIN32
	// Windows Start
	return true;
#else
#ifdef __linux__
	// Linux code
	return false;
	// Linux End
#else
#ifdef __ANDROID__
	// Android code
	return false;
	// Android End
#else
#ifdef __APPLE__
	// macOS code
	return false;
	// macOS End
#else
	return false;
#endif
#endif
#endif
#endif
}

bool UWrexa_Screen_Capture::Request_To_Screen_Capture_Operating_System_API(bool Bypass_Check)
{
	//FString My_Log_String = "screen_capture_lite_shared Application Not Allowed to Capture the screen. Waiting for permission";
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *My_Log_String);
#ifdef _WIN32
	// Windows Start
	return true;
#else
#ifdef __linux__
	// Linux code
	return false;
	// Linux End
#else
#ifdef __ANDROID__
	// Android code
	return false;
	// Android End
#else
#ifdef __APPLE__
	// macOS code
	return false;
	// macOS End
#else
	return false;
#endif
#endif
#endif
#endif
}

bool UWrexa_Screen_Capture::Stop_Screen_Capture_API()
{
	//if (Image_Raw_Buffer != nullptr)
	//{
	//	delete Image_Raw_Buffer;
	//	Image_Raw_Buffer = nullptr;
	//}
	return true;
}

int UWrexa_Screen_Capture::Get_Total_Number_Of_Monitors()
{
	return Get_Total_Number_Of_Monitors_API();
}

bool UWrexa_Screen_Capture::Is_Screen_Capture_Allowed_By_The_Operating_System()
{
	FString My_Log_String = "screen_capture_lite_shared Checking if Screen Capture is Enabled";
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *My_Log_String);
	return Is_Screen_Capture_Allowed_By_The_Operating_System_API();
}

bool UWrexa_Screen_Capture::Request_To_Screen_Capture_Operating_System(bool Bypass_Check)
{	
	return Request_To_Screen_Capture_Operating_System_API(Bypass_Check);
}

bool UWrexa_Screen_Capture::Start_Screen_Capture(const int& Monitor_Index, const int& CallBack_Frequency_Milliseconds, const int& Offset_Width, const int& Offset_Height)
{
	return Start_Screen_Capture_Internal_API(Monitor_Index, CallBack_Frequency_Milliseconds, Offset_Width, Offset_Height);
}

bool UWrexa_Screen_Capture::Stop_Screen_Capture()
{
	return Stop_Screen_Capture_API();
}

UTexture2D* UWrexa_Screen_Capture::TextureFrom_Screen_Capture(bool Swap_Colour_Channel, bool& IsTextureAvailable)
{
#ifndef ENABLE_SCREEN_CAPTURE
	return NULL;
#else
	IsTextureAvailable = captureDesktop_DX_REUSE_Unsafe(Current_Monitor_Index, Screen_Capture_Width, Screen_Capture_Height, 0, 0, (unsigned char**)(&Image_Raw_Buffer));
	if (!IsTextureAvailable)
	{
		return NULL;
	};
	
	//FString My_Log_String = "screen_capture_lite_shared Attempting To Get Texture From Screen Capture";	
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *My_Log_String);
			
	if (Image_Raw_Buffer != nullptr)
	{
		return UWrexa_Blueprint_Functions::TextureFrom_RGBA_Image_From_Pointer(false, Screen_Capture_Width, Screen_Capture_Height, Image_Raw_Buffer);
	}
	else
	{
		//My_Log_String = "screen_capture_lite_shared Attempting To Get Texture From Screen Capture Failed As Image Buffer is Unchanged";
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *My_Log_String);
	}
#endif
	return NULL;
}

void UWrexa_Screen_Capture::Image_From_Screen_Capture(bool& IsTextureAvailable, int& Width, int& Height, TArray<uint8>& Image_Buffer, bool& True_For_RGBA_False_For_RGB)
{
	IsTextureAvailable = false;
	True_For_RGBA_False_For_RGB = true;
#ifndef ENABLE_SCREEN_CAPTURE
	return;
#else
	//if (Image_Raw_Buffer != nullptr)
	//{
	//	delete Image_Raw_Buffer;
	//	Image_Raw_Buffer = nullptr;
	//}

	if (!(Capture_Screen_API_REUSE(Current_Monitor_Index, Screen_Capture_Width, Screen_Capture_Height, (&((unsigned char*)Image_Raw_Buffer)))))
	{
		return;
	}

	if (Image_Raw_Buffer != nullptr)
	{
		IsTextureAvailable = true;
		Width = Screen_Capture_Width;
		Height = Screen_Capture_Height;
		Image_Buffer.Init(0, Width * Height * 4);
		FMemory::Memcpy(Image_Buffer.GetData(), Image_Raw_Buffer, Width * Height * 4);
	}
	return;
#endif
}

void UWrexa_Screen_Capture::Image_From_Screen_Capture_Pointer(bool& IsTextureAvailable, int& Width, int& Height, uint8** Image_Buffer, bool& True_For_RGBA_False_For_RGB)
{
	IsTextureAvailable = false;
	True_For_RGBA_False_For_RGB = true;
#ifndef ENABLE_SCREEN_CAPTURE
	return;
#else
	//if (Image_Raw_Buffer != nullptr)
	//{
	//	delete Image_Raw_Buffer;
	//	Image_Raw_Buffer = nullptr;
	//}

	//if (!(Capture_Screen_API_REUSE(Current_Monitor_Index, Screen_Capture_Width, Screen_Capture_Height, (&((unsigned char*)Image_Raw_Buffer)))))
	//{
	//	return;
	//}

	//if (Image_Raw_Buffer != nullptr)
	//{
	//	IsTextureAvailable = true;
	//	Width = Screen_Capture_Width;
	//	Height = Screen_Capture_Height;
	//	(*Image_Buffer) = Image_Raw_Buffer;
	//}

	IsTextureAvailable = captureDesktop_DX_REUSE_Unsafe(Current_Monitor_Index, Width, Height, 0, 0, Image_Buffer);
	if (IsTextureAvailable)
	{
		Screen_Capture_Width = Width;
		Screen_Capture_Height = Height;
	}
	return;
#endif
}

void UWrexa_Screen_Capture::Image_From_Screen_Capture_Pointer_Unsafe(bool& IsTextureAvailable, int& Return_Width, int& Return_Height, int Resized_Width, int Resized_Height, uint8** Image_Buffer)
{
#ifndef ENABLE_SCREEN_CAPTURE
	IsTextureAvailable = false;
	return;
#else
	IsTextureAvailable = captureDesktop_DX_REUSE_Unsafe(Current_Monitor_Index, Return_Width, Return_Height, Resized_Width, Resized_Height, Image_Buffer);
	return;
#endif
}

bool UWrexa_Screen_Capture::Request_Get_Frame(UWrexa_Instance* Reference_Of_Main_Game_Instance, const bool& Is_GPU_Capture)
{
#ifndef ENABLE_SCREEN_CAPTURE
	return false;
#else
	return Singleton_Screen_Capture_Thread::Get_Frame(Reference_Of_Main_Game_Instance, Current_Monitor_Index, Is_GPU_Capture);
#endif
}

bool UWrexa_Screen_Capture::Request_Get_Frame_With_Config(UWrexa_Instance* Reference_Of_Main_Game_Instance, const bool& Is_GPU_Capture, const bool& Should_Resize_And_JPEG, const int& Arg_JPEG_Quality_Percent, const EResize_Alogrithm_Type& Arg_Resize_Algorithm_Type, const int& Arg_Resize_Width, const int& Arg_Resize_Height)
{
#ifndef ENABLE_SCREEN_CAPTURE
	return false;
#else
	return Singleton_Screen_Capture_Thread::Get_Frame_With_Config(Reference_Of_Main_Game_Instance, Current_Monitor_Index, Is_GPU_Capture, Should_Resize_And_JPEG, Arg_JPEG_Quality_Percent, Arg_Resize_Algorithm_Type, Arg_Resize_Width, Arg_Resize_Height);
#endif
}

//FScreen_Share_Thread::FScreen_Share_Thread(AWrexa_Player_Controller* Reference_Of_Target_Player_Controller)
//{
//	Target_Player_Controller = Reference_Of_Target_Player_Controller;
//	Thread = FRunnableThread::Create(this, TEXT("FScreen_Share_Thread"), 0, TPri_AboveNormal); //windows default = 8mb for thread, could specify more
//}

FScreen_Share_Thread::FScreen_Share_Thread(UWrexa_Instance* Reference_Of_Main_Game_Instance)
{
	Main_Game_Instance = Reference_Of_Main_Game_Instance;
	Thread = FRunnableThread::Create(this, TEXT("FScreen_Share_Thread"), 0, TPri_AboveNormal); //windows default = 8mb for thread, could specify more
}

FScreen_Share_Thread::~FScreen_Share_Thread()
{
	Safely_Destroy();
}

void FScreen_Share_Thread::Safely_Destroy()
{
	if (Is_Screen_Capture_Thread_Running)
	{
		Is_Screen_Capture_Thread_Running = false;
		//Is_Screen_Capture_Thread_Stop_Requested = false;
	}

	while (IsAccesingArray)
	{
		FPlatformProcess::Sleep(0.1);
	}
	IsAccesingArray = true;
	
	RGBA_Buffer.Empty();
	IsAccesingArray = false;	

	Shutdown();
}

//Init
bool FScreen_Share_Thread::Init()
{
	//Init the Data	
	IsAccesingArray = false;

	// Multi Threaded GPU Screen Capture
	Is_Currently_GPU_Capture = true;
	//std::atomic<bool> Is_Requesting_GPU_Capture = true;
	Is_Screen_Capture_Thread_Running = false;
	//Is_Screen_Capture_Thread_Stop_Requested = false;
	Is_Capture_Current_Frame = false;
	return true;
}

//Run
uint32 FScreen_Share_Thread::Run()
{
	//Initial wait before starting
	FPlatformProcess::Sleep(0.03);

	Is_Screen_Capture_Thread_Running = true;

	// While not told to stop this thread 
	// and not yet finished
	while (StopTaskCounter.GetValue() == 0)
	{		
		//When Not Modifying
		if (!Is_Capture_Current_Frame)
		{
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			//prevent thread from using too many resources
			FPlatformProcess::Sleep(0.05);
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~			
		}
		else
		{
			if (!Is_Screen_Capture_Thread_Running)
			{
				break;
			}
#ifdef ENABLE_SCREEN_CAPTURE
			if (UWrexa_Screen_Capture::Get_Total_Number_Of_Monitors_API() < Current_Monitor_Index.GetValue())
			{
				Is_Capture_Current_Frame = false;
				continue;
			}
			while (IsAccesingArray)
			{
				FPlatformProcess::Sleep(0.1);
			}
			IsAccesingArray = true;
#ifdef _WIN32
#ifdef USE_DX_SCREEN_CAP
			if (Is_Currently_GPU_Capture)
#else
			if (false)
#endif
			{
#ifdef USE_DX_SCREEN_CAP
				unsigned char *Image_Buffer = nullptr;
				bool IsTextureAvailable = UWrexa_Screen_Capture::captureDesktop_DX_REUSE_Unsafe(Current_Monitor_Index.GetValue(), Width, Height, 0, 0, &Image_Buffer);
				if (IsTextureAvailable)
				{
					if (RGBA_Buffer.Num() != (Width * Height * 4))
					{
						RGBA_Buffer.Init(0, (Width * Height * 4));
					}

					FMemory::Memcpy((void*)(RGBA_Buffer.GetData()), Image_Buffer, (Width * Height * 4));
				}
#endif
			}
			else
			{
				std::vector<HMONITOR> monitors;
				if (!(EnumDisplayMonitors(nullptr, nullptr, UWrexa_Screen_Capture::MonitorEnumProc, reinterpret_cast<LPARAM>(&monitors))))
				{
					IsAccesingArray = false;
					Is_Capture_Current_Frame = false;
					continue;
				}

				HMONITOR hMonitor = monitors[Current_Monitor_Index.GetValue()];

				// Get the monitor info
				MONITORINFO monitorInfo;
				monitorInfo.cbSize = sizeof(monitorInfo);
				GetMonitorInfo(hMonitor, &monitorInfo);

				// Get the screen dimensions
				int New_width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
				int New_height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

				if (RGBA_Buffer.Num() != (New_width * New_height * 4))
				{
					RGBA_Buffer.Init(0, (New_width * New_height * 4));
				}
				Width = New_width;
				Height = New_height;

				// Create a device context for the entire screen
				HDC hdcScreen = GetDC(NULL);

				// Create a memory device context and bitmap to store the screen capture
				HDC hdcMemDC = CreateCompatibleDC(hdcScreen);
				HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen, Width, Height);
				HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMemDC, hbmScreen);

				// Capture the screen and store it in the bitmap
				BitBlt(hdcMemDC, 0, 0, Width, Height, hdcScreen, 0, 0, SRCCOPY);
				//(*RGBA_Buffer) = new unsigned char[width * height * 4];
				BITMAPINFOHEADER bi = { sizeof(BITMAPINFOHEADER), Width, -Height, 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
				GetDIBits(hdcMemDC, hbmScreen, 0, Height, (RGBA_Buffer.GetData()), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

				// Clean up
				SelectObject(hdcMemDC, hbmOld);
				DeleteDC(hdcMemDC);
				ReleaseDC(NULL, hdcScreen);
			}
#endif
#endif
			if (Is_Resize_And_JPEG)
			{
				bool Is_Successful = false;
				TArray<uint8> Raw_JPEG_Buffer;

				int Current_Resize_Width = Resize_Width.GetValue();
				int Current_Resize_Height = Resize_Height.GetValue();

				if (RGBA_Buffer.Num() > 0)
				{
					

					if ((Current_Resize_Width > 0) && (Current_Resize_Height > 0))
					{
						UWrexa_Blueprint_Functions::LoadRawJPGBufferFromRGBA_Buffer(RGBA_Buffer, Width, Height, Raw_JPEG_Buffer, Is_Successful, Current_Resize_Width, Current_Resize_Height, static_cast<EResize_Alogrithm_Type>(Resize_Algorithm_Type.GetValue()), true, JPEG_Quality_Percent.GetValue());

						//Width = Current_Resize_Width;
						//Height = Current_Resize_Height;
					}
				}

				if (Is_Successful)
				{
					if (Is_Screen_Capture_Thread_Running)
					{
						AsyncTask(ENamedThreads::GameThread, [=]()
							{
								//Target_Player_Controller->On_New_Screen_Frame_Received(RGBA_Buffer, Width, Height);
								Singleton_Screen_Capture_Thread* This_Inst = Singleton_Screen_Capture_Thread::GetInstance();
								if (This_Inst->Is_Game_Thread_Available_For_Access)
								{
									if (IsValid(Main_Game_Instance))
									{
										Main_Game_Instance->On_New_Screen_Frame_Received(Raw_JPEG_Buffer, Current_Resize_Width, Current_Resize_Height);
									}
								}
							});
					}
				}
			}
			else
			{
				if (Is_Screen_Capture_Thread_Running)
				{
					AsyncTask(ENamedThreads::GameThread, [=]()
						{
							//Target_Player_Controller->On_New_Screen_Frame_Received(RGBA_Buffer, Width, Height);
							Singleton_Screen_Capture_Thread* This_Inst = Singleton_Screen_Capture_Thread::GetInstance();
							if (This_Inst->Is_Game_Thread_Available_For_Access)
							{
								if (IsValid(Main_Game_Instance))
								{
									Main_Game_Instance->On_New_Screen_Frame_Received(RGBA_Buffer, Width, Height);
								}
							}
						});
				}
			}

			IsAccesingArray = false;
			Is_Capture_Current_Frame = false;

			if (!Is_Screen_Capture_Thread_Running)
			{
				break;
			}
		}
	}

	Is_Screen_Capture_Thread_Running = false;
	return 0;
}

//stop
void FScreen_Share_Thread::Stop()
{
	StopTaskCounter.Increment();
}

// Construction Helper
//FScreen_Share_Thread* FScreen_Share_Thread::Create_Screen_Capture_Thread(AWrexa_Player_Controller* Reference_Of_Target_Player_Controller)
//{
//	if (Reference_Of_Target_Player_Controller == nullptr)
//	{
//		return nullptr;
//	}
//
//	return new FScreen_Share_Thread(Reference_Of_Target_Player_Controller);
//}

FScreen_Share_Thread* FScreen_Share_Thread::Create_Screen_Capture_Thread(UWrexa_Instance* Reference_Of_Main_Game_Instance)
{
	if (Reference_Of_Main_Game_Instance == nullptr)
	{
		return nullptr;
	}

	return new FScreen_Share_Thread(Reference_Of_Main_Game_Instance);
}

//~~~ Thread Core Functions ~~~
void FScreen_Share_Thread::Change_Screen_Capture_Type(bool Is_GPU_Capture)
{
	if (Is_Screen_Capture_Thread_Running)
	{
		Is_Currently_GPU_Capture = Is_GPU_Capture;
	}
}

void FScreen_Share_Thread::Should_Resize_And_Convert_To_JPEG(bool Should_Resize_And_JPEG, int Arg_JPEG_Quality_Percent, EResize_Alogrithm_Type Arg_Resize_Algorithm_Type, int Arg_Resize_Width, int Arg_Resize_Height)
{
	if (Is_Screen_Capture_Thread_Running)
	{
		Resize_Width.Set(Arg_Resize_Width);
		Resize_Height.Set(Arg_Resize_Height);
		JPEG_Quality_Percent.Set(FMath::Clamp(Arg_JPEG_Quality_Percent,1,100));
		Resize_Algorithm_Type.Set(Arg_Resize_Algorithm_Type);
		Is_Resize_And_JPEG = Should_Resize_And_JPEG;
	}
}

void FScreen_Share_Thread::Request_Frame(int Capture_Monitor_Index)
{
	if (Is_Screen_Capture_Thread_Running)
	{
		if (!Is_Capture_Current_Frame)
		{
			Current_Monitor_Index.Set(Capture_Monitor_Index);
			Is_Capture_Current_Frame = true;
		}
	}
}

void FScreen_Share_Thread::Request_Frame_And_Set_Screen_Capture_Type(int Capture_Monitor_Index, bool Is_GPU_Capture)
{
	if (Is_Screen_Capture_Thread_Running)
	{
		if (!Is_Capture_Current_Frame)
		{
			Current_Monitor_Index.Set(Capture_Monitor_Index);
			Is_Currently_GPU_Capture = Is_GPU_Capture;
			Is_Capture_Current_Frame = true;
		}
	}
}

void FScreen_Share_Thread::Request_Frame_And_Set_Config(int Capture_Monitor_Index, bool Is_GPU_Capture, bool Should_Resize_And_JPEG, int Arg_JPEG_Quality_Percent, EResize_Alogrithm_Type Arg_Resize_Algorithm_Type, int Arg_Resize_Width, int Arg_Resize_Height)
{
	if (Is_Screen_Capture_Thread_Running)
	{
		if (!Is_Capture_Current_Frame)
		{
			Current_Monitor_Index.Set(Capture_Monitor_Index);
			Is_Currently_GPU_Capture = Is_GPU_Capture;
			Resize_Width.Set(Arg_Resize_Width);
			Resize_Height.Set(Arg_Resize_Height);
			JPEG_Quality_Percent.Set(FMath::Clamp(Arg_JPEG_Quality_Percent, 1, 100));
			Resize_Algorithm_Type.Set(Arg_Resize_Algorithm_Type);
			Is_Resize_And_JPEG = Should_Resize_And_JPEG;
			Is_Capture_Current_Frame = true;
		}
	}
}

void FScreen_Share_Thread::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

void FScreen_Share_Thread::Shutdown()
{
	if (Runnable != nullptr)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = nullptr;
	}
}

bool FScreen_Share_Thread::IsThreadFinished()
{
	//if (Runnable) return Runnable->IsFinished;
	return true;
}

Singleton_Screen_Capture_Thread::Singleton_Screen_Capture_Thread()
{
	if (Screen_Capture_Thread_Ptr == nullptr)
	{
		Screen_Capture_Thread_Ptr = nullptr;
	}
}

Singleton_Screen_Capture_Thread* Singleton_Screen_Capture_Thread::GetInstance()
{
#if !UE_SERVER
	static Singleton_Screen_Capture_Thread Singleton_Thread;
	return &Singleton_Thread;
#else
	return nullptr;
#endif
}

Singleton_Screen_Capture_Thread::~Singleton_Screen_Capture_Thread()
{
	if (Screen_Capture_Thread_Ptr != nullptr)
	{
		Screen_Capture_Thread_Ptr->Safely_Destroy();
		Screen_Capture_Thread_Ptr = nullptr;
	}
}

void Singleton_Screen_Capture_Thread::Stop_Externally()
{
#if !UE_SERVER
	Singleton_Screen_Capture_Thread* Singleton_Ref = GetInstance();
	Singleton_Ref->Is_Game_Thread_Available_For_Access.AtomicSet(false);
	if (Singleton_Ref->Screen_Capture_Thread_Ptr != nullptr)
	{
		Singleton_Ref->Screen_Capture_Thread_Ptr->Safely_Destroy();
		Singleton_Ref->Screen_Capture_Thread_Ptr = nullptr;
	}
#else
	return;
#endif
}

//bool Singleton_Screen_Capture_Thread::Get_Frame(AWrexa_Player_Controller* Reference_Of_Target_Player_Controller, int Monitor_Index, const bool& Is_GPU_Capture)
//{
//	Singleton_Screen_Capture_Thread& This_Inst = GetInstance();
//
//	if (This_Inst.Screen_Capture_Thread_Ptr == nullptr)
//	{		
//		if (Reference_Of_Target_Player_Controller != nullptr)
//		{
//			This_Inst.Screen_Capture_Thread_Ptr = FScreen_Share_Thread::Create_Screen_Capture_Thread(Reference_Of_Target_Player_Controller);
//		}		
//
//		if (This_Inst.Screen_Capture_Thread_Ptr == nullptr)
//		{
//			return false;
//		}
//	}
//
//	This_Inst.Screen_Capture_Thread_Ptr->Request_Frame_And_Set_Screen_Capture_Type(Monitor_Index, Is_GPU_Capture);
//	return true;
//}

bool Singleton_Screen_Capture_Thread::Get_Frame(UWrexa_Instance* Reference_Of_Main_Game_Instance, int Monitor_Index, const bool& Is_GPU_Capture)
{
#if !UE_SERVER
	Singleton_Screen_Capture_Thread* This_Inst = GetInstance();

	if (Reference_Of_Main_Game_Instance == nullptr)
	{
		return false;
	}

	if (This_Inst->Screen_Capture_Thread_Ptr == nullptr)
	{
		This_Inst->Screen_Capture_Thread_Ptr = FScreen_Share_Thread::Create_Screen_Capture_Thread(Reference_Of_Main_Game_Instance);

		if (This_Inst->Screen_Capture_Thread_Ptr == nullptr)
		{
			return false;
		}
		else
		{
			This_Inst->Is_Game_Thread_Available_For_Access.AtomicSet(true);
			FPlatformProcess::Sleep(0.1);
		}
	}

	This_Inst->Screen_Capture_Thread_Ptr->Request_Frame_And_Set_Screen_Capture_Type(Monitor_Index, Is_GPU_Capture);
	return true;
#else
	return false;
#endif
}

bool Singleton_Screen_Capture_Thread::Get_Frame_With_Config(UWrexa_Instance* Reference_Of_Main_Game_Instance, int Capture_Monitor_Index, const bool& Is_GPU_Capture, const bool& Should_Resize_And_JPEG, const int& Arg_JPEG_Quality_Percent, const EResize_Alogrithm_Type& Arg_Resize_Algorithm_Type, const int& Arg_Resize_Width, const int& Arg_Resize_Height)
{
#if !UE_SERVER
	Singleton_Screen_Capture_Thread* This_Inst = GetInstance();

	if (Reference_Of_Main_Game_Instance == nullptr)
	{
		return false;
	}

	if (This_Inst->Screen_Capture_Thread_Ptr == nullptr)
	{
		This_Inst->Screen_Capture_Thread_Ptr = FScreen_Share_Thread::Create_Screen_Capture_Thread(Reference_Of_Main_Game_Instance);

		if (This_Inst->Screen_Capture_Thread_Ptr == nullptr)
		{
			return false;
		}
		else
		{
			This_Inst->Is_Game_Thread_Available_For_Access.AtomicSet(true);
			FPlatformProcess::Sleep(0.1);
		}
	}

	This_Inst->Screen_Capture_Thread_Ptr->Request_Frame_And_Set_Config(Capture_Monitor_Index, Is_GPU_Capture, Should_Resize_And_JPEG, Arg_JPEG_Quality_Percent, Arg_Resize_Algorithm_Type, Arg_Resize_Width, Arg_Resize_Height);
	return true;
#else
	return false;
#endif
}