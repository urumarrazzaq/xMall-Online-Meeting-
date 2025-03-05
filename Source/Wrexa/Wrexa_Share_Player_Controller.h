// Developed By Wrexa Technologies 2023

#pragma once

#include "CoreMinimal.h"
#include "Wrexa_Player_Controller.h"
#include "Wrexa_Blueprint_Functions.h"
#include "Wrexa_Screen_Share_Component.h"
#include "Wrexa_Share_Player_Controller.generated.h"

class UWrexa_Screen_Capture;

/**
 * 
 */
UCLASS()
class WREXA_API AWrexa_Share_Player_Controller : public AWrexa_Player_Controller
{
	GENERATED_BODY()

public:

	// Main Functions
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_Image_Helper_Stage_1();
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_Image_Helper_Stage_2_JPG(UWrexa_Image_Share_Component* Key);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_Image_Helper_Stage_2_RGB(UWrexa_Image_Share_Component* Key);


	// JPGE Image
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrexa | Share_Component | Image")
		float Next_JPG_Server_Request_To_Client_Delay_In_Seconds = 0.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wrexa | Share_Component | Image")
		TMap<UWrexa_Image_Share_Component*, FJPEG_Fragment_Struct> Sending_Shared_Image_JPGE;	// For The Sender	( Sending Client)					Not Replicated
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wrexa | Share_Component | Image")
		TMap<UWrexa_Image_Share_Component*, FJPEG_Fragment_Struct> Received_Shared_Image_JPGE;	// For The Receiver	( Receiving Server and Clients )	Manually Replicated

	// Local
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_JPG_Image(UWrexa_Image_Share_Component* Key, const TArray<uint8>& Buffer, const int& Max_Array_Size, const int& Offset_Order);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
	void Share_JPG_Image_RGBA_Resize_And_Convert(UWrexa_Image_Share_Component* Key, const TArray<uint8> RGBA_Buffer, const int& Original_Width, const int& Original_Height, const uint8& Quality_Factor_Percent, int Resize_Width, int Resize_Height, EResize_Alogrithm_Type Resize_Algorithm_Type, const int& Max_Array_Size, const int& Offset_Order);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_JPG_Image_Screen_Capture_Component(UWrexa_Image_Share_Component* Key, UWrexa_Screen_Capture* Screen_Capture_Contex, const int& Max_Array_Size, const uint8& Quality_Factor_Percent);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_JPG_Image_Screen_Capture_Component_With_Resize(UWrexa_Image_Share_Component* Key, UWrexa_Screen_Capture* Screen_Capture_Contex, const int& Max_Array_Size, const uint8& Quality_Factor_Percent
			, const int& Resize_Width, const int& Resize_Height);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_JPG_Image_Screen_Capture_Component_Bilinear_Resize_Resize(UWrexa_Image_Share_Component* Key, UWrexa_Screen_Capture* Screen_Capture_Contex, const int& Max_Array_Size, const uint8& Quality_Factor_Percent
			, const int& Resize_Width, const int& Resize_Height);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_JPG_Image_Screen_Capture_Component_Nearest_Neighbour_Resize(UWrexa_Image_Share_Component* Key, UWrexa_Screen_Capture* Screen_Capture_Contex, const int& Max_Array_Size, const uint8& Quality_Factor_Percent
			, const int& Resize_Width, const int& Resize_Height);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_Image_From_File(UWrexa_Image_Share_Component* Key, const FString& File_Path_With_File_Name_And_Wihout_Extension, EJoyImageFormats ImageFormat, const int& Max_Array_Size, const uint8& Quality_Factor_Percent, int Resize_Width, int Resize_Height, EResize_Alogrithm_Type Resize_Algorithm_Type);
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_Image_From_Downloaded_File(UWrexa_Image_Share_Component* Key, const UWrexa_WebFileDownloader* Downloaded_Context, EJoyImageFormats ImageFormat, const int& Max_Array_Size, const uint8& Quality_Factor_Percent, int Resize_Width, int Resize_Height, EResize_Alogrithm_Type Resize_Algorithm_Type);

	// Networked
	UFUNCTION(BlueprintCallable, Server, Unreliable, Category = "Wrexa | Share_Component | Image")
		void Share_Image_JPG_To_Server(UWrexa_Image_Share_Component* Target_Image_Share_Component,
			const int& Expected_Total_Fragments,
			const int& Expected_Each_Fragment_Size,
			const int& JPEG_Buffer_Size,
			const int& Received_Fragment_Index,
			const TArray<uint8>& Received_Bytes);

	UFUNCTION(BlueprintCallable, Client, Unreliable, Category = "Wrexa | Share_Component | Image")
		void Share_Image_JPG_Receive_Confirmation_To_Client(UWrexa_Image_Share_Component* Target_Image_Share_Component, const int& Received_Index);

	UFUNCTION(BlueprintCallable, Client, Unreliable, Category = "Wrexa | Share_Component | Image")
		void Share_Image_JPG_To_Client(UWrexa_Image_Share_Component* Target_Image_Share_Component,
			const int& Expected_Total_Fragments,
			const int& Expected_Each_Fragment_Size,
			const int& JPEG_Buffer_Size,
			const int& Received_Fragment_Index,
			const TArray<uint8>& Received_Bytes);

	// RAW RGB Image
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wrexa | Share_Component | Image")
	TMap<UWrexa_Image_Share_Component*, FRGB_Fragment_Map> Sending_Shared_Image_RGB;	// For The Sender	( Sending Client)					Not Replicated
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wrexa | Share_Component | Image")
	TMap<UWrexa_Image_Share_Component*, FRGB_Fragment_Map> Received_Shared_Image_RGB;	// For The Receiver	( Receiving Server and Clients )	Manually Replicated
	
	// Local
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_RGB_Image(UWrexa_Image_Share_Component* Key, const TArray<uint8>& Buffer, const int& Max_Array_Size, const int &Offset_Order, const bool& Is_RGBA,
			const int& Width, const int& Height);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_RGB_Image_Screen_Capture_Component(UWrexa_Image_Share_Component* Key, UWrexa_Screen_Capture* Screen_Capture_Contex, const int& Max_Array_Size, const int &Offset_Order);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_RGB_Image_Bilinear_Resize(UWrexa_Image_Share_Component* Key, TArray<uint8>& Buffer, const int& Max_Array_Size, const int& Offset_Order, const bool& Is_RGBA,
			const int& Original_Width, const int& Original_Height, const int& Resize_Width, const int& Resize_Height);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_RGB_Image_Screen_Capture_Component_Bilinear_Resize(UWrexa_Image_Share_Component* Key, UWrexa_Screen_Capture* Screen_Capture_Contex, const int& Max_Array_Size, const int& Offset_Order,
			const int& Resize_Width, const int& Resize_Height);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_RGB_Image_Nearest_Neighbour_Resize(UWrexa_Image_Share_Component* Key, TArray<uint8>& Buffer, const int& Max_Array_Size, const int& Offset_Order, const bool& Is_RGBA,
			const int& Original_Width, const int& Original_Height, const int& Resize_Width, const int& Resize_Height);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Share_RGB_Image_Screen_Capture_Component_Nearest_Neighbour_Resize(UWrexa_Image_Share_Component* Key, UWrexa_Screen_Capture* Screen_Capture_Contex, const int& Max_Array_Size, const int& Offset_Order,
			const int& Resize_Width, const int& Resize_Height);

	// Networked
	UFUNCTION(BlueprintCallable, Server, Unreliable, Category = "Wrexa | Share_Component | Image")
		void Share_Image_RGB_To_Server(UWrexa_Image_Share_Component* Target_Image_Share_Component,
										const int& Expected_Total_Fragments,
										const int& Expected_Each_Fragment_Size,
										const int& Resolution_Width,
										const int& Resolution_Height,
										const int& Received_Fragment_Index,
										const ERGB_Colour_Channel& Received_Colour_Channel,
										const TArray<uint8>& Received_Bytes);

	UFUNCTION(BlueprintCallable, Client, Unreliable, Category = "Wrexa | Share_Component | Image")
		void Share_Image_RGB_To_Client(UWrexa_Image_Share_Component* Target_Image_Share_Component,
										const int& Expected_Total_Fragments,
										const int& Expected_Each_Fragment_Size,
										const int& Resolution_Width,
										const int& Resolution_Height,
										const int& Received_Fragment_Index,
										const ERGB_Colour_Channel& Received_Colour_Channel,
										const TArray<uint8>& Received_Bytes);
};
