// Developed By Wrexa Technologies 2023

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
//#include "UnrealEd.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
//#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
//#include "UnrealEdClasses.h"

// Cmake Libries
THIRD_PARTY_INCLUDES_START
//#include "../Deps/jpge_compressor/jpge.h"//TODO CHECK THIS
//#include "../Deps/jpge_compressor/jpgd.h"//TODO CHECK THIS
//#include "jpge_compressor/jpge.h"
//#include "jpge_compressor/jpgd.h"

// ScreenCapture Requirments
//#include <algorithm>
//#include <atomic>
//#include <chrono>
//#include <climits>
//#include <iostream>
//#include <locale>
//#include <string>
//#include <thread>
//#include <vector>
THIRD_PARTY_INCLUDES_END

// WebFile Downloader
#include "Wrexa_WebFileDownloader.h"

#include "Wrexa_Blueprint_Functions.generated.h"

UENUM(BlueprintType)
enum class EJoyImageFormats : uint8
{
	JPG		UMETA(DisplayName = "JPG"),
	PNG		UMETA(DisplayName = "PNG"),
	BMP		UMETA(DisplayName = "BMP"),
	ICO		UMETA(DisplayName = "ICO"),
	EXR		UMETA(DisplayName = "EXR"),
	ICNS	UMETA(DisplayName = "ICNS")
};

UENUM(BlueprintType)
enum class ERGB_Colour_Channel : uint8
{
	RED		= 0 UMETA(DisplayName = "RED"),
	GREEN	= 1 UMETA(DisplayName = "GREEN"),
	BLUE	= 2 UMETA(DisplayName = "BLUE")
};

UENUM(BlueprintType)
enum EJPGE_subsampling_type : int
{
	Y_ONLY	 = 0 UMETA(DisplayName = "Y_ONLY"),
	H1V1	 = 1 UMETA(DisplayName = "H1V1"),
	H2V1	 = 2 UMETA(DisplayName = "H2V1"),
	H2V2	 = 3 UMETA(DisplayName = "H2V2"),
};

UENUM(BlueprintType)
enum EResize_Alogrithm_Type : int
{
	NONE = 0 UMETA(DisplayName = "NONE"),
	Nearest_Neighbour = 1 UMETA(DisplayName = "Nearest_Neighbour"),
	Bilinear_Resize = 2 UMETA(DisplayName = "Bilinear_Resize"),
};

USTRUCT(BlueprintType)
struct FRGB_Fragment_Map
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Expected_Total_Fragments = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Expected_Each_Fragment_Size = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Resolution_Width = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Resolution_Height = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<uint8> RED_Channel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<uint8> GREEN_Channel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<uint8> BLUE_Channel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Offset_Index = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Offset_Order = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ERGB_Colour_Channel Offset_Channel = ERGB_Colour_Channel::RED;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Fragments_Sent = 0;
};

USTRUCT(BlueprintType)
struct FJPEG_Fragment_Struct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Expected_Total_Fragments = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Expected_Each_Fragment_Size = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int JPEG_Buffer_Size = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<uint8> JPEG_Buffer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Sent_Fragment_Num = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Server_Confrim_Received_Num = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Max_Retry_Blocks = 1008;//Retry Attempt Is Hard Coded

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Current_Retry_Attempts = 0;
};

/**
 *
 */
UCLASS()
class WREXA_API UWrexa_Blueprint_Functions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//Networking
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Networking | Querry DNS | Get Host IP")
		static FString Querry_DNS_String(FString a_ServerAddress);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Networking | Querry DNS | Get Host IP")
		static FString Querry_DNS(FName a_ServerAddress);

	//Binary
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary")
		static FString ObjectBytesToString(TArray<uint8> In, int Count);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary")
		static FString SaveGameObjectToBinaryToString(USaveGame* SaveGameObject);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary")
		static USaveGame* StringToSaveGameObject(FString Input);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static void GetTextureData(const UTexture2D* Texture, TArray<uint8>& Pixels, int& Width, int& Height);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static void GetTextureData_RGB_Array(const UTexture2D* Texture, TArray<uint8>& RED_Channel, TArray<uint8>& GREEN_Channel, TArray<uint8>& BLUE_Channel, int& Width, int& Height);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static UTexture2D* TextureFrom_RGBA_Image(const bool& Swap_Colour_Channel, const int& Width, const int& Height, const TArray<uint8>& RGBA_Image_Buffer);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static UTexture2D* TextureFrom_RGB_Image(const bool& Swap_Colour_Channel, const int& Width, const int& Height, const TArray<uint8>& RED_Channel, const TArray<uint8>& GREEN_Channel, const TArray<uint8>& BLUE_Channel);

	static UTexture2D* TextureFrom_RGBA_Image_From_Pointer(const bool& Swap_Colour_Channel, const int& Width, const int& Height, uint8* RGBA_Image_Buffer);
	
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static UTexture2D* TextureFrom_RGB_Image_Single_Buffer(const bool& Swap_Colour_Channel, const int& Width, const int& Height, const TArray<uint8>& RGB_Image_Buffer);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static UTexture2D* TextureFrom_JPGE_Image(const bool& Swap_Colour_Channel, const TArray<uint8>& JPG_Image_Buffer);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static UTexture2D* TextureFrom_Image(const bool& Swap_Colour_Channel, int& Return_Width, int& Return_Height, const TArray<uint8>& Image_Buffer, EJoyImageFormats ImageFormat);

	static bool Make_Fragmented_Data_From_RGBA_By_Pointer(const TArray<uint8>& Raw_Data, const int& Max_Array_Size, int& Total_Fragments, FRGB_Fragment_Map* Fragmented_RGB_Data_Map);
	static bool Make_Fragmented_Data_From_RGB_By_Pointer(const TArray<uint8>& Raw_Data, const int& Max_Array_Size, int& Total_Fragments, FRGB_Fragment_Map* Fragmented_RGB_Data_Map);
	static bool Make_Fragmented_Data_From_JPGE_By_Pointer(const TArray<uint8>& Raw_Data, const int& Max_Array_Size, int& Total_Fragments, FJPEG_Fragment_Struct* Fragmented_JPGE_Data_Map);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static bool Make_Fragmented_Data_From_RGBA(const TArray<uint8>& Raw_Data, const int& Max_Array_Size, int& Total_Fragments, FRGB_Fragment_Map& Fragmented_RGB_Data_Map);
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static bool Make_Fragmented_Data_From_RGB(const TArray<uint8>& Raw_Data, const int& Max_Array_Size, int& Total_Fragments, FRGB_Fragment_Map& Fragmented_RGB_Data_Map);
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static bool Make_Fragmented_Data_From_JPGE(const TArray<uint8>& Raw_Data, const int& Max_Array_Size, int& Total_Fragments, FJPEG_Fragment_Struct& Fragmented_JPGE_Data_Map);

	//Binary | Image
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static TArray<uint8> RGBA_Bilinear_Resize(int Original_Width, int Original_Height, TArray<uint8> RGBA_Image_Buffer_Fragment, int Resize_Width, int Resize_Height, const bool& Ignore_Alpha);
	
	static void RGBA_Bilinear_Resize_Reference(int Original_Width, int Original_Height, uint8* RGBA_Image_Buffer_Fragment, TArray<uint8>& Resized_RGBA_Buffer, int Resize_Width, int Resize_Height, const bool& Ignore_Alpha);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static TArray<uint8> RGB_Bilinear_Resize(int Original_Width, int Original_Height, TArray<uint8> RGB_Image_Buffer_Fragment, int Resize_Width, int Resize_Height);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static TArray<uint8> RGBA_Nearest_Neighbour_Resize(int Original_Width, int Original_Height, TArray<uint8> RGBA_Image_Buffer_Fragment, int Resize_Width, int Resize_Height, const bool& Ignore_Alpha);

	static void RGBA_Nearest_Neighbour_Resize_Reference(int Original_Width, int Original_Height, uint8* RGBA_Image_Buffer_Fragment, TArray<uint8>& Resized_RGBA_Buffer, int Resize_Width, int Resize_Height, const bool& Ignore_Alpha);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static TArray<uint8> RGB_Nearest_Neighbour_Resize(int Original_Width, int Original_Height, TArray<uint8> RGB_Image_Buffer_Fragment, int Resize_Width, int Resize_Height);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static TArray<uint8> RGBA_To_RGB_Buffer(int Width, int Height, TArray<uint8> RGBA_Image_Buffer_Fragment);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static TArray<uint8> RGB_To_RGBA_Buffer(int Width, int Height, TArray<uint8> RGB_Image_Buffer_Fragment);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static TArray<uint8> RGBA_To_JPGE_Buffer(const int& Width, const int& Height, TArray<uint8> RGBA_Image_Buffer, const uint8& Quality_Factor_Percent, const bool& Swap_Colour_Channel, bool& IsSuccessful);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static TArray<uint8> RGB_To_JPGE_Buffer(const int& Width, const int& Height, TArray<uint8> RGB_Image_Buffer, const uint8& Quality_Factor_Percent, const bool& Swap_Colour_Channel, bool& IsSuccessful);

	static void RGBA_To_JPGE_Buffer_Reference(const int& Width, const int& Height, uint8* RGBA_Image_Buffer, TArray<uint8>& Return_Array, const uint8& Quality_Factor_Percent, const bool& Swap_Colour_Channel, bool& IsSuccessful);
	static void RGBA_To_JPGE_Buffer_With_Resize_Reference(const int& Width, const int& Height, int Resize_Width, int Resize_Height, uint8* RGBA_Image_Buffer, TArray<uint8>& Return_Array, const uint8& Quality_Factor_Percent, const bool& Swap_Colour_Channel, bool& IsSuccessful);
	
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Binary | Image")
		static TArray<uint8> JPGE_To_RGBA_Buffer(TArray<uint8> JPGE_Buffer, int& Output_Width, int& Output_Height, int& Total_Colour_Channels, bool& IsSuccessful);

	// Extra
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Extra")
	static TArray<APlayerController*> Get_All_Local_Player_Controllers(UGameInstance *GameInstance);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Extra")
		static TArray<int> Reorder_Array(const TArray<int> &Array_To_Order, bool IsAscending);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Extra")
		static int Get_Size(const TArray<int>& Array_To_Order);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Extra")
		static uint8 Get_Array_Average(const TArray<uint8>& Array_To_Get_Average);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Extra")
		static void Shutdown();


	//Widget
	//class SWidget;
	//TSharedPtr<SWidget> SlateWidget;
	//class FWidgetRenderer* WidgetRenderer = nullptr;// = new FWidgetRenderer(true);

	//UFUNCTION(BlueprintCallable, Category = "Wrexa | Widget")
	//	UTexture2D* TextureFromWidget(UUserWidget* const Widget, const FVector2D& DrawSize);

	//UFUNCTION(BlueprintCallable, Category = "Wrexa | Widget")
	//	static UTextureRenderTarget2D* WidgetToTexture(UUserWidget* const widget, const FVector2D& drawSize);

	//Screen Capture
	//static void TestCopyContiguous();
	//static void TestCopyNonContiguous();
	//static void ExtractAndConvertToRGBA(const SL::Screen_Capture::Image& img, unsigned char* dst, size_t dst_size);
	//static void createframegrabber();
	//static void createpartialframegrabber();
	//static std::vector<SL::Screen_Capture::Window> getWindowToCapture(std::string window_to_search_for);
	//static void createwindowgrabber();

	// File -> Image Format



	//UFUNCTION(BlueprintCallable, Category = "Wrexa | File | Image");
		static EImageFormat GetJoyImageFormat(EJoyImageFormats JoyFormat);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | File | Image")
		static FString GetJoyImageExtension(EJoyImageFormats JoyFormat);
	
	UFUNCTION(BlueprintCallable, Category = "Wrexa | File | Image")
		static UTexture2D* LoadTexture2DFromFile(const FString& File_Path_With_File_Name_And_Wihout_Extension, EJoyImageFormats ImageFormat, bool& IsValid, int32& Width, int32& Height, const bool& Swap_Colour_Channel);
	
	UFUNCTION(BlueprintCallable, Category = "Wrexa | File | Image")
		UTexture2D* LoadTexture2DFromDownloaded_File(const UWrexa_WebFileDownloader* Downloaded_Context, EJoyImageFormats ImageFormat, bool& IsValid, int32& Width, int32& Height, const bool& Swap_Colour_Channel);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | File | Image")
		static void LoadRawJPGBufferFromRGBA_Buffer(const TArray<uint8>& Raw_RGBA_Data, const int& Width, const int& Height, TArray<uint8>& Raw_JPEG_Buffer, bool& IsValid, int Resize_Width, int Resize_Height, EResize_Alogrithm_Type Resize_Algorithm_Type, const bool& Swap_Colour_Channel, const uint8& Quality_Factor_Percent);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | File | Image")
		static void LoadRawJPGBufferFromFile(const FString& File_Path_With_File_Name_And_Wihout_Extension, EJoyImageFormats ImageFormat, TArray<uint8>& Raw_JPEG_Buffer, bool& IsValid, int Resize_Width, int Resize_Height, EResize_Alogrithm_Type Resize_Algorithm_Type, const bool& Swap_Colour_Channel, const uint8& Quality_Factor_Percent);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | File | Image")
		static void LoadRawJPGBufferFromDownloaded_File(const UWrexa_WebFileDownloader* Downloaded_Context, EJoyImageFormats ImageFormat, TArray<uint8>& Raw_JPEG_Buffer, bool& IsValid, int Resize_Width, int Resize_Height, EResize_Alogrithm_Type Resize_Algorithm_Type, const bool& Swap_Colour_Channel, const uint8& Quality_Factor_Percent);
};
