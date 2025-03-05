// Developed By Wrexa Technologies 2023

#include "Wrexa_Blueprint_Functions.h"

// Encoding
#include "Misc/Base64.h"

// Extra Functions
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

// Shared Pointer
#include "Templates/SharedPointer.h"

// Widget Rendering
#include "Widgets/SWidget.h"
#include "Blueprint/UserWidget.h"
#include "Slate/WidgetRenderer.h"
#include "Engine/TextureRenderTarget2D.h"

// Socket Functions
#include "SocketSubsystem.h"

// Local Player Functions
#include "Engine/LocalPlayer.h"

// GameInstance Functions
#include "Engine/GameInstance.h"

// HTTP Functions
//#include "Wrexa_WebFileDownloader.h"

// Image Processing
#include "ImageUtils.h"
//#pragma optimize("", off)
//Networking
FString UWrexa_Blueprint_Functions::Querry_DNS_String(FString a_ServerAddress)
{
	ISocketSubsystem* const SocketSubSystem = ISocketSubsystem::Get();

	//FString IP = ServerAddress.ToString();
	if (SocketSubSystem)
	{
		auto ResolveInfo = SocketSubSystem->GetHostByName(TCHAR_TO_ANSI(*a_ServerAddress));
		while (!ResolveInfo->IsComplete());

		if (ResolveInfo->GetErrorCode() == 0)
		{
			const FInternetAddr* Addr = &ResolveInfo->GetResolvedAddress();
			uint32 OutIP = 0;
			Addr->GetIp(OutIP);
			a_ServerAddress = FString::FromInt((0xff & (OutIP >> 24)));
			a_ServerAddress += ".";
			a_ServerAddress += FString::FromInt((0xff & (OutIP >> 16)));
			a_ServerAddress += ".";
			a_ServerAddress += FString::FromInt((0xff & (OutIP >> 8)));
			a_ServerAddress += ".";
			a_ServerAddress += FString::FromInt((0xff & OutIP));
			return a_ServerAddress;
		}
	}
	return "null";
}

FString UWrexa_Blueprint_Functions::Querry_DNS(FName a_ServerAddress)
{
	return Querry_DNS_String(a_ServerAddress.ToString());
}

//Binary
FString UWrexa_Blueprint_Functions::ObjectBytesToString(TArray<uint8> In, int Count)
{
	return FBase64::Encode(In);
}

FString UWrexa_Blueprint_Functions::SaveGameObjectToBinaryToString(USaveGame* SaveGameObject)
{
	TArray<uint8> ObjectBytes;
	if (UGameplayStatics::SaveGameToMemory(SaveGameObject, ObjectBytes))
	{
		return ObjectBytesToString(ObjectBytes, ObjectBytes.Num());
	}
	//When Failed
	return "";
}

USaveGame* UWrexa_Blueprint_Functions::StringToSaveGameObject(FString Input)
{
	TArray<uint8> ObjectBytes;
	FBase64::Decode(Input, ObjectBytes);
	return UGameplayStatics::LoadGameFromMemory(ObjectBytes);
}

// Get the texture data from the Texture2D
void UWrexa_Blueprint_Functions::GetTextureData(const UTexture2D* Texture, TArray<uint8>& Pixels, int& Width, int& Height)
{
	// Get the resource object for the texture

	// Lock the texture data so we can read it
	TArray<FColor> TextureData;
	FTexture2DMipMap& Mip = Texture->PlatformData->Mips[0];
	TextureData.Empty(Mip.SizeX * Mip.SizeY);
	FColor* DataPtr = static_cast<FColor*>(Mip.BulkData.Lock(LOCK_READ_ONLY));
	for (int32 i = 0; i < Mip.SizeX * Mip.SizeY; ++i)
	{
		TextureData.Add(DataPtr[i]);
	}
	Mip.BulkData.Unlock();

	// Copy the texture data into a raw uint8 array
	Pixels.Init(0, TextureData.Num() * 4);
	for (int32 i = 0; i < TextureData.Num(); ++i)
	{
		Pixels[i * 4 + 0] = TextureData[i].B;
		Pixels[i * 4 + 1] = TextureData[i].G;
		Pixels[i * 4 + 2] = TextureData[i].R;
		Pixels[i * 4 + 3] = TextureData[i].A;
	}

	// Set the width and height
	Width = Mip.SizeX;
	Height = Mip.SizeY;
}

void UWrexa_Blueprint_Functions::GetTextureData_RGB_Array(const UTexture2D* Texture, TArray<uint8>& RED_Channel, TArray<uint8>& GREEN_Channel, TArray<uint8>& BLUE_Channel, int& Width, int& Height)
{
	// Get the resource object for the texture

	// Lock the texture data so we can read it
	TArray<FColor> TextureData;
	FTexture2DMipMap& Mip = Texture->PlatformData->Mips[0];
	TextureData.Empty(Mip.SizeX * Mip.SizeY);
	FColor* DataPtr = static_cast<FColor*>(Mip.BulkData.Lock(LOCK_READ_ONLY));
	for (int32 i = 0; i < Mip.SizeX * Mip.SizeY; ++i)
	{
		TextureData.Add(DataPtr[i]);
	}
	Mip.BulkData.Unlock();

	// Copy the texture data into a raw uint8 array
	RED_Channel.Init(0, TextureData.Num());
	GREEN_Channel.Init(0, TextureData.Num());
	BLUE_Channel.Init(0, TextureData.Num());
	for (int32 i = 0; i < TextureData.Num(); ++i)
	{
		RED_Channel[i] = TextureData[i].R;
		RED_Channel[i] = TextureData[i].G;
		RED_Channel[i] = TextureData[i].B;
	}

	// Set the width and height
	Width = Mip.SizeX;
	Height = Mip.SizeY;
}

UTexture2D* UWrexa_Blueprint_Functions::TextureFrom_RGBA_Image(const bool& Swap_Colour_Channel, const int& Width, const int& Height, const TArray<uint8>& RGBA_Image_Buffer)
{
	//UTextureRenderTarget2D* TextureRenderTarget = WidgetRenderer->DrawWidget(SlateWidget.ToSharedRef(), DrawSize);
	// Creates Texture2D to store RenderTexture content
	//UTexture2D* Texture = UTexture2D::CreateTransient(DrawSize.X, DrawSize.Y, PF_B8G8R8A8);
	//Screen_imgbuffer.resize(Capturing_Monitor.Width * Capturing_Monitor.Height * sizeof(SL::Screen_Capture::ImageBGRA), 0); // create a Black image to start with

	UTexture2D* Captured_Texture = UTexture2D::CreateTransient(Width, Height, Swap_Colour_Channel ? PF_B8G8R8A8 : EPixelFormat::PF_R8G8B8A8); //, TEXT("Screen_Share_Texture"));
#if WITH_EDITORONLY_DATA
	Captured_Texture->MipGenSettings = TMGS_NoMipmaps;
#endif
	// RGBA to Texture2D
	size_t Image_Total_Size = Width * Height * 4;
	uint8* MipData = (uint8*)(Captured_Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	for (size_t i = 0; i < Image_Total_Size; ++i)
	{
		MipData[i] = RGBA_Image_Buffer[i];
	}
	//FMemory::Memcpy(MipData, (void*)(Screen_imgbuffer.GetData()), Capturing_Monitor.Width * Capturing_Monitor.Height * 4);

	//Captured_Texture->Rename(TEXT("Screen_Share_Texture")); // works in UMG too!
	Captured_Texture->PlatformData->Mips[0].BulkData.Unlock();
	Captured_Texture->UpdateResource();


	return Captured_Texture;
}

UTexture2D* UWrexa_Blueprint_Functions::TextureFrom_RGB_Image(const bool& Swap_Colour_Channel, const int& Width, const int& Height, const TArray<uint8>& RED_Channel, const TArray<uint8>& GREEN_Channel, const TArray<uint8>& BLUE_Channel)
{
	UTexture2D* Captured_Texture = UTexture2D::CreateTransient(Width, Height, Swap_Colour_Channel ? PF_B8G8R8A8 : EPixelFormat::PF_R8G8B8A8); //, TEXT("Screen_Share_Texture"));
#if WITH_EDITORONLY_DATA
	Captured_Texture->MipGenSettings = TMGS_NoMipmaps;
#endif
	// RGBA to Texture2D
	size_t Image_Size = Width * Height;
	uint8* MipData = (uint8*)(Captured_Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	for (size_t i = 0; i < Image_Size; ++i)
	{
		MipData[(i * 4) + 0] = RED_Channel[i];
		MipData[(i * 4) + 1] = GREEN_Channel[i];
		MipData[(i * 4) + 2] = BLUE_Channel[i];
		MipData[(i * 4) + 3] = 0;
	}

	//Captured_Texture->Rename(TEXT("Screen_Share_Texture")); // works in UMG too!
	Captured_Texture->PlatformData->Mips[0].BulkData.Unlock();
	Captured_Texture->UpdateResource();


	return Captured_Texture;
}

UTexture2D* UWrexa_Blueprint_Functions::TextureFrom_RGBA_Image_From_Pointer(const bool& Swap_Colour_Channel, const int& Width, const int& Height, uint8* RGBA_Image_Buffer)
{
	//UTextureRenderTarget2D* TextureRenderTarget = WidgetRenderer->DrawWidget(SlateWidget.ToSharedRef(), DrawSize);
// Creates Texture2D to store RenderTexture content
//UTexture2D* Texture = UTexture2D::CreateTransient(DrawSize.X, DrawSize.Y, PF_B8G8R8A8);
//Screen_imgbuffer.resize(Capturing_Monitor.Width * Capturing_Monitor.Height * sizeof(SL::Screen_Capture::ImageBGRA), 0); // create a Black image to start with

	UTexture2D* Captured_Texture = UTexture2D::CreateTransient(Width, Height, Swap_Colour_Channel ? PF_B8G8R8A8 : EPixelFormat::PF_R8G8B8A8); //, TEXT("Screen_Share_Texture"));
#if WITH_EDITORONLY_DATA
	Captured_Texture->MipGenSettings = TMGS_NoMipmaps;
#endif
	// RGBA to Texture2D
	size_t Image_Total_Size = Width * Height * 4;
	uint8* MipData = (uint8*)(Captured_Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	for (size_t i = 0; i < Image_Total_Size; ++i)
	{
		MipData[i] = (*(RGBA_Image_Buffer + i));
	}
	//FMemory::Memcpy(MipData, (void*)(Screen_imgbuffer.GetData()), Capturing_Monitor.Width * Capturing_Monitor.Height * 4);

	//Captured_Texture->Rename(TEXT("Screen_Share_Texture")); // works in UMG too!
	Captured_Texture->PlatformData->Mips[0].BulkData.Unlock();
	Captured_Texture->UpdateResource();


	return Captured_Texture;
}

UTexture2D* UWrexa_Blueprint_Functions::TextureFrom_RGB_Image_Single_Buffer(const bool& Swap_Colour_Channel, const int& Width, const int& Height, const TArray<uint8>& RGB_Image_Buffer)
{
	UTexture2D* Captured_Texture = UTexture2D::CreateTransient(Width, Height, Swap_Colour_Channel ? PF_B8G8R8A8 : EPixelFormat::PF_R8G8B8A8); //, TEXT("Screen_Share_Texture"));
#if WITH_EDITORONLY_DATA
	Captured_Texture->MipGenSettings = TMGS_NoMipmaps;
#endif
	// RGBA to Texture2D
	size_t Image_Size = Width * Height;
	uint8* MipData = (uint8*)(Captured_Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	for (size_t i = 0; i < Image_Size; ++i)
	{
		MipData[(i * 4) + 0] = RGB_Image_Buffer[(i * 3) + 0];
		MipData[(i * 4) + 1] = RGB_Image_Buffer[(i * 3) + 1];
		MipData[(i * 4) + 2] = RGB_Image_Buffer[(i * 3) + 2];
		MipData[(i * 4) + 3] = 0;
	}

	//Captured_Texture->Rename(TEXT("Screen_Share_Texture")); // works in UMG too!
	Captured_Texture->PlatformData->Mips[0].BulkData.Unlock();
	Captured_Texture->UpdateResource();


	return Captured_Texture;
}

UTexture2D* UWrexa_Blueprint_Functions::TextureFrom_JPGE_Image(const bool& Swap_Colour_Channel, const TArray<uint8>& JPG_Image_Buffer)
{
	bool Is_Successful = false;
	TArray<uint8> RGBA_Image_Buffer;
	int Width = 0;
	int Height = 0;

	return TextureFrom_Image(false, Width, Height, JPG_Image_Buffer, EJoyImageFormats::JPG);
}

UTexture2D* UWrexa_Blueprint_Functions::TextureFrom_Image(const bool& Swap_Colour_Channel, int& Return_Width, int& Return_Height, const TArray<uint8>& Image_Buffer, EJoyImageFormats ImageFormat)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr< IImageWrapper > ImageWrapper = ImageWrapperModule.CreateImageWrapper(GetJoyImageFormat(ImageFormat));

	if (!(ImageWrapper.IsValid() && ImageWrapper->SetCompressed(Image_Buffer.GetData(), Image_Buffer.Num())))
	{
		return NULL;
	}

	TArray<uint8> UncompressedBGRA;
	if (!(ImageWrapper.Get()->GetRaw(ERGBFormat::RGBA, 8, UncompressedBGRA)))
	{
		return NULL;
	}

	Return_Width = ImageWrapper->GetWidth();
	Return_Height = ImageWrapper->GetHeight();

	UTexture2D* Captured_Texture = UTexture2D::CreateTransient(Return_Width, Return_Height, Swap_Colour_Channel ? PF_B8G8R8A8 : EPixelFormat::PF_R8G8B8A8); //, TEXT("Screen_Share_Texture"));
#if WITH_EDITORONLY_DATA
	Captured_Texture->MipGenSettings = TMGS_NoMipmaps;
#endif
	// RGBA to Texture2D
	size_t Image_Size = Return_Width * Return_Height;
	uint8* MipData = (uint8*)(Captured_Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	for (size_t i = 0; i < Image_Size; ++i)
	{
		MipData[(i * 4) + 0] = UncompressedBGRA[(i * 4) + 0];
		MipData[(i * 4) + 1] = UncompressedBGRA[(i * 4) + 1];
		MipData[(i * 4) + 2] = UncompressedBGRA[(i * 4) + 2];
		MipData[(i * 4) + 3] = 0;
	}

	//Captured_Texture->Rename(TEXT("Screen_Share_Texture")); // works in UMG too!
	Captured_Texture->PlatformData->Mips[0].BulkData.Unlock();
	Captured_Texture->UpdateResource();


	return Captured_Texture;
}

bool UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_RGBA_By_Pointer(const TArray<uint8>& Raw_Data, const int& Max_Array_Size, int& Total_Fragments, FRGB_Fragment_Map* Fragmented_RGB_Data_Map)
{
	if ((Raw_Data.Num() <= 0) || (Max_Array_Size == 0))
	{
		return false;
	}
	//Fragmented_Data.Unique_Index = Unique_Index;

	Total_Fragments = 1 + (((Raw_Data.Num() / 4) - 1) / Max_Array_Size);
	bool Has_Remainder = Total_Fragments != ((Raw_Data.Num() / 4) / Max_Array_Size);

	Fragmented_RGB_Data_Map->Expected_Total_Fragments = Total_Fragments;
	Fragmented_RGB_Data_Map->Expected_Each_Fragment_Size = Max_Array_Size;
	Fragmented_RGB_Data_Map->Fragments_Sent = 0;

	Fragmented_RGB_Data_Map->RED_Channel.Init(0, Raw_Data.Num() / 4);//Colour Black
	Fragmented_RGB_Data_Map->GREEN_Channel.Init(0, Raw_Data.Num() / 4);//Colour Black
	Fragmented_RGB_Data_Map->BLUE_Channel.Init(0, Raw_Data.Num() / 4);//Colour Black

	int RGBA_Index = 0;
	for (int i = 0; i < (int)(Raw_Data.Num() / 4); ++i)
	{
		RGBA_Index = i * 4;
		Fragmented_RGB_Data_Map->RED_Channel[i] = Raw_Data[RGBA_Index + 0];
		Fragmented_RGB_Data_Map->GREEN_Channel[i] = Raw_Data[RGBA_Index + 1];
		Fragmented_RGB_Data_Map->BLUE_Channel[i] = Raw_Data[RGBA_Index + 2];
	}

	return true;
}

bool UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_RGB_By_Pointer(const TArray<uint8>& Raw_Data, const int& Max_Array_Size, int& Total_Fragments, FRGB_Fragment_Map* Fragmented_RGB_Data_Map)
{
	if ((Raw_Data.Num() <= 0) || (Max_Array_Size == 0))
	{
		return false;
	}
	//Fragmented_Data.Unique_Index = Unique_Index;

	Total_Fragments = 1 + (((Raw_Data.Num() / 3) - 1) / Max_Array_Size);
	bool Has_Remainder = Total_Fragments != ((Raw_Data.Num() / 3) / Max_Array_Size);

	Fragmented_RGB_Data_Map->Expected_Total_Fragments = Total_Fragments;
	Fragmented_RGB_Data_Map->Expected_Each_Fragment_Size = Max_Array_Size;
	Fragmented_RGB_Data_Map->Fragments_Sent = 0;

	Fragmented_RGB_Data_Map->RED_Channel.Init(0, Raw_Data.Num() / 3);//Colour Black
	Fragmented_RGB_Data_Map->GREEN_Channel.Init(0, Raw_Data.Num() / 3);//Colour Black
	Fragmented_RGB_Data_Map->BLUE_Channel.Init(0, Raw_Data.Num() / 3);//Colour Black

	int RGB_Index = 0;
	for (int i = 0; i < (int)(Raw_Data.Num() / 3); ++i)
	{
		RGB_Index = i * 3;
		Fragmented_RGB_Data_Map->RED_Channel[i] = Raw_Data[RGB_Index + 0];
		Fragmented_RGB_Data_Map->GREEN_Channel[i] = Raw_Data[RGB_Index + 1];
		Fragmented_RGB_Data_Map->BLUE_Channel[i] = Raw_Data[RGB_Index + 2];
	}

	return true;
}

bool UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_JPGE_By_Pointer(const TArray<uint8>& Raw_Data, const int& Max_Array_Size, int& Total_Fragments, FJPEG_Fragment_Struct* Fragmented_JPGE_Data_Map)
{
	if ((Raw_Data.Num() <= 0) || (Max_Array_Size == 0))
	{
		return false;
	}
	//Fragmented_Data.Unique_Index = Unique_Index;

	Total_Fragments = 1 + ((Raw_Data.Num() - 1) / Max_Array_Size);
	bool Has_Remainder = Total_Fragments != (Raw_Data.Num() / Max_Array_Size);

	Fragmented_JPGE_Data_Map->Expected_Total_Fragments = Total_Fragments;
	Fragmented_JPGE_Data_Map->Expected_Each_Fragment_Size = Max_Array_Size;
	Fragmented_JPGE_Data_Map->Sent_Fragment_Num = 0;
	Fragmented_JPGE_Data_Map->Server_Confrim_Received_Num = 0;

	Fragmented_JPGE_Data_Map->JPEG_Buffer.Init(0, Raw_Data.Num());

	for (int i = 0; i < (int)(Raw_Data.Num()); ++i)
	{
		Fragmented_JPGE_Data_Map->JPEG_Buffer[i] = Raw_Data[i];
	}

	return true;
}

bool UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_RGBA(const TArray<uint8>& Raw_Data, const int& Max_Array_Size, int& Total_Fragments, FRGB_Fragment_Map& Fragmented_RGB_Data_Map)
{
	if ((Raw_Data.Num() <= 0) || (Max_Array_Size == 0))
	{
		return false;
	}
	//Fragmented_Data.Unique_Index = Unique_Index;

	Total_Fragments = 1 + (((Raw_Data.Num() / 4) - 1) / Max_Array_Size);
	bool Has_Remainder = Total_Fragments != ((Raw_Data.Num() / 4) / Max_Array_Size);

	Fragmented_RGB_Data_Map.Expected_Total_Fragments = Total_Fragments;
	Fragmented_RGB_Data_Map.Expected_Each_Fragment_Size = Max_Array_Size;
	Fragmented_RGB_Data_Map.Fragments_Sent = 0;

	Fragmented_RGB_Data_Map.RED_Channel.Init(0, Raw_Data.Num() / 4);//Colour Black
	Fragmented_RGB_Data_Map.GREEN_Channel.Init(0, Raw_Data.Num() / 4);//Colour Black
	Fragmented_RGB_Data_Map.BLUE_Channel.Init(0, Raw_Data.Num() / 4);//Colour Black
	
	int RGBA_Index = 0;
	for (int i = 0; i < (int)(Raw_Data.Num()/4); ++i)
	{
		RGBA_Index = i * 4;
		Fragmented_RGB_Data_Map.RED_Channel[i] = Raw_Data[RGBA_Index + 0];
		Fragmented_RGB_Data_Map.GREEN_Channel[i] = Raw_Data[RGBA_Index + 1];
		Fragmented_RGB_Data_Map.BLUE_Channel[i] = Raw_Data[RGBA_Index + 2];
	}

	return true;
}

bool UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_RGB(const TArray<uint8>& Raw_Data, const int& Max_Array_Size, int& Total_Fragments, FRGB_Fragment_Map& Fragmented_RGB_Data_Map)
{
	if ((Raw_Data.Num() <= 0) || (Max_Array_Size == 0))
	{
		return false;
	}
	//Fragmented_Data.Unique_Index = Unique_Index;

	Total_Fragments = 1 + (((Raw_Data.Num()/3) - 1) / Max_Array_Size);
	bool Has_Remainder = Total_Fragments != ((Raw_Data.Num() / 3) / Max_Array_Size);

	Fragmented_RGB_Data_Map.Expected_Total_Fragments = Total_Fragments;
	Fragmented_RGB_Data_Map.Expected_Each_Fragment_Size = Max_Array_Size;
	Fragmented_RGB_Data_Map.Fragments_Sent = 0;

	Fragmented_RGB_Data_Map.RED_Channel.Init(0, Raw_Data.Num() / 3);//Colour Black
	Fragmented_RGB_Data_Map.GREEN_Channel.Init(0, Raw_Data.Num() / 3);//Colour Black
	Fragmented_RGB_Data_Map.BLUE_Channel.Init(0, Raw_Data.Num() / 3);//Colour Black

	int RGB_Index = 0;
	for (int i = 0; i < (int)(Raw_Data.Num() / 3); ++i)
	{
		RGB_Index = i * 3;
		Fragmented_RGB_Data_Map.RED_Channel[i] = Raw_Data[RGB_Index + 0];
		Fragmented_RGB_Data_Map.GREEN_Channel[i] = Raw_Data[RGB_Index + 1];
		Fragmented_RGB_Data_Map.BLUE_Channel[i] = Raw_Data[RGB_Index + 2];
	}

	return true;
}

bool UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_JPGE(const TArray<uint8>& Raw_Data, const int& Max_Array_Size, int& Total_Fragments, FJPEG_Fragment_Struct& Fragmented_JPGE_Data_Map)
{
	if ((Raw_Data.Num() <= 0) || (Max_Array_Size == 0))
	{
		return false;
	}
	//Fragmented_Data.Unique_Index = Unique_Index;

	Total_Fragments = 1 + ((Raw_Data.Num() - 1) / Max_Array_Size);
	bool Has_Remainder = Total_Fragments != (Raw_Data.Num() / Max_Array_Size);

	Fragmented_JPGE_Data_Map.Expected_Total_Fragments = Total_Fragments;
	Fragmented_JPGE_Data_Map.Expected_Each_Fragment_Size = Max_Array_Size;
	Fragmented_JPGE_Data_Map.Sent_Fragment_Num = 0;
	Fragmented_JPGE_Data_Map.Server_Confrim_Received_Num = 0;

	Fragmented_JPGE_Data_Map.JPEG_Buffer.Init(0, Raw_Data.Num());

	for (int i = 0; i < (int)(Raw_Data.Num()); ++i)
	{
		Fragmented_JPGE_Data_Map.JPEG_Buffer[i] = Raw_Data[i];
	}

	return true;
}

TArray<uint8> UWrexa_Blueprint_Functions::RGBA_Bilinear_Resize(int Original_Width, int Original_Height, TArray<uint8> RGBA_Image_Buffer_Fragment, int Resize_Width, int Resize_Height, const bool& Ignore_Alpha)
{
	//NOT ACCOUNTED FOR 0,0 Widht and Height

//FMath::BiLerp(1, 2, 3, 4, 0.5f, 0.5f);
//FMath::Clamp(15, 0, 20);
	
	int Total_Size = Resize_Width * Resize_Height * 4;
	int Original_Total_Size = Original_Width * Original_Height * 4;

	TArray<uint8> Resized_RGBA_Buffer;// = new int[Resize_Width * Resize_Height];
	Resized_RGBA_Buffer.Init(0, Total_Size);// Initialize Array and Set Black As Default Colour

	float Current_Height_Ratio = 0.0f;
	float Current_Width_Ratio = 0.0f;

	TArray<uint8> Calc_R;
	TArray<uint8> Calc_G;
	TArray<uint8> Calc_B;
	TArray<uint8> Calc_A;

	Calc_R.Init(0, 9);// Eight Directions(4+4) + Center
	Calc_G.Init(0, 9);// Starts From Top Left Corner And Ends At Bottom Right Corner [((Calc / 3) - 1),((Calc % 3) - 1)] Formula
	Calc_B.Init(0, 9);// Ends at 9th Element, 4th Element is the Center
	Calc_A.Init(0, 9);

	int Index = 0;
	int Inner_Index = 0;
	int Calc_Index = 0;

	float Divisor_Width = 0;
	float Divisor_Height = 0;
	int Int_Divisor_Width = 0;

	Divisor_Width = Resize_Width - 1;
	Divisor_Width = (Divisor_Width > 0) ? Divisor_Width : 1;

	Divisor_Height = Resize_Height - 1;
	Divisor_Height = (Divisor_Height > 0) ? Divisor_Height : 1;

	Int_Divisor_Width = Resize_Width - 1;
	Int_Divisor_Width = (Divisor_Width > 0) ? Divisor_Width : 1;

	int Cur_Width = 0;
	int Cur_Height = 0;

	for (int i = 0; i < Resize_Height * Resize_Width; i++)
	{
		Index = i * 4;

		Cur_Height += Cur_Width / Int_Divisor_Width;
		Cur_Width = i % Resize_Width;

		Current_Width_Ratio = std::clamp((float)(((float)(Cur_Width)) / Divisor_Width), 0.0f, 1.0f);
		//Current_Height_Ratio = std::clamp((float)((float)(i % (int)Resize_Height) / (float)Resize_Height), 0.0f, 1.0f);
		Current_Height_Ratio = std::clamp((float)(((float)(Cur_Height)) / Divisor_Height), 0.0f, 1.0f);

		//for (int8 Calc = -4; Calc <= 4; ++Calc)
		for (int8 Calc = 0; Calc < 9; ++Calc)
		{
			// 0 = -1 , -1   // 0    // Starts From Top Left Corner
			// 1 = -1 ,  0   // 0.33
			// 2 = -1 ,  1   // 0.66
			// 3 =  0 , -1   // 1
			// 4 =  0 ,  0   // 1.33 // Middle
			// 5 =  0 ,  1   // 1.66
			// 6 =  1 , -1   // 2
			// 7 =  1 ,  0   // 2.33
			// 8 =  1 ,  1   // 2.66 // Ends At Bottom Right Corner

			//(int)((Cur_Width * 4) + (Cur_Height * Original_Width * 4)),
			//(int)((((Cur_Width + (Calc / (int8)3)) * 4)) + ((Cur_Height + (Calc % (int8)3)) * Original_Width * 4)),
			//(int)((((((int)(Current_Width_Ratio * (float)Original_Width)) + (Calc / (int8)3)) * 4)) + ((((int)(Current_Height_Ratio * (float)Original_Height)) + (Calc % (int8)3)) * Original_Width * 4)),
			Inner_Index = std::clamp
			(				
				(int)((((((int)(Current_Width_Ratio * (float)Original_Width)) + ((Calc / (int8)3) - 1)) * 4)) + ((((int)(Current_Height_Ratio * (float)Original_Height)) + (Calc % ((int8)3) - 1)) * Original_Width * 4)),
				(int)0,
				(int)(Original_Total_Size - (int)4)
			);

			Calc_R[Calc] = RGBA_Image_Buffer_Fragment[Inner_Index + 0];
			Calc_G[Calc] = RGBA_Image_Buffer_Fragment[Inner_Index + 1];
			Calc_B[Calc] = RGBA_Image_Buffer_Fragment[Inner_Index + 2];
			Calc_A[Calc] = RGBA_Image_Buffer_Fragment[Inner_Index + 3];
		}

		Resized_RGBA_Buffer[Index + 0] = Get_Array_Average(Calc_R);
		Resized_RGBA_Buffer[Index + 1] = Get_Array_Average(Calc_G);
		Resized_RGBA_Buffer[Index + 2] = Get_Array_Average(Calc_B);
		Resized_RGBA_Buffer[Index + 3] = (((uint8)(!Ignore_Alpha)) * Get_Array_Average(Calc_A));
	}

	return Resized_RGBA_Buffer;
}

void UWrexa_Blueprint_Functions::RGBA_Bilinear_Resize_Reference(int Original_Width, int Original_Height, uint8* RGBA_Image_Buffer_Fragment, TArray<uint8>& Resized_RGBA_Buffer, int Resize_Width, int Resize_Height, const bool& Ignore_Alpha)
{
	//NOT ACCOUNTED FOR 0,0 Widht and Height

//FMath::BiLerp(1, 2, 3, 4, 0.5f, 0.5f);
//FMath::Clamp(15, 0, 20);

	int Total_Size = Resize_Width * Resize_Height * 4;
	int Original_Total_Size = Original_Width * Original_Height * 4;

	Resized_RGBA_Buffer.Init(0, Total_Size);// Initialize Array and Set Black As Default Colour

	float Current_Height_Ratio = 0.0f;
	float Current_Width_Ratio = 0.0f;

	TArray<uint8> Calc_R;
	TArray<uint8> Calc_G;
	TArray<uint8> Calc_B;
	TArray<uint8> Calc_A;

	Calc_R.Init(0, 9);// Eight Directions(4+4) + Center
	Calc_G.Init(0, 9);// Starts From Top Left Corner And Ends At Bottom Right Corner [((Calc / 3) - 1),((Calc % 3) - 1)] Formula
	Calc_B.Init(0, 9);// Ends at 9th Element, 4th Element is the Center
	Calc_A.Init(0, 9);

	int Index = 0;
	int Inner_Index = 0;
	int Calc_Index = 0;

	float Divisor_Width = 0;
	float Divisor_Height = 0;
	int Int_Divisor_Width = 0;

	Divisor_Width = Resize_Width - 1;
	Divisor_Width = (Divisor_Width > 0) ? Divisor_Width : 1;

	Divisor_Height = Resize_Height - 1;
	Divisor_Height = (Divisor_Height > 0) ? Divisor_Height : 1;

	Int_Divisor_Width = Resize_Width - 1;
	Int_Divisor_Width = (Divisor_Width > 0) ? Divisor_Width : 1;

	int Cur_Width = 0;
	int Cur_Height = 0;

	for (int i = 0; i < Resize_Height * Resize_Width; i++)
	{
		Index = i * 4;

		Cur_Height += Cur_Width / Int_Divisor_Width;
		Cur_Width = i % Resize_Width;

		Current_Width_Ratio = std::clamp((float)(((float)(Cur_Width)) / Divisor_Width), 0.0f, 1.0f);
		//Current_Height_Ratio = std::clamp((float)((float)(i % (int)Resize_Height) / (float)Resize_Height), 0.0f, 1.0f);
		Current_Height_Ratio = std::clamp((float)(((float)(Cur_Height)) / Divisor_Height), 0.0f, 1.0f);

		{
			// 0 = -1 , -1   // 0    // Starts From Top Left Corner
			// 1 = -1 ,  0   // 0.33
			// 2 = -1 ,  1   // 0.66
			// 3 =  0 , -1   // 1
			// 4 =  0 ,  0   // 1.33 // Middle
			// 5 =  0 ,  1   // 1.66
			// 6 =  1 , -1   // 2
			// 7 =  1 ,  0   // 2.33
			// 8 =  1 ,  1   // 2.66 // Ends At Bottom Right Corner

			Inner_Index = std::clamp
			(
				(int)((((((int)(Current_Width_Ratio * (float)Original_Width)) - 1) * 4)) + ((((int)(Current_Height_Ratio * (float)Original_Height)) - 1) * Original_Width * 4)),
				(int)0,
				(int)(Original_Total_Size - (int)4)
			);

			Calc_R[0] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 0);
			Calc_G[0] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 1);
			Calc_B[0] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 2);
			Calc_A[0] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 3);

			Inner_Index = std::clamp
			(
				(int)((((((int)(Current_Width_Ratio * (float)Original_Width)) - 1) * 4)) + ((((int)(Current_Height_Ratio * (float)Original_Height))) * Original_Width * 4)),
				(int)0,
				(int)(Original_Total_Size - (int)4)
			);

			Calc_R[1] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 0);
			Calc_G[1] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 1);
			Calc_B[1] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 2);
			Calc_A[1] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 3);

			Inner_Index = std::clamp
			(
				(int)((((((int)(Current_Width_Ratio * (float)Original_Width)) - 1) * 4)) + ((((int)(Current_Height_Ratio * (float)Original_Height)) + 1) * Original_Width * 4)),
				(int)0,
				(int)(Original_Total_Size - (int)4)
			);

			Calc_R[2] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 0);
			Calc_G[2] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 1);
			Calc_B[2] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 2);
			Calc_A[2] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 3);

			Inner_Index = std::clamp
			(
				(int)((((((int)(Current_Width_Ratio * (float)Original_Width))) * 4)) + ((((int)(Current_Height_Ratio * (float)Original_Height)) - 1) * Original_Width * 4)),
				(int)0,
				(int)(Original_Total_Size - (int)4)
			);

			Calc_R[3] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 0);
			Calc_G[3] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 1);
			Calc_B[3] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 2);
			Calc_A[3] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 3);

			Inner_Index = std::clamp
			(
				(int)((((((int)(Current_Width_Ratio * (float)Original_Width))) * 4)) + ((((int)(Current_Height_Ratio * (float)Original_Height))) * Original_Width * 4)),
				(int)0,
				(int)(Original_Total_Size - (int)4)
			);

			Calc_R[4] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 0);
			Calc_G[4] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 1);
			Calc_B[4] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 2);
			Calc_A[4] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 3);

			Inner_Index = std::clamp
			(
				(int)((((((int)(Current_Width_Ratio * (float)Original_Width))) * 4)) + ((((int)(Current_Height_Ratio * (float)Original_Height)) + 1) * Original_Width * 4)),
				(int)0,
				(int)(Original_Total_Size - (int)4)
			);

			Calc_R[5] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 0);
			Calc_G[5] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 1);
			Calc_B[5] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 2);
			Calc_A[5] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 3);

			Inner_Index = std::clamp
			(
				(int)((((((int)(Current_Width_Ratio * (float)Original_Width)) + 1) * 4)) + ((((int)(Current_Height_Ratio * (float)Original_Height)) - 1) * Original_Width * 4)),
				(int)0,
				(int)(Original_Total_Size - (int)4)
			);

			Calc_R[6] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 0);
			Calc_G[6] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 1);
			Calc_B[6] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 2);
			Calc_A[6] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 3);

			Inner_Index = std::clamp
			(
				(int)((((((int)(Current_Width_Ratio * (float)Original_Width)) + 1) * 4)) + ((((int)(Current_Height_Ratio * (float)Original_Height))) * Original_Width * 4)),
				(int)0,
				(int)(Original_Total_Size - (int)4)
			);

			Calc_R[7] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 0);
			Calc_G[7] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 1);
			Calc_B[7] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 2);
			Calc_A[7] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 3);

			Inner_Index = std::clamp
			(
				(int)((((((int)(Current_Width_Ratio * (float)Original_Width)) + 1) * 4)) + ((((int)(Current_Height_Ratio * (float)Original_Height)) + 1) * Original_Width * 4)),
				(int)0,
				(int)(Original_Total_Size - (int)4)
			);

			Calc_R[8] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 0);
			Calc_G[8] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 1);
			Calc_B[8] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 2);
			Calc_A[8] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 3);
		}

		Resized_RGBA_Buffer[Index + 0] = Get_Array_Average(Calc_R);
		Resized_RGBA_Buffer[Index + 1] = Get_Array_Average(Calc_G);
		Resized_RGBA_Buffer[Index + 2] = Get_Array_Average(Calc_B);
		Resized_RGBA_Buffer[Index + 3] = (((uint8)(!Ignore_Alpha)) * Get_Array_Average(Calc_A));
	}
}

TArray<uint8> UWrexa_Blueprint_Functions::RGB_Bilinear_Resize(int Original_Width, int Original_Height, TArray<uint8> RGB_Image_Buffer_Fragment, int Resize_Width, int Resize_Height)
{
	//NOT ACCOUNTED FOR 0,0 Widht and Height

//FMath::BiLerp(1, 2, 3, 4, 0.5f, 0.5f);
//FMath::Clamp(15, 0, 20);

	int Total_Size = Resize_Width * Resize_Height * 3;
	int Original_Total_Size = Original_Width * Original_Height * 3;

	TArray<uint8> Resized_RGB_Buffer;// = new int[Resize_Width * Resize_Height];
	Resized_RGB_Buffer.Init(0, Total_Size);// Initialize Array and Set Black As Default Colour

	float Current_Height_Ratio = 0.0f;
	float Current_Width_Ratio = 0.0f;

	TArray<uint8> Calc_R;
	TArray<uint8> Calc_G;
	TArray<uint8> Calc_B;

	Calc_R.Init(0, 9);// Eight Directions(4+4) + Center
	Calc_G.Init(0, 9);// Starts From Top Left Corner And Ends At Bottom Right Corner [((Calc / 3) - 1),((Calc % 3) - 1)] Formula
	Calc_B.Init(0, 9);// Ends at 9th Element, 4th Element is the Center

	int Index = 0;
	int Inner_Index = 0;
	int Calc_Index = 0;

	float Divisor_Width = 0;
	float Divisor_Height = 0;
	int Int_Divisor_Width = 0;

	Divisor_Width = Resize_Width - 1;
	Divisor_Width = (Divisor_Width > 0) ? Divisor_Width : 1;

	Divisor_Height = Resize_Height - 1;
	Divisor_Height = (Divisor_Height > 0) ? Divisor_Height : 1;

	Int_Divisor_Width = Resize_Width - 1;
	Int_Divisor_Width = (Divisor_Width > 0) ? Divisor_Width : 1;

	int Cur_Width = 0;
	int Cur_Height = 0;

	for (int i = 0; i < Resize_Height * Resize_Width; i++)
	{
		Index = i * 3;

		Cur_Height += Cur_Width / Int_Divisor_Width;
		Cur_Width = i % Resize_Width;

		Current_Width_Ratio = std::clamp((float)(((float)(Cur_Width)) / Divisor_Width), 0.0f, 1.0f);
		//Current_Height_Ratio = std::clamp((float)((float)(i % (int)Resize_Height) / (float)Resize_Height), 0.0f, 1.0f);
		Current_Height_Ratio = std::clamp((float)(((float)(Cur_Height)) / Divisor_Height), 0.0f, 1.0f);

		//for (int8 Calc = -4; Calc <= 4; ++Calc)
		for (int8 Calc = 0; Calc < 9; ++Calc)
		{
			// 0 = -1 , -1   // 0    // Starts From Top Left Corner
			// 1 = -1 ,  0   // 0.33
			// 2 = -1 ,  1   // 0.66
			// 3 =  0 , -1   // 1
			// 4 =  0 ,  0   // 1.33 // Middle
			// 5 =  0 ,  1   // 1.66
			// 6 =  1 , -1   // 2
			// 7 =  1 ,  0   // 2.33
			// 8 =  1 ,  1   // 2.66 // Ends At Bottom Right Corner

			//(int)((Cur_Width * 3) + (Cur_Height * Original_Width * 3)),
			//(int)((((Cur_Width + (Calc / (int8)3)) * 3)) + ((Cur_Height + (Calc % (int8)3)) * Original_Width * 3)),
			//(int)((((((int)(Current_Width_Ratio * (float)Original_Width)) + (Calc / (int8)3)) * 3)) + ((((int)(Current_Height_Ratio * (float)Original_Height)) + (Calc % (int8)3)) * Original_Width * 3)),
			Inner_Index = std::clamp
			(
				(int)((((((int)(Current_Width_Ratio * (float)Original_Width)) + (Calc / (int8)3)) * 3)) + ((((int)(Current_Height_Ratio * (float)Original_Height)) + (Calc % (int8)3)) * Original_Width * 3)),
				(int)0,
				(int)(Original_Total_Size - (int)3)
			);

			Calc_R[Calc] = RGB_Image_Buffer_Fragment[Inner_Index + 0];
			Calc_G[Calc] = RGB_Image_Buffer_Fragment[Inner_Index + 1];
			Calc_B[Calc] = RGB_Image_Buffer_Fragment[Inner_Index + 2];
		}

		Resized_RGB_Buffer[Index + 0] = Get_Array_Average(Calc_R);
		Resized_RGB_Buffer[Index + 1] = Get_Array_Average(Calc_G);
		Resized_RGB_Buffer[Index + 2] = Get_Array_Average(Calc_B);
	}

	return Resized_RGB_Buffer;
}

TArray<uint8> UWrexa_Blueprint_Functions::RGBA_Nearest_Neighbour_Resize(int Original_Width, int Original_Height, TArray<uint8> RGBA_Image_Buffer_Fragment, int Resize_Width, int Resize_Height, const bool& Ignore_Alpha)
{
	//NOT ACCOUNTED FOR 0,0 Widht and Height

//FMath::BiLerp(1, 2, 3, 4, 0.5f, 0.5f);
//FMath::Clamp(15, 0, 20);

	int Total_Size = Resize_Width * Resize_Height * 4;
	int Original_Total_Size = Original_Width * Original_Height * 4;

	TArray<uint8> Resized_RGBA_Buffer;// = new int[Resize_Width * Resize_Height];
	Resized_RGBA_Buffer.Init(0, Total_Size);// Initialize Array and Set Black As Default Colour

	float Current_Height_Ratio = 0.0f;
	float Current_Width_Ratio = 0.0f;

	int Index = 0;
	int Inner_Index = 0;
	int Calc_Index = 0;

	float Divisor_Width = 0;
	float Divisor_Height = 0;
	int Int_Divisor_Width = 0;

	Divisor_Width = Resize_Width - 1;
	Divisor_Width = (Divisor_Width > 0) ? Divisor_Width : 1;

	Divisor_Height = Resize_Height - 1;
	Divisor_Height = (Divisor_Height > 0) ? Divisor_Height : 1;

	Int_Divisor_Width = Resize_Width - 1;
	Int_Divisor_Width = (Divisor_Width > 0) ? Divisor_Width : 1;

	int Cur_Width = 0;
	int Cur_Height = 0;

	for (int i = 0; i < Resize_Height * Resize_Width; i++)
	{
		Index = i * 4;

		Cur_Height += Cur_Width / Int_Divisor_Width;
		Cur_Width = i % Resize_Width;

		Current_Width_Ratio = std::clamp((float)(((float)(Cur_Width)) / Divisor_Width), 0.0f, 1.0f);
		//Current_Height_Ratio = std::clamp((float)((float)(i % (int)Resize_Height) / (float)Resize_Height), 0.0f, 1.0f);
		Current_Height_Ratio = std::clamp((float)(((float)(Cur_Height)) / Divisor_Height), 0.0f, 1.0f);
		
		Inner_Index = std::clamp
		(
			(int)((((((int)(Current_Width_Ratio * (float)Original_Width))) * 4)) + ((((int)(Current_Height_Ratio * (float)Original_Height))) * Original_Width * 4)),
			(int)0,
			(int)(Original_Total_Size - (int)4)
		);

		Resized_RGBA_Buffer[Index + 0] = RGBA_Image_Buffer_Fragment[Inner_Index + 0];
		Resized_RGBA_Buffer[Index + 1] = RGBA_Image_Buffer_Fragment[Inner_Index + 1];
		Resized_RGBA_Buffer[Index + 2] = RGBA_Image_Buffer_Fragment[Inner_Index + 2];
		Resized_RGBA_Buffer[Index + 3] = (((uint8)(!Ignore_Alpha)) * RGBA_Image_Buffer_Fragment[Inner_Index + 3]);		
	}

	return Resized_RGBA_Buffer;
}

void UWrexa_Blueprint_Functions::RGBA_Nearest_Neighbour_Resize_Reference(int Original_Width, int Original_Height, uint8* RGBA_Image_Buffer_Fragment, TArray<uint8>& Resized_RGBA_Buffer, int Resize_Width, int Resize_Height, const bool& Ignore_Alpha)
{
	//NOT ACCOUNTED FOR 0,0 Widht and Height

//FMath::BiLerp(1, 2, 3, 4, 0.5f, 0.5f);
//FMath::Clamp(15, 0, 20);

	int Total_Size = Resize_Width * Resize_Height * 4;
	int Original_Total_Size = Original_Width * Original_Height * 4;

	Resized_RGBA_Buffer.Init(0, Total_Size);// Initialize Array and Set Black As Default Colour

	float Current_Height_Ratio = 0.0f;
	float Current_Width_Ratio = 0.0f;

	int Index = 0;
	int Inner_Index = 0;
	int Calc_Index = 0;

	float Divisor_Width = 0;
	float Divisor_Height = 0;
	int Int_Divisor_Width = 0;

	Divisor_Width = Resize_Width - 1;
	Divisor_Width = (Divisor_Width > 0) ? Divisor_Width : 1;

	Divisor_Height = Resize_Height - 1;
	Divisor_Height = (Divisor_Height > 0) ? Divisor_Height : 1;

	Int_Divisor_Width = Resize_Width - 1;
	Int_Divisor_Width = (Divisor_Width > 0) ? Divisor_Width : 1;

	int Cur_Width = 0;
	int Cur_Height = 0;

	for (int i = 0; i < Resize_Height * Resize_Width; i++)
	{
		Index = i * 4;

		Cur_Height += Cur_Width / Int_Divisor_Width;
		Cur_Width = i % Resize_Width;

		Current_Width_Ratio = std::clamp((float)(((float)(Cur_Width)) / Divisor_Width), 0.0f, 1.0f);
		//Current_Height_Ratio = std::clamp((float)((float)(i % (int)Resize_Height) / (float)Resize_Height), 0.0f, 1.0f);
		Current_Height_Ratio = std::clamp((float)(((float)(Cur_Height)) / Divisor_Height), 0.0f, 1.0f);

		Inner_Index = std::clamp
		(
			(int)((((((int)(Current_Width_Ratio * (float)Original_Width))) * 4)) + ((((int)(Current_Height_Ratio * (float)Original_Height))) * Original_Width * 4)),
			(int)0,
			(int)(Original_Total_Size - (int)4)
		);

		Resized_RGBA_Buffer[Index + 0] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 0);
		Resized_RGBA_Buffer[Index + 1] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 1);
		Resized_RGBA_Buffer[Index + 2] = *(RGBA_Image_Buffer_Fragment + Inner_Index + 2);
		Resized_RGBA_Buffer[Index + 3] = (((uint8)(!Ignore_Alpha)) * (*(RGBA_Image_Buffer_Fragment + Inner_Index + 3)));
	}
}

TArray<uint8> UWrexa_Blueprint_Functions::RGB_Nearest_Neighbour_Resize(int Original_Width, int Original_Height, TArray<uint8> RGB_Image_Buffer_Fragment, int Resize_Width, int Resize_Height)
{
	//NOT ACCOUNTED FOR 0,0 Widht and Height

//FMath::BiLerp(1, 2, 3, 4, 0.5f, 0.5f);
//FMath::Clamp(15, 0, 20);

	int Total_Size = Resize_Width * Resize_Height * 3;
	int Original_Total_Size = Original_Width * Original_Height * 3;

	TArray<uint8> Resized_RGB_Buffer;// = new int[Resize_Width * Resize_Height];
	Resized_RGB_Buffer.Init(0, Total_Size);// Initialize Array and Set Black As Default Colour

	float Current_Height_Ratio = 0.0f;
	float Current_Width_Ratio = 0.0f;

	int Index = 0;
	int Inner_Index = 0;
	int Calc_Index = 0;

	float Divisor_Width = 0;
	float Divisor_Height = 0;
	int Int_Divisor_Width = 0;

	Divisor_Width = Resize_Width - 1;
	Divisor_Width = (Divisor_Width > 0) ? Divisor_Width : 1;

	Divisor_Height = Resize_Height - 1;
	Divisor_Height = (Divisor_Height > 0) ? Divisor_Height : 1;

	Int_Divisor_Width = Resize_Width - 1;
	Int_Divisor_Width = (Divisor_Width > 0) ? Divisor_Width : 1;

	int Cur_Width = 0;
	int Cur_Height = 0;

	for (int i = 0; i < Resize_Height * Resize_Width; i++)
	{
		Index = i * 3;

		Cur_Height += Cur_Width / Int_Divisor_Width;
		Cur_Width = i % Resize_Width;

		Current_Width_Ratio = std::clamp((float)(((float)(Cur_Width)) / Divisor_Width), 0.0f, 1.0f);
		//Current_Height_Ratio = std::clamp((float)((float)(i % (int)Resize_Height) / (float)Resize_Height), 0.0f, 1.0f);
		Current_Height_Ratio = std::clamp((float)(((float)(Cur_Height)) / Divisor_Height), 0.0f, 1.0f);

		Inner_Index = std::clamp
		(
			(int)((((((int)(Current_Width_Ratio * (float)Original_Width))) * 3)) + ((((int)(Current_Height_Ratio * (float)Original_Height))) * Original_Width * 3)),
			(int)0,
			(int)(Original_Total_Size - (int)4)
		);

		Resized_RGB_Buffer[Index + 0] = RGB_Image_Buffer_Fragment[Inner_Index + 0];
		Resized_RGB_Buffer[Index + 1] = RGB_Image_Buffer_Fragment[Inner_Index + 1];
		Resized_RGB_Buffer[Index + 2] = RGB_Image_Buffer_Fragment[Inner_Index + 2];
	}

	return Resized_RGB_Buffer;
}

TArray<uint8> UWrexa_Blueprint_Functions::RGBA_To_RGB_Buffer(int Width, int Height, TArray<uint8> RGBA_Image_Buffer_Fragment)
{
	TArray<uint8> Output;
	Output.Init(0, (Width * Height * 3));

	int RGB_Index = 0;
	int RGBA_Index = 0;
	for (int i = 0; i < (int)(Width * Height); ++i)
	{
		RGB_Index = i * 3;
		RGBA_Index = i * 4;
		Output[RGB_Index + 0] = RGBA_Image_Buffer_Fragment[RGBA_Index + 0];
		Output[RGB_Index + 1] = RGBA_Image_Buffer_Fragment[RGBA_Index + 1];
		Output[RGB_Index + 2] = RGBA_Image_Buffer_Fragment[RGBA_Index + 2];
	}

	return Output;
}

TArray<uint8> UWrexa_Blueprint_Functions::RGB_To_RGBA_Buffer(int Width, int Height, TArray<uint8> RGB_Image_Buffer_Fragment)
{
	TArray<uint8> Output;
	Output.Init(0, (Width * Height * 4));

	int RGB_Index = 0;
	int RGBA_Index = 0;
	for (int i = 0; i < (int)(Width * Height); ++i)
	{
		RGB_Index = i * 3;
		RGBA_Index = i * 4;
		Output[RGBA_Index + 0] = RGB_Image_Buffer_Fragment[RGB_Index + 0];
		Output[RGBA_Index + 1] = RGB_Image_Buffer_Fragment[RGB_Index + 1];
		Output[RGBA_Index + 2] = RGB_Image_Buffer_Fragment[RGB_Index + 2];
		Output[RGBA_Index + 3] = 0;
	}

	return Output;
}

TArray<uint8> UWrexa_Blueprint_Functions::RGBA_To_JPGE_Buffer(const int& Width, const int& Height, TArray<uint8> RGBA_Image_Buffer, const uint8& Quality_Factor_Percent, const bool& Swap_Colour_Channel, bool& IsSuccessful)
{
	// Create an instance of the image wrapper module
	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");

	// Create an instance of the image wrapper for the JPEG format
	TSharedPtr<IImageWrapper> imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	// Set the dimensions and format of the image
	imageWrapper->SetRaw(RGBA_Image_Buffer.GetData(), RGBA_Image_Buffer.Num(), Width, Height, (Swap_Colour_Channel? ERGBFormat::BGRA : ERGBFormat::RGBA), 8);

	// Encode the image data as a JPEG image
	TArray64<uint8> jpegData = imageWrapper->GetCompressed(Quality_Factor_Percent);

	TArray<uint8> Return_Array(jpegData);

	IsSuccessful = true;
	return Return_Array;
}

TArray<uint8> UWrexa_Blueprint_Functions::RGB_To_JPGE_Buffer(const int& Width, const int& Height, TArray<uint8> RGB_Image_Buffer, const uint8& Quality_Factor_Percent, const bool& Swap_Colour_Channel, bool& IsSuccessful)
{
	// Create an instance of the image wrapper module
	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");

	// Create an instance of the image wrapper for the JPEG format
	TSharedPtr<IImageWrapper> imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	// Convert RGB Data To RGBA
	RGB_Image_Buffer = RGB_To_RGBA_Buffer(Width, Height, RGB_Image_Buffer);

	// Set the dimensions and format of the image
	imageWrapper->SetRaw(RGB_Image_Buffer.GetData(), RGB_Image_Buffer.Num(), Width, Height, (Swap_Colour_Channel ? ERGBFormat::BGRA : ERGBFormat::RGBA), 8);

	// Encode the image data as a JPEG image
	TArray64<uint8> jpegData = imageWrapper->GetCompressed(Quality_Factor_Percent);

	TArray<uint8> Return_Array(jpegData);

	IsSuccessful = true;
	return Return_Array;
}

void UWrexa_Blueprint_Functions::RGBA_To_JPGE_Buffer_Reference(const int& Width, const int& Height, uint8* RGBA_Image_Buffer, TArray<uint8>& Return_Array, const uint8& Quality_Factor_Percent, const bool& Swap_Colour_Channel, bool& IsSuccessful)
{
	// Create an instance of the image wrapper module
	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");

	// Create an instance of the image wrapper for the JPEG format
	TSharedPtr<IImageWrapper> imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	// Set the dimensions and format of the image
	imageWrapper->SetRaw(RGBA_Image_Buffer, Width * Height * 4, Width, Height, (Swap_Colour_Channel ? ERGBFormat::BGRA : ERGBFormat::RGBA), 8);

	// Encode the image data as a JPEG image
	Return_Array = imageWrapper->GetCompressed(Quality_Factor_Percent);

	IsSuccessful = true;
}

void UWrexa_Blueprint_Functions::RGBA_To_JPGE_Buffer_With_Resize_Reference(const int& Width, const int& Height, int Resize_Width, int Resize_Height, uint8* RGBA_Image_Buffer, TArray<uint8>& Return_Array, const uint8& Quality_Factor_Percent, const bool& Swap_Colour_Channel, bool& IsSuccessful)
{
	// Create an instance of the image wrapper module
	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");

	// Create an instance of the image wrapper for the JPEG format
	TSharedPtr<IImageWrapper> imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);


	// Set the dimensions and format of the image
	imageWrapper->SetRaw(RGBA_Image_Buffer, Width * Height * 4, Width, Height, (Swap_Colour_Channel ? ERGBFormat::BGRA : ERGBFormat::RGBA), 8);

	// Encode the image data as a JPEG image
	Return_Array = imageWrapper->GetCompressed(Quality_Factor_Percent);

	IsSuccessful = true;
}

TArray<uint8> UWrexa_Blueprint_Functions::JPGE_To_RGBA_Buffer(TArray<uint8> JPGE_Buffer, int& Output_Width, int& Output_Height, int& Total_Colour_Channels, bool& IsSuccessful)
{
	TArray<uint8> Return_Array;

	IsSuccessful = false;

	// Use the ImageWrapper module to decode the JPEG data
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(JPGE_Buffer.GetData(), JPGE_Buffer.Num()))
	{
		return Return_Array;
	}

	// Get the image data as a byte array in RGBA format
	TArray<uint8> ImageData;
	if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, ImageData))
	{
		return Return_Array;
	}

	// Copy the image data to the output parameters
	Return_Array = ImageData;
	Output_Width = ImageWrapper->GetWidth();
	Output_Height = ImageWrapper->GetHeight();

	IsSuccessful = true;
	return Return_Array;
}

// Extra
TArray<APlayerController*> UWrexa_Blueprint_Functions::Get_All_Local_Player_Controllers(UGameInstance *GameInstance)
{
	//TArray<ULocalPlayer*> Return_Local_Players_Array;

	TArray< APlayerController*> Return_Local_Player_Controller_Array;

	//FLocalPlayerIterator
	APlayerController* Return_PlayerController = nullptr;
	for (FLocalPlayerIterator It(GameInstance->GetEngine(), GameInstance->GetWorld()); It; ++It)
	{
		//Return_Local_Players_Array.Add(*It);
		
		Return_PlayerController = (*It)->GetPlayerController(GameInstance->GetWorld());
		if (Return_PlayerController != nullptr)
		{
			Return_Local_Player_Controller_Array.Add(Return_PlayerController);
		}
	}

	//return Return_Local_Players_Array;
	return Return_Local_Player_Controller_Array;
}

TArray<int> UWrexa_Blueprint_Functions::Reorder_Array(const TArray<int>& Array_To_Order, bool IsAscending)
{
	TArray<int> MyArray = Array_To_Order;
	//Array_To_Order.Sort();
	if (IsAscending)
	{
		MyArray.Sort([&](const int& L, const int& R)
			{
				return L > R;
			});
	}
	else
	{
		MyArray.Sort([&](const int& R, const int& L)
		{
			return L > R;
		});
	}
	
	return MyArray;
}
int UWrexa_Blueprint_Functions::Get_Size(const TArray<int>& Array_To_Order)
{
	return Array_To_Order.Num();
}

uint8 UWrexa_Blueprint_Functions::Get_Array_Average(const TArray<uint8>& Array_To_Get_Average)
{
	float Calc = 0;
	for (int i = 0; i < Array_To_Get_Average.Num(); ++i)
	{
		Calc += (float)(Array_To_Get_Average[i]);
	}

	return ((uint8)((float)Calc / ((float)(Array_To_Get_Average.Num()))));
}

void UWrexa_Blueprint_Functions::Shutdown()
{
	UE_LOG(LogTemp, Warning, TEXT("Requesting to Shut Down Game, As the Shutdown Function Is Called"));
	FPlatformMisc::RequestExit(false);
}

//Widget
/**
* Renders a UMG Widget to a texture with the specified size.
*
* @param Widget		The widget to be rendered.
* @param DrawSize	The size to render the Widget to. Also will be the texture size.
* @return			The texture containing the rendered widget.
*/
//UTexture2D* UWrexa_Blueprint_Functions::TextureFromWidget(UUserWidget* const Widget, const FVector2D& DrawSize)
//{
//	if (FSlateApplication::IsInitialized()
//		&& Widget != NULL && Widget->IsValidLowLevel()
//		&& DrawSize.X >= 1 && DrawSize.Y >= 1)
//	{
//		TSharedPtr<SWidget> SlateWidget(Widget->TakeWidget());
//		if (!SlateWidget.IsValid()) return NULL;
//		//WidgetRenderer = new FWidgetRenderer(true);
//		if (WidgetRenderer == nullptr)
//		{
//			WidgetRenderer = new FWidgetRenderer(true);
//		}
//		if (!WidgetRenderer) return nullptr;
//
//		UTextureRenderTarget2D* TextureRenderTarget = WidgetRenderer->DrawWidget(SlateWidget.ToSharedRef(), DrawSize);
//		// Creates Texture2D to store RenderTexture content
//		UTexture2D* Texture = UTexture2D::CreateTransient(DrawSize.X, DrawSize.Y, PF_B8G8R8A8);
//#if WITH_EDITORONLY_DATA
//		Texture->MipGenSettings = TMGS_NoMipmaps;
//#endif
//
//		// Lock and copies the data between the textures
//		TArray<FColor> SurfData;
//		FRenderTarget* RenderTarget = TextureRenderTarget->GameThread_GetRenderTargetResource();
//		RenderTarget->ReadPixels(SurfData);
//
//		void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
//		const int32 TextureDataSize = SurfData.Num() * 4;
//		FMemory::Memcpy(TextureData, SurfData.GetData(), TextureDataSize);
//		Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
//		Texture->UpdateResource();
//
//		// Free resources
//		SurfData.Empty();
//		TextureRenderTarget->ConditionalBeginDestroy();
//		SlateWidget.Reset();
//		//WidgetRenderer.Reset();
//
//		return Texture;
//	}
//	return NULL;
//}

//UTextureRenderTarget2D* UWrexa_Blueprint_Functions::WidgetToTexture(UUserWidget* const widget, const FVector2D& drawSize)
//{
//	// As long as the slate application is initialized and the widget passed in is not null continue...
//	if (FSlateApplication::IsInitialized() && widget != nullptr)
//	{
//		// Get the slate widget as a smart pointer. Return if null.
//		TSharedPtr<SWidget> SlateWidget(widget->TakeWidget());
//		if (!SlateWidget) return nullptr;
//		// Create a new widget renderer to render the widget to a texture render target 2D.
//		//FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(true);
//
//		if (WidgetRenderer == nullptr)
//		{
//			WidgetRenderer = new FWidgetRenderer(true);
//		}
//		if (!WidgetRenderer) return nullptr;
//		// Update/Create the render target 2D.
//		UTextureRenderTarget2D* TextureRenderTarget = WidgetRenderer->DrawWidget(SlateWidget.ToSharedRef(), drawSize);
//		// Return the updated render target 2D.
//		return TextureRenderTarget;
//	}
//	else return nullptr;
//}

// Image Formats
EImageFormat UWrexa_Blueprint_Functions::GetJoyImageFormat(EJoyImageFormats JoyFormat)
{
	switch (JoyFormat) {
	case EJoyImageFormats::JPG: return EImageFormat::JPEG;
	case EJoyImageFormats::PNG: return EImageFormat::PNG;
	case EJoyImageFormats::BMP: return EImageFormat::BMP;
	case EJoyImageFormats::ICO: return EImageFormat::ICO;
	case EJoyImageFormats::EXR: return EImageFormat::EXR;
	case EJoyImageFormats::ICNS: return EImageFormat::ICNS;
	}

	return EImageFormat::JPEG;
}

FString UWrexa_Blueprint_Functions::GetJoyImageExtension(EJoyImageFormats JoyFormat)
{
	switch (JoyFormat) {
	case EJoyImageFormats::JPG: return ".jpg";
	case EJoyImageFormats::PNG: return ".png";
	case EJoyImageFormats::BMP: return ".bmp";
	case EJoyImageFormats::ICO: return ".ico";
	case EJoyImageFormats::EXR: return ".exr";
	case EJoyImageFormats::ICNS: return ".icns";
	}
	return ".png";
}

UTexture2D* UWrexa_Blueprint_Functions::LoadTexture2DFromFile(const FString& File_Path_With_File_Name_And_Wihout_Extension, EJoyImageFormats ImageFormat, bool& IsValid, int32& Width, int32& Height, const bool& Swap_Colour_Channel)
{
	IsValid = false;
	UTexture2D* LoadedT2D = NULL;

	//bool isOK = UWrexa_WebFileeDownloader::CreateWebFileFolder();
	//if (!isOK) return LoadedT2D;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr< IImageWrapper > ImageWrapper = ImageWrapperModule.CreateImageWrapper(GetJoyImageFormat(ImageFormat));

	//Load From File
	//FString FullFilePath = UWrexa_WebFileDownloader::WebFileFolder() + "/" + FileName + GetJoyImageExtension(ImageFormat);
	FString FullFilePath = File_Path_With_File_Name_And_Wihout_Extension + GetJoyImageExtension(ImageFormat);
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FullFilePath)) return NULL;

	//Create T2D!
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		TArray<uint8> UncompressedBGRA;
		if (ImageWrapper.Get()->GetRaw(ERGBFormat::RGBA, 8, UncompressedBGRA))
		{
			LoadedT2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), Swap_Colour_Channel ? PF_B8G8R8A8 : EPixelFormat::PF_R8G8B8A8);

			//Valid?
			if (!LoadedT2D) return NULL;
			//~~~~~~~~~~~~~~

			//Out!
			Width = ImageWrapper->GetWidth();
			Height = ImageWrapper->GetHeight();

			//Copy!
			void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
			LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();

			//Update!
			LoadedT2D->UpdateResource();
		}
	}

	// Success!
	IsValid = true;
	return LoadedT2D;
}

UTexture2D* UWrexa_Blueprint_Functions::LoadTexture2DFromDownloaded_File(const UWrexa_WebFileDownloader* Downloaded_Context, EJoyImageFormats ImageFormat, bool& IsValid, int32& Width, int32& Height, const bool& Swap_Colour_Channel)
{
	IsValid = false;
	UTexture2D* LoadedT2D = NULL;

	if(!(Downloaded_Context->IsComplete))
	{
		return NULL;
	}

	//bool isOK = UWrexa_WebFileeDownloader::CreateWebFileFolder();
	//if (!isOK) return LoadedT2D;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr< IImageWrapper > ImageWrapper = ImageWrapperModule.CreateImageWrapper(GetJoyImageFormat(ImageFormat));

	//Load From File
	//FString FullFilePath = UWrexa_WebFileDownloader::WebFileFolder() + "/" + FileName + GetJoyImageExtension(ImageFormat);
	if (!(Downloaded_Context->IsComplete))
	{
		return NULL;
	}
	FString FullFilePath = Downloaded_Context->WebFileFolder() + "/" + Downloaded_Context->OutputFileName;
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FullFilePath))
	{
		return NULL;
	}

	//Create T2D!
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		TArray<uint8> UncompressedBGRA;
		if (ImageWrapper.Get()->GetRaw(ERGBFormat::RGBA, 8, UncompressedBGRA))
		{
			LoadedT2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), Swap_Colour_Channel ? PF_B8G8R8A8 : EPixelFormat::PF_R8G8B8A8);

			//Valid?
			if (!LoadedT2D) return NULL;
			//~~~~~~~~~~~~~~

			//Out!
			Width = ImageWrapper->GetWidth();
			Height = ImageWrapper->GetHeight();

			//Copy!
			void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
			LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();

			//Update!
			LoadedT2D->UpdateResource();
		}
	}

	// Success!
	IsValid = true;
	return LoadedT2D;
}

void UWrexa_Blueprint_Functions::LoadRawJPGBufferFromRGBA_Buffer(const TArray<uint8>& Raw_RGBA_Data, const int& Width, const int& Height, TArray<uint8>& Raw_JPEG_Buffer, bool& IsValid, int Resize_Width, int Resize_Height, EResize_Alogrithm_Type Resize_Algorithm_Type, const bool& Swap_Colour_Channel, const uint8& Quality_Factor_Percent)
{
	IsValid = false;

	if (Raw_RGBA_Data.Num() == (Width* Height*4))
	{
		//TArray<uint8> UncompressedBGRA;
		//if (ImageWrapper.Get()->GetRaw(ERGBFormat::RGBA, 8, UncompressedBGRA))
		//{
		if (Raw_RGBA_Data.Num() == 0)
		{
			return;
		}

		TArray<uint8> RGBA_Image_Buffer;

		switch (Resize_Algorithm_Type)
		{
		case EResize_Alogrithm_Type::NONE:
		{
			Raw_JPEG_Buffer = RGBA_To_JPGE_Buffer(Width, Height, Raw_RGBA_Data, Quality_Factor_Percent, Swap_Colour_Channel, IsValid);
			return;
			break;
		}
		case EResize_Alogrithm_Type::Nearest_Neighbour:
		{
			RGBA_Image_Buffer = UWrexa_Blueprint_Functions::RGBA_Nearest_Neighbour_Resize(Width, Height, Raw_RGBA_Data, Resize_Width, Resize_Height, false);
			Raw_JPEG_Buffer = RGBA_To_JPGE_Buffer(Resize_Width, Resize_Height, RGBA_Image_Buffer, Quality_Factor_Percent, Swap_Colour_Channel, IsValid);
			return;
			break;
		}
		case EResize_Alogrithm_Type::Bilinear_Resize:
		{
			RGBA_Image_Buffer = UWrexa_Blueprint_Functions::RGBA_Bilinear_Resize(Width, Height, Raw_RGBA_Data, Resize_Width, Resize_Height, false);
			Raw_JPEG_Buffer = RGBA_To_JPGE_Buffer(Resize_Width, Resize_Height, RGBA_Image_Buffer, Quality_Factor_Percent, Swap_Colour_Channel, IsValid);
			return;
			break;
		}
		default:
		{
			break;
		}
		}
		//}
	}
}

void UWrexa_Blueprint_Functions::LoadRawJPGBufferFromFile(const FString& File_Path_With_File_Name_And_Wihout_Extension, EJoyImageFormats ImageFormat, TArray<uint8>& Raw_JPEG_Buffer, bool& IsValid, int Resize_Width, int Resize_Height, EResize_Alogrithm_Type Resize_Algorithm_Type, const bool& Swap_Colour_Channel, const uint8& Quality_Factor_Percent)
{
	IsValid = false;

	//bool isOK = UWrexa_WebFileeDownloader::CreateWebFileFolder();
	//if (!isOK) return LoadedT2D;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr< IImageWrapper > ImageWrapper = ImageWrapperModule.CreateImageWrapper(GetJoyImageFormat(ImageFormat));

	//Load From File
	//FString FullFilePath = UWrexa_WebFileDownloader::WebFileFolder() + "/" + FileName + GetJoyImageExtension(ImageFormat);
	FString FullFilePath = File_Path_With_File_Name_And_Wihout_Extension + GetJoyImageExtension(ImageFormat);
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FullFilePath))
	{
		return;
	}

	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		TArray<uint8> UncompressedBGRA;
		if (ImageWrapper.Get()->GetRaw(ERGBFormat::RGBA, 8, UncompressedBGRA))
		{
			if (UncompressedBGRA.Num() == 0)
			{
				return;
			}

			TArray<uint8> RGBA_Image_Buffer;

			switch (Resize_Algorithm_Type)
			{
			case EResize_Alogrithm_Type::NONE:
			{
				Raw_JPEG_Buffer = RGBA_To_JPGE_Buffer(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), UncompressedBGRA, Quality_Factor_Percent, Swap_Colour_Channel, IsValid);
				return;
				break;
			}
			case EResize_Alogrithm_Type::Nearest_Neighbour:
			{
				RGBA_Image_Buffer = UWrexa_Blueprint_Functions::RGBA_Nearest_Neighbour_Resize(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), UncompressedBGRA, Resize_Width, Resize_Height, false);
				Raw_JPEG_Buffer = RGBA_To_JPGE_Buffer(Resize_Width, Resize_Height, RGBA_Image_Buffer, Quality_Factor_Percent, Swap_Colour_Channel, IsValid);
				return;
				break;
			}
			case EResize_Alogrithm_Type::Bilinear_Resize:
			{
				RGBA_Image_Buffer = UWrexa_Blueprint_Functions::RGBA_Bilinear_Resize(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), UncompressedBGRA, Resize_Width, Resize_Height, false);
				Raw_JPEG_Buffer = RGBA_To_JPGE_Buffer(Resize_Width, Resize_Height, RGBA_Image_Buffer, Quality_Factor_Percent, Swap_Colour_Channel, IsValid);
				return;
				break;
			}
			default:
			{
				break;
			}
			}
		}
	}
}

void UWrexa_Blueprint_Functions::LoadRawJPGBufferFromDownloaded_File(const UWrexa_WebFileDownloader* Downloaded_Context, EJoyImageFormats ImageFormat, TArray<uint8>& Raw_JPEG_Buffer, bool& IsValid, int Resize_Width, int Resize_Height, EResize_Alogrithm_Type Resize_Algorithm_Type, const bool& Swap_Colour_Channel, const uint8& Quality_Factor_Percent)
{
	IsValid = false;

	//bool isOK = UWrexa_WebFileeDownloader::CreateWebFileFolder();
	//if (!isOK) return LoadedT2D;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr< IImageWrapper > ImageWrapper = ImageWrapperModule.CreateImageWrapper(GetJoyImageFormat(ImageFormat));

	//Load From File
	if (!(Downloaded_Context->IsComplete))
	{
		return;
	}
	FString FullFilePath = Downloaded_Context->WebFileFolder() + "/" + Downloaded_Context->OutputFileName;
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FullFilePath))
	{
		return;
	}

	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		TArray<uint8> UncompressedBGRA;
		if (ImageWrapper.Get()->GetRaw(ERGBFormat::RGBA, 8, UncompressedBGRA))
		{
			if (UncompressedBGRA.Num() == 0)
			{
				return;
			}

			TArray<uint8> RGBA_Image_Buffer;

			switch (Resize_Algorithm_Type)
			{
			case EResize_Alogrithm_Type::NONE:
			{
				Raw_JPEG_Buffer = RGBA_To_JPGE_Buffer(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), UncompressedBGRA, Quality_Factor_Percent, Swap_Colour_Channel, IsValid);
				return;
				break;
			}
			case EResize_Alogrithm_Type::Nearest_Neighbour:
			{
				RGBA_Image_Buffer = UWrexa_Blueprint_Functions::RGBA_Nearest_Neighbour_Resize(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), UncompressedBGRA, Resize_Width, Resize_Height, false);
				Raw_JPEG_Buffer = RGBA_To_JPGE_Buffer(Resize_Width, Resize_Height, RGBA_Image_Buffer, Quality_Factor_Percent, Swap_Colour_Channel, IsValid);
				return;
				break;
			}
			case EResize_Alogrithm_Type::Bilinear_Resize:
			{
				RGBA_Image_Buffer = UWrexa_Blueprint_Functions::RGBA_Bilinear_Resize(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), UncompressedBGRA, Resize_Width, Resize_Height, false);
				Raw_JPEG_Buffer = RGBA_To_JPGE_Buffer(Resize_Width, Resize_Height, RGBA_Image_Buffer, Quality_Factor_Percent, Swap_Colour_Channel, IsValid);return;
				return;
				break;
			}
			default:
			{
				break;
			}
			}
		}
	}
}