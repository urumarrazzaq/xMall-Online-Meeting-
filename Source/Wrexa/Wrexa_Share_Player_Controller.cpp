// Developed By Wrexa Technologies 2023


#include "Wrexa_Share_Player_Controller.h"
#include "Wrexa_Screen_Capture.h"

//#pragma optimize("", on)
void AWrexa_Share_Player_Controller::Share_Image_Helper_Stage_1()
{
	for (const TPair<UWrexa_Image_Share_Component*, FJPEG_Fragment_Struct>& pair : Sending_Shared_Image_JPGE)
	{
		FJPEG_Fragment_Struct* Fragment_Map_Reference = Sending_Shared_Image_JPGE.Find(pair.Key);

		if (Fragment_Map_Reference == nullptr)
		{
			break;
		}

		if ((Fragment_Map_Reference->Sent_Fragment_Num) > (Fragment_Map_Reference->Server_Confrim_Received_Num))
		{
			if (Fragment_Map_Reference->Current_Retry_Attempts > Fragment_Map_Reference->Max_Retry_Blocks)
			{
				Fragment_Map_Reference->Current_Retry_Attempts = 0;
				Fragment_Map_Reference->Sent_Fragment_Num = Fragment_Map_Reference->Server_Confrim_Received_Num;
			}
			else
			{
				Fragment_Map_Reference->Current_Retry_Attempts += 1;
				break;
			}
		}

		if (Fragment_Map_Reference->Server_Confrim_Received_Num < pair.Value.Expected_Total_Fragments)
		{
			Share_Image_Helper_Stage_2_JPG(pair.Key);
			return;
		}
	}

	for (const TPair<UWrexa_Image_Share_Component*, FRGB_Fragment_Map>& pair : Sending_Shared_Image_RGB)
	{
		if (pair.Value.Fragments_Sent < pair.Value.Expected_Total_Fragments)
		{
			Share_Image_Helper_Stage_2_RGB(pair.Key);
			return;
		}
	}
}
//#pragma optimize("", off)
void AWrexa_Share_Player_Controller::Share_Image_Helper_Stage_2_JPG(UWrexa_Image_Share_Component* Key)
{
	if (Key == nullptr)
	{
		return;
	}

	if (!IsValid(Key))
	{
		return;
	}

	FJPEG_Fragment_Struct* Fragment_Map_Reference = Sending_Shared_Image_JPGE.Find(Key);

	if (Fragment_Map_Reference == nullptr)
	{
		return;
	}

	if (0 >= Fragment_Map_Reference->Expected_Total_Fragments)
	{
		return;
	}

	if (Fragment_Map_Reference->Sent_Fragment_Num >= Fragment_Map_Reference->Expected_Total_Fragments)
	{
		return;
	}

	TArray<uint8> Colour_Channel_Bytes;
	TArray<uint8>& Returned_Colour_Channel_Bytes = Fragment_Map_Reference->JPEG_Buffer;

	if (Returned_Colour_Channel_Bytes.Num() > (Fragment_Map_Reference->Expected_Total_Fragments * Fragment_Map_Reference->Expected_Each_Fragment_Size))
	{
		return;
	}

	int Start = Fragment_Map_Reference->Sent_Fragment_Num * Fragment_Map_Reference->Expected_Each_Fragment_Size;
	if (Fragment_Map_Reference->Sent_Fragment_Num >= (Fragment_Map_Reference->Expected_Total_Fragments - 1))
	{
		Colour_Channel_Bytes.Init(0, ((Returned_Colour_Channel_Bytes.Num()) % (Fragment_Map_Reference->Expected_Each_Fragment_Size)));
	}
	else
	{
		Colour_Channel_Bytes.Init(0, Fragment_Map_Reference->Expected_Each_Fragment_Size);
	}

	//int End = Start + Expected_Each_Fragment_Size;
	for (int i = 0; i < Colour_Channel_Bytes.Num(); ++i)
	{
		Colour_Channel_Bytes[i] = Returned_Colour_Channel_Bytes[(Start + i)];
	}

	Share_Image_JPG_To_Server(
		Key,
		Fragment_Map_Reference->Expected_Total_Fragments,
		Fragment_Map_Reference->Expected_Each_Fragment_Size,
		Fragment_Map_Reference->JPEG_Buffer_Size,
		(Fragment_Map_Reference->Sent_Fragment_Num)++,//Post Increment
		Colour_Channel_Bytes);

	//Stage 1 Restart
	//Share_Image_Helper_Stage_1();	Will Cause Reliable Buffer Overflow If Too Much Data is Sent Too Quickly
}

void AWrexa_Share_Player_Controller::Share_Image_Helper_Stage_2_RGB(UWrexa_Image_Share_Component* Key)
{
	if (Key == nullptr)
	{
		return;
	}

	if (!IsValid(Key))
	{
		return;
	}

	FRGB_Fragment_Map* Fragment_Map_Reference = Sending_Shared_Image_RGB.Find(Key);

	if (Fragment_Map_Reference == nullptr)
	{
		return;
	}

	if (0 >= Fragment_Map_Reference->Expected_Total_Fragments)
	{
		return;
	}

	if (Fragment_Map_Reference->Fragments_Sent >= Fragment_Map_Reference->Expected_Total_Fragments)
	{
		return;
	}

	if (Fragment_Map_Reference->Offset_Index >= Fragment_Map_Reference->Expected_Total_Fragments)
	{
		return;
		//Fragment_Map_Reference->Offset_Index = 0;
	}

	TArray<uint8> Colour_Channel_Bytes;
	TArray<uint8>& Returned_Colour_Channel_Bytes = ((Fragment_Map_Reference->Offset_Channel == ERGB_Colour_Channel::RED) ? (Fragment_Map_Reference->RED_Channel) :
		((Fragment_Map_Reference->Offset_Channel == ERGB_Colour_Channel::BLUE) ? (Fragment_Map_Reference->BLUE_Channel) :
			(Fragment_Map_Reference->GREEN_Channel)));

	if (Returned_Colour_Channel_Bytes.Num() > (Fragment_Map_Reference->Expected_Total_Fragments * Fragment_Map_Reference->Expected_Each_Fragment_Size))
	{
		return;
	}

	int Start = Fragment_Map_Reference->Offset_Index * Fragment_Map_Reference->Expected_Each_Fragment_Size;
	if (Fragment_Map_Reference->Offset_Index >= (Fragment_Map_Reference->Expected_Total_Fragments - 1))
	{
		Colour_Channel_Bytes.Init(0, ((Returned_Colour_Channel_Bytes.Num()) % (Fragment_Map_Reference->Expected_Each_Fragment_Size)));
	}
	else
	{
		Colour_Channel_Bytes.Init(0, Fragment_Map_Reference->Expected_Each_Fragment_Size);
	}

	//int End = Start + Expected_Each_Fragment_Size;
	for (int i = 0; i < Colour_Channel_Bytes.Num(); ++i)
	{
		Colour_Channel_Bytes[i] = Returned_Colour_Channel_Bytes[(Start + i)];
	}

	Share_Image_RGB_To_Server(
		Key,
		Fragment_Map_Reference->Expected_Total_Fragments,
		Fragment_Map_Reference->Expected_Each_Fragment_Size,
		Fragment_Map_Reference->Resolution_Width,
		Fragment_Map_Reference->Resolution_Height,
		Fragment_Map_Reference->Offset_Index,
		Fragment_Map_Reference->Offset_Channel,
		Colour_Channel_Bytes);

	switch (Fragment_Map_Reference->Offset_Channel)
	{
	case ERGB_Colour_Channel::RED:
	{
		Fragment_Map_Reference->Offset_Channel = ERGB_Colour_Channel::GREEN;
		break;
	}

	case ERGB_Colour_Channel::GREEN:
	{
		Fragment_Map_Reference->Offset_Channel = ERGB_Colour_Channel::BLUE;
		break;
	}

	case ERGB_Colour_Channel::BLUE:
	{
		Fragment_Map_Reference->Offset_Channel = ERGB_Colour_Channel::RED;
		break;
	}

	default:
	{
		break;
	}
	}

	if ((Fragment_Map_Reference->Offset_Order == 0) || (Fragment_Map_Reference->Offset_Order == (Fragment_Map_Reference->Expected_Total_Fragments - 1)))
	{
		Fragment_Map_Reference->Offset_Index += 1;
	}
	else
	{
		Fragment_Map_Reference->Offset_Index += Fragment_Map_Reference->Offset_Order;
	}

	Fragment_Map_Reference->Offset_Index += (bool)((Fragment_Map_Reference->Offset_Index) / (Fragment_Map_Reference->Expected_Total_Fragments));
	Fragment_Map_Reference->Offset_Index = (Fragment_Map_Reference->Offset_Index) % (Fragment_Map_Reference->Expected_Total_Fragments);

	Fragment_Map_Reference->Fragments_Sent += 1;

	//Stage 1 Restart
	//Share_Image_Helper_Stage_1();	Will Cause Reliable Buffer Overflow If Too Much Data is Sent Too Quickly
}
//#pragma optimize("", off)

//#pragma optimize("", off)
void AWrexa_Share_Player_Controller::Share_JPG_Image(UWrexa_Image_Share_Component* Key, const TArray<uint8>& Buffer, const int& Max_Array_Size, const int& Offset_Order)
{
	if (Key == nullptr)
	{
		return;
	}

	bool IsSuccessful = false;
	FJPEG_Fragment_Struct* Fragment_Map_Reference = Sending_Shared_Image_JPGE.Find(Key);

	if (Fragment_Map_Reference == nullptr)
	{
		Fragment_Map_Reference = &(Sending_Shared_Image_JPGE.Add(Key));
		Fragment_Map_Reference->Sent_Fragment_Num = 0;
		Fragment_Map_Reference->Server_Confrim_Received_Num = 0;
		Fragment_Map_Reference->Current_Retry_Attempts = 0;
		//Fragment_Map_Reference->Fragments_Sent_Server_Confirmed.Empty();
	}
	else
	{
		if ((Fragment_Map_Reference->Server_Confrim_Received_Num) >= (Fragment_Map_Reference->Expected_Total_Fragments))
		{
			Fragment_Map_Reference->Sent_Fragment_Num = 0;
			Fragment_Map_Reference->Server_Confrim_Received_Num = 0;
			Fragment_Map_Reference->Current_Retry_Attempts = 0;
		}
	}
	
	if ((Fragment_Map_Reference->Sent_Fragment_Num) > (Fragment_Map_Reference->Server_Confrim_Received_Num))
	{
		if (Fragment_Map_Reference->Current_Retry_Attempts > Fragment_Map_Reference->Max_Retry_Blocks)
		{
			Fragment_Map_Reference->Current_Retry_Attempts = 0;
			Fragment_Map_Reference->Sent_Fragment_Num = Fragment_Map_Reference->Server_Confrim_Received_Num;
		}
		else
		{
			Fragment_Map_Reference->Current_Retry_Attempts += 1;
			UTexture2D* Returned_Texture = UWrexa_Blueprint_Functions::TextureFrom_JPGE_Image(
				false,
				Buffer);

			if (Returned_Texture == nullptr)
			{
				return;
			}

			Key->Set_New_Texture(Returned_Texture);
			Share_Image_Helper_Stage_1();// This will Try to Restart the Process Not Intended, May Be CPU Intensive
			return;
		}		
	}


	int Total_Fragments = 0;
	IsSuccessful = UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_JPGE_By_Pointer(Buffer, Max_Array_Size, Total_Fragments, Fragment_Map_Reference);

	if (IsSuccessful)
	{
		Fragment_Map_Reference->JPEG_Buffer_Size = Buffer.Num();
		//Key->On_RGB_Fragment_Received_Pointer(Fragment_Map_Reference);
		Key->On_JPG_Fragment_Received(Sending_Shared_Image_JPGE.FindRef(Key));
		Share_Image_Helper_Stage_1();// This will Try to Restart the Process Not Intended, May Be CPU Intensive
	}
}

void AWrexa_Share_Player_Controller::Share_JPG_Image_RGBA_Resize_And_Convert(UWrexa_Image_Share_Component* Key, const TArray<uint8> RGBA_Buffer, const int& Original_Width, const int& Original_Height, const uint8& Quality_Factor_Percent, int Resize_Width, int Resize_Height, EResize_Alogrithm_Type Resize_Algorithm_Type, const int& Max_Array_Size, const int& Offset_Order)
{
	bool Is_Successful = true;
	TArray<uint8> Raw_JPEG_Buffer;
	UWrexa_Blueprint_Functions::LoadRawJPGBufferFromRGBA_Buffer(RGBA_Buffer, Original_Width, Original_Height, Raw_JPEG_Buffer, Is_Successful, Resize_Width, Resize_Height, Resize_Algorithm_Type, true, Quality_Factor_Percent);
	if (Is_Successful)
	{
		Share_JPG_Image(Key, Raw_JPEG_Buffer, Max_Array_Size, Offset_Order);
	}
}

//#pragma optimize("", off)
void AWrexa_Share_Player_Controller::Share_JPG_Image_Screen_Capture_Component(UWrexa_Image_Share_Component* Key, UWrexa_Screen_Capture* Screen_Capture_Contex, const int& Max_Array_Size, const uint8& Quality_Factor_Percent)
{
	if (Key == nullptr)
	{
		return;
	}

	if (Screen_Capture_Contex == nullptr)
	{
		return;
	}

	bool IsSuccessful = false;
	FJPEG_Fragment_Struct* Fragment_Map_Reference = Sending_Shared_Image_JPGE.Find(Key);

	if (Fragment_Map_Reference == nullptr)
	{
		Fragment_Map_Reference = &(Sending_Shared_Image_JPGE.Add(Key));
		Fragment_Map_Reference->Sent_Fragment_Num = 0;
		Fragment_Map_Reference->Server_Confrim_Received_Num = 0;
		Fragment_Map_Reference->Current_Retry_Attempts = 0;
		//Fragment_Map_Reference->Fragments_Sent_Server_Confirmed.Empty();
	}
	else
	{
		if ((Fragment_Map_Reference->Server_Confrim_Received_Num) >= (Fragment_Map_Reference->Expected_Total_Fragments))
		{
			Fragment_Map_Reference->Sent_Fragment_Num = 0;
			Fragment_Map_Reference->Server_Confrim_Received_Num = 0;
			Fragment_Map_Reference->Current_Retry_Attempts = 0;
		}
	}

	int Width = 0;
	int Height = 0;

	int Total_Fragments = 0;	
	
	bool True_For_RGBA_False_For_RGB = false;
	uint8* Buffer = nullptr;	
	
	Screen_Capture_Contex->Image_From_Screen_Capture_Pointer(IsSuccessful, Width, Height, &Buffer, True_For_RGBA_False_For_RGB);
	if (!IsSuccessful)
	{
		return;
	}	

	TArray<uint8> JPG_Buffer;
	
	if (True_For_RGBA_False_For_RGB)
	{
		UWrexa_Blueprint_Functions::RGBA_To_JPGE_Buffer_Reference(Width, Height, Buffer, JPG_Buffer, Quality_Factor_Percent, true, IsSuccessful);
	}
	else
	{
		//UWrexa_Blueprint_Functions::RGB_To_JPGE_Buffer_Reference(Width, Height, Buffer, JPG_Buffer, Quality_Factor_Percent, true, IsSuccessful);
		return;
	}
		
	if (!IsSuccessful)
	{
		return;
	}

	if ((Fragment_Map_Reference->Sent_Fragment_Num) > (Fragment_Map_Reference->Server_Confrim_Received_Num))
	{
		if (Fragment_Map_Reference->Current_Retry_Attempts > Fragment_Map_Reference->Max_Retry_Blocks)
		{
			Fragment_Map_Reference->Current_Retry_Attempts = 0;
			Fragment_Map_Reference->Sent_Fragment_Num = Fragment_Map_Reference->Server_Confrim_Received_Num;
		}
		else
		{
			Fragment_Map_Reference->Current_Retry_Attempts += 1;
			UTexture2D* Returned_Texture = UWrexa_Blueprint_Functions::TextureFrom_JPGE_Image(
				false,
				JPG_Buffer);

			if (Returned_Texture == nullptr)
			{
				return;
			}

			Key->Set_New_Texture(Returned_Texture);
			Share_Image_Helper_Stage_1();
			return;
		}
	}

	//Fragment_Map_Reference->Resolution_Width = Width;
	//Fragment_Map_Reference->Resolution_Height = Height;

	IsSuccessful = UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_JPGE_By_Pointer(JPG_Buffer, Max_Array_Size, Total_Fragments, Fragment_Map_Reference);	

	if (IsSuccessful)
	{
		Fragment_Map_Reference->JPEG_Buffer_Size = JPG_Buffer.Num();
		//Key->On_RGB_Fragment_Received_Pointer(Fragment_Map_Reference);
		Key->On_JPG_Fragment_Received(Sending_Shared_Image_JPGE.FindRef(Key));
		Share_Image_Helper_Stage_1();
	}
}

void AWrexa_Share_Player_Controller::Share_JPG_Image_Screen_Capture_Component_With_Resize(UWrexa_Image_Share_Component* Key, UWrexa_Screen_Capture* Screen_Capture_Contex, const int& Max_Array_Size, const uint8& Quality_Factor_Percent
	, const int& Resize_Width, const int& Resize_Height)
{
	if (Key == nullptr)
	{
		return;
	}

	if (Screen_Capture_Contex == nullptr)
	{
		return;
	}

	bool IsSuccessful = false;
	FJPEG_Fragment_Struct* Fragment_Map_Reference = Sending_Shared_Image_JPGE.Find(Key);

	if (Fragment_Map_Reference == nullptr)
	{
		Fragment_Map_Reference = &(Sending_Shared_Image_JPGE.Add(Key));
		Fragment_Map_Reference->Sent_Fragment_Num = 0;
		Fragment_Map_Reference->Server_Confrim_Received_Num = 0;
		Fragment_Map_Reference->Current_Retry_Attempts = 0;
		//Fragment_Map_Reference->Fragments_Sent_Server_Confirmed.Empty();
	}
	else
	{
		if ((Fragment_Map_Reference->Server_Confrim_Received_Num) >= (Fragment_Map_Reference->Expected_Total_Fragments))
		{
			Fragment_Map_Reference->Sent_Fragment_Num = 0;
			Fragment_Map_Reference->Server_Confrim_Received_Num = 0;
			Fragment_Map_Reference->Current_Retry_Attempts = 0;
		}
	}

	int Width = 0;
	int Height = 0;

	int Total_Fragments = 0;

	uint8* Buffer = nullptr;

	Screen_Capture_Contex->Image_From_Screen_Capture_Pointer_Unsafe(IsSuccessful, Width, Height, Resize_Width, Resize_Height, &Buffer);
	if (!IsSuccessful)
	{
		return;
	}

	TArray<uint8> JPG_Buffer;

	UWrexa_Blueprint_Functions::RGBA_To_JPGE_Buffer_With_Resize_Reference(Width, Height, Resize_Width, Resize_Height, Buffer, JPG_Buffer, Quality_Factor_Percent, true, IsSuccessful);

	if (!IsSuccessful)
	{
		return;
	}

	if ((Fragment_Map_Reference->Sent_Fragment_Num) > (Fragment_Map_Reference->Server_Confrim_Received_Num))
	{
		if (Fragment_Map_Reference->Current_Retry_Attempts > Fragment_Map_Reference->Max_Retry_Blocks)
		{
			Fragment_Map_Reference->Current_Retry_Attempts = 0;
			Fragment_Map_Reference->Sent_Fragment_Num = Fragment_Map_Reference->Server_Confrim_Received_Num;
		}
		else
		{
			Fragment_Map_Reference->Current_Retry_Attempts += 1;
			UTexture2D* Returned_Texture = UWrexa_Blueprint_Functions::TextureFrom_JPGE_Image(
				false,
				JPG_Buffer);

			if (Returned_Texture == nullptr)
			{
				return;
			}

			Key->Set_New_Texture(Returned_Texture);
			Share_Image_Helper_Stage_1();
			return;
		}
	}

	//Fragment_Map_Reference->Resolution_Width = Width;
	//Fragment_Map_Reference->Resolution_Height = Height;

	IsSuccessful = UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_JPGE_By_Pointer(JPG_Buffer, Max_Array_Size, Total_Fragments, Fragment_Map_Reference);

	if (IsSuccessful)
	{
		Fragment_Map_Reference->JPEG_Buffer_Size = JPG_Buffer.Num();
		//Key->On_RGB_Fragment_Received_Pointer(Fragment_Map_Reference);
		Key->On_JPG_Fragment_Received(Sending_Shared_Image_JPGE.FindRef(Key));
		Share_Image_Helper_Stage_1();
	}
}


void AWrexa_Share_Player_Controller::Share_JPG_Image_Screen_Capture_Component_Bilinear_Resize_Resize(UWrexa_Image_Share_Component* Key, UWrexa_Screen_Capture* Screen_Capture_Contex, const int& Max_Array_Size, const uint8& Quality_Factor_Percent
	, const int& Resize_Width, const int& Resize_Height)
{
	if (Key == nullptr)
	{
		return;
	}

	if (Screen_Capture_Contex == nullptr)
	{
		return;
	}

	bool IsSuccessful = false;
	FJPEG_Fragment_Struct* Fragment_Map_Reference = Sending_Shared_Image_JPGE.Find(Key);

	if (Fragment_Map_Reference == nullptr)
	{
		Fragment_Map_Reference = &(Sending_Shared_Image_JPGE.Add(Key));
		Fragment_Map_Reference->Sent_Fragment_Num = 0;
		Fragment_Map_Reference->Server_Confrim_Received_Num = 0;
		Fragment_Map_Reference->Current_Retry_Attempts = 0;
		//Fragment_Map_Reference->Fragments_Sent_Server_Confirmed.Empty();
	}
	else
	{
		if ((Fragment_Map_Reference->Server_Confrim_Received_Num) >= (Fragment_Map_Reference->Expected_Total_Fragments))
		{
			Fragment_Map_Reference->Sent_Fragment_Num = 0;
			Fragment_Map_Reference->Server_Confrim_Received_Num = 0;
			Fragment_Map_Reference->Current_Retry_Attempts = 0;
		}
	}

	int Width = 0;
	int Height = 0;

	int Total_Fragments = 0;

	bool True_For_RGBA_False_For_RGB = false;
	uint8* Buffer = nullptr;

	Screen_Capture_Contex->Image_From_Screen_Capture_Pointer(IsSuccessful, Width, Height, &Buffer, True_For_RGBA_False_For_RGB);
	if (!IsSuccessful)
	{
		return;
	}

	TArray<uint8> JPG_Buffer;

	int To_Width = 0;
	int To_Height = 0;
	if ((Resize_Width <= 0) || (Resize_Height <= 0))
	{
		To_Width = Width;
		To_Height = Height;
	}
	else
	{
		To_Width = Resize_Width;
		To_Height = Resize_Height;
	}

	if (True_For_RGBA_False_For_RGB)
	{
		UWrexa_Blueprint_Functions::RGBA_Bilinear_Resize_Reference(Width, Height, Buffer, JPG_Buffer, To_Width, To_Height, true);
		UWrexa_Blueprint_Functions::RGBA_To_JPGE_Buffer_Reference(To_Width, To_Height, JPG_Buffer.GetData(), JPG_Buffer, Quality_Factor_Percent, true, IsSuccessful);
	}
	else
	{
		//Buffer = UWrexa_Blueprint_Functions::RGB_Bilinear_Resize(Width, Height, Buffer, To_Width, To_Height);
		//JPG_Buffer = UWrexa_Blueprint_Functions::RGB_To_JPGE_Buffer(To_Width, To_Height, Buffer, Quality_Factor_Percent, true, IsSuccessful);
		return;
	}

	if (!IsSuccessful)
	{
		return;
	}

	if ((Fragment_Map_Reference->Sent_Fragment_Num) > (Fragment_Map_Reference->Server_Confrim_Received_Num))
	{
		if (Fragment_Map_Reference->Current_Retry_Attempts > Fragment_Map_Reference->Max_Retry_Blocks)
		{
			Fragment_Map_Reference->Current_Retry_Attempts = 0;
			Fragment_Map_Reference->Sent_Fragment_Num = Fragment_Map_Reference->Server_Confrim_Received_Num;
		}
		else
		{
			Fragment_Map_Reference->Current_Retry_Attempts += 1;
			UTexture2D* Returned_Texture = UWrexa_Blueprint_Functions::TextureFrom_JPGE_Image(
				false,
				JPG_Buffer);

			if (Returned_Texture == nullptr)
			{
				return;
			}

			Key->Set_New_Texture(Returned_Texture);
			Share_Image_Helper_Stage_1();
			return;
		}
	}

	//Fragment_Map_Reference->Resolution_Width = Width;
	//Fragment_Map_Reference->Resolution_Height = Height;

	IsSuccessful = UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_JPGE_By_Pointer(JPG_Buffer, Max_Array_Size, Total_Fragments, Fragment_Map_Reference);

	if (IsSuccessful)
	{
		Fragment_Map_Reference->JPEG_Buffer_Size = JPG_Buffer.Num();
		//Key->On_RGB_Fragment_Received_Pointer(Fragment_Map_Reference);
		Key->On_JPG_Fragment_Received(Sending_Shared_Image_JPGE.FindRef(Key));
		Share_Image_Helper_Stage_1();
	}
}

void AWrexa_Share_Player_Controller::Share_JPG_Image_Screen_Capture_Component_Nearest_Neighbour_Resize(UWrexa_Image_Share_Component* Key, UWrexa_Screen_Capture* Screen_Capture_Contex, const int& Max_Array_Size, const uint8& Quality_Factor_Percent
	, const int& Resize_Width, const int& Resize_Height)
{
	if (Key == nullptr)
	{
		return;
	}

	if (Screen_Capture_Contex == nullptr)
	{
		return;
	}

	bool IsSuccessful = false;
	FJPEG_Fragment_Struct* Fragment_Map_Reference = Sending_Shared_Image_JPGE.Find(Key);

	if (Fragment_Map_Reference == nullptr)
	{
		Fragment_Map_Reference = &(Sending_Shared_Image_JPGE.Add(Key));
		Fragment_Map_Reference->Sent_Fragment_Num = 0;
		Fragment_Map_Reference->Server_Confrim_Received_Num = 0;
		Fragment_Map_Reference->Current_Retry_Attempts = 0;
		//Fragment_Map_Reference->Fragments_Sent_Server_Confirmed.Empty();
	}
	else
	{
		if ((Fragment_Map_Reference->Server_Confrim_Received_Num) >= (Fragment_Map_Reference->Expected_Total_Fragments))
		{
			Fragment_Map_Reference->Sent_Fragment_Num = 0;
			Fragment_Map_Reference->Server_Confrim_Received_Num = 0;
			Fragment_Map_Reference->Current_Retry_Attempts = 0;
		}
	}

	int Width = 0;
	int Height = 0;

	int Total_Fragments = 0;

	bool True_For_RGBA_False_For_RGB = false;
	uint8* Buffer = nullptr;

	Screen_Capture_Contex->Image_From_Screen_Capture_Pointer(IsSuccessful, Width, Height, &Buffer, True_For_RGBA_False_For_RGB);
	if (!IsSuccessful)
	{
		return;
	}

	TArray<uint8> JPG_Buffer;

	int To_Width = 0;
	int To_Height = 0;
	if ((Resize_Width <= 0) || (Resize_Height <= 0))
	{
		To_Width = Width;
		To_Height = Height;
	}
	else
	{
		To_Width = Resize_Width;
		To_Height = Resize_Height;
	}
	if (True_For_RGBA_False_For_RGB)
	{
		UWrexa_Blueprint_Functions::RGBA_Nearest_Neighbour_Resize_Reference(Width, Height, Buffer, JPG_Buffer, To_Width, To_Height, true);
		UWrexa_Blueprint_Functions::RGBA_To_JPGE_Buffer_Reference(To_Width, To_Height, JPG_Buffer.GetData(), JPG_Buffer, Quality_Factor_Percent, true, IsSuccessful);
	}
	else
	{
		//Buffer = UWrexa_Blueprint_Functions::RGB_Bilinear_Resize(Width, Height, Buffer, Resize_Width, Resize_Height);
		//JPG_Buffer = UWrexa_Blueprint_Functions::RGB_To_JPGE_Buffer(Resize_Width, Resize_Height, Buffer, Quality_Factor_Percent, true, IsSuccessful);
		return;
	}

	if (!IsSuccessful)
	{
		return;
	}

	if ((Fragment_Map_Reference->Sent_Fragment_Num) > (Fragment_Map_Reference->Server_Confrim_Received_Num))
	{
		if (Fragment_Map_Reference->Current_Retry_Attempts > Fragment_Map_Reference->Max_Retry_Blocks)
		{
			Fragment_Map_Reference->Current_Retry_Attempts = 0;
			Fragment_Map_Reference->Sent_Fragment_Num = Fragment_Map_Reference->Server_Confrim_Received_Num;
		}
		else
		{
			Fragment_Map_Reference->Current_Retry_Attempts += 1;
			UTexture2D* Returned_Texture = UWrexa_Blueprint_Functions::TextureFrom_JPGE_Image(
				false,
				JPG_Buffer);

			if (Returned_Texture == nullptr)
			{
				return;
			}

			Key->Set_New_Texture(Returned_Texture);
			Share_Image_Helper_Stage_1();
			return;
		}
	}

	//Fragment_Map_Reference->Resolution_Width = Width;
	//Fragment_Map_Reference->Resolution_Height = Height;

	IsSuccessful = UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_JPGE_By_Pointer(JPG_Buffer, Max_Array_Size, Total_Fragments, Fragment_Map_Reference);

	if (IsSuccessful)
	{
		Fragment_Map_Reference->JPEG_Buffer_Size = JPG_Buffer.Num();
		//Key->On_RGB_Fragment_Received_Pointer(Fragment_Map_Reference);
		Key->On_JPG_Fragment_Received(Sending_Shared_Image_JPGE.FindRef(Key));
		Share_Image_Helper_Stage_1();
	}
}

void AWrexa_Share_Player_Controller::Share_Image_From_File(UWrexa_Image_Share_Component* Key, const FString& File_Path_With_File_Name_And_Wihout_Extension, EJoyImageFormats ImageFormat, const int& Max_Array_Size, const uint8& Quality_Factor_Percent, int Resize_Width, int Resize_Height, EResize_Alogrithm_Type Resize_Algorithm_Type)
{
	bool IsSuccessful = false;
	TArray<uint8> Raw_JPEG_Buffer;
	UWrexa_Blueprint_Functions::LoadRawJPGBufferFromFile(
		File_Path_With_File_Name_And_Wihout_Extension,
		ImageFormat,
		Raw_JPEG_Buffer,
		IsSuccessful,
		Resize_Width,
		Resize_Height,
		Resize_Algorithm_Type,
		false,
		Quality_Factor_Percent);

	if (!IsSuccessful)
	{
		return;
	}

	Share_JPG_Image(Key, Raw_JPEG_Buffer, Max_Array_Size, 0);
	return;
}

void AWrexa_Share_Player_Controller::Share_Image_From_Downloaded_File(UWrexa_Image_Share_Component* Key, const UWrexa_WebFileDownloader* Downloaded_Context, EJoyImageFormats ImageFormat, const int& Max_Array_Size, const uint8& Quality_Factor_Percent, int Resize_Width, int Resize_Height, EResize_Alogrithm_Type Resize_Algorithm_Type)
{
	bool IsSuccessful = false;
	TArray<uint8> Raw_JPEG_Buffer;
	UWrexa_Blueprint_Functions::LoadRawJPGBufferFromDownloaded_File(
		Downloaded_Context,
		ImageFormat,
		Raw_JPEG_Buffer,
		IsSuccessful,
		Resize_Width,
		Resize_Height,
		Resize_Algorithm_Type,
		false,
		Quality_Factor_Percent);

	if (!IsSuccessful)
	{
		return;
	}

	Share_JPG_Image(Key, Raw_JPEG_Buffer, Max_Array_Size, 0);
	return;
}

void AWrexa_Share_Player_Controller::Share_Image_JPG_To_Server_Implementation(UWrexa_Image_Share_Component* Target_Image_Share_Component,
	const int& Expected_Total_Fragments,
	const int& Expected_Each_Fragment_Size,
	const int& JPEG_Buffer_Size,
	const int& Received_Fragment_Index,
	const TArray<uint8>& Received_Bytes)
{
	if (Target_Image_Share_Component == nullptr)
	{
		return;
	}

	if (!IsValid(Target_Image_Share_Component))
	{
		return;
	}

	if (Target_Image_Share_Component->Owning_Controller != this)
	{
		return;
	}

	if ((JPEG_Buffer_Size) >
		(Expected_Each_Fragment_Size * Expected_Total_Fragments))
	{
		return;
	}

	if ((Received_Fragment_Index >= Expected_Total_Fragments) || (Received_Fragment_Index < 0))
	{
		return;
	}

	if (Received_Bytes.Num() > Expected_Each_Fragment_Size)
	{
		return;
	}

	FJPEG_Fragment_Struct* Fragment_Map_Reference = Received_Shared_Image_JPGE.Find(Target_Image_Share_Component);

	if (!(Received_Shared_Image_JPGE.Contains(Target_Image_Share_Component)))
	{
		Fragment_Map_Reference = &(Received_Shared_Image_JPGE.Add(Target_Image_Share_Component));

		Fragment_Map_Reference->Expected_Total_Fragments = Expected_Total_Fragments;
		Fragment_Map_Reference->Expected_Each_Fragment_Size = Expected_Each_Fragment_Size;
		Fragment_Map_Reference->JPEG_Buffer_Size = JPEG_Buffer_Size;
		Fragment_Map_Reference->Sent_Fragment_Num = 0;
	}
	else
	{
		if ((Fragment_Map_Reference->Expected_Total_Fragments != Expected_Total_Fragments) ||
			(Fragment_Map_Reference->Expected_Each_Fragment_Size != Expected_Each_Fragment_Size) ||
			(Fragment_Map_Reference->JPEG_Buffer_Size != JPEG_Buffer_Size))
		{
			Fragment_Map_Reference->Expected_Total_Fragments = Expected_Total_Fragments;
			Fragment_Map_Reference->Expected_Each_Fragment_Size = Expected_Each_Fragment_Size;
			Fragment_Map_Reference->JPEG_Buffer_Size = JPEG_Buffer_Size;
			Fragment_Map_Reference->Sent_Fragment_Num = 0;
		}
	}

	if (Fragment_Map_Reference->JPEG_Buffer.Num() != JPEG_Buffer_Size)
	{
		Fragment_Map_Reference->JPEG_Buffer.Init(0, JPEG_Buffer_Size);//Colour Black
		//Fragment_Map_Reference.JPEG_Buffer.Reserve(JPEG_Buffer_Size);
	}

	int Start = Received_Fragment_Index * Expected_Each_Fragment_Size;

	if ((Start + Received_Bytes.Num()) > (int)JPEG_Buffer_Size)
	{
		return;
	}

	//int End = Start + Expected_Each_Fragment_Size;

	for (int i = 0; i < Received_Bytes.Num(); ++i)
	{
		Fragment_Map_Reference->JPEG_Buffer[(Start + i)] = Received_Bytes[i];
	}

	APlayerController* PlayerController = nullptr;
	AWrexa_Share_Player_Controller* The_Wrexa_Player_Controller;
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter) 
	{
		PlayerController = iter->Get();	
		if((PlayerController != nullptr) && (PlayerController != this))
		{
			The_Wrexa_Player_Controller = Cast<AWrexa_Share_Player_Controller>(PlayerController);
			if (The_Wrexa_Player_Controller != nullptr)
			{
				The_Wrexa_Player_Controller->Share_Image_JPG_To_Client(
					Target_Image_Share_Component,
					Expected_Total_Fragments,
					Expected_Each_Fragment_Size,
					JPEG_Buffer_Size,
					Received_Fragment_Index,
					Received_Bytes);
			}
		}
	}

	//FTimerHandle TimerHandle;
	//GetGameInstance()->GetTimerManager().SetTimer(TimerHandle, [&, this, Target_Image_Share_Component, Received_Fragment_Index]()
	//	{
	//		this->Share_Image_JPG_Receive_Confirmation_To_Client(Target_Image_Share_Component, Received_Fragment_Index);
	//	}, Next_JPG_Server_Request_To_Client_Delay_In_Seconds, false);

	Share_Image_JPG_Receive_Confirmation_To_Client(Target_Image_Share_Component, Received_Fragment_Index);
}

void AWrexa_Share_Player_Controller::Share_Image_JPG_Receive_Confirmation_To_Client_Implementation(UWrexa_Image_Share_Component* Target_Image_Share_Component, const int& Received_Index)
{
	if (Target_Image_Share_Component == nullptr)
	{
		return;
	}

	if (!IsValid(Target_Image_Share_Component))
	{
		return;
	}

	bool IsSuccessful = false;
	FJPEG_Fragment_Struct* Fragment_Map_Reference = Sending_Shared_Image_JPGE.Find(Target_Image_Share_Component);

	if (Fragment_Map_Reference == nullptr)
	{
		return;
	}

	Fragment_Map_Reference->Server_Confrim_Received_Num = Received_Index + 1;

	Share_Image_Helper_Stage_1();
}

void AWrexa_Share_Player_Controller::Share_Image_JPG_To_Client_Implementation(UWrexa_Image_Share_Component* Target_Image_Share_Component,
	const int& Expected_Total_Fragments,
	const int& Expected_Each_Fragment_Size,
	const int& JPEG_Buffer_Size,
	const int& Received_Fragment_Index,
	const TArray<uint8>& Received_Bytes)
{
	if (Target_Image_Share_Component == nullptr)
	{
		return;
	}

	if (!IsValid(Target_Image_Share_Component))
	{
		return;
	}

	if ((JPEG_Buffer_Size) >
		(Expected_Each_Fragment_Size * Expected_Total_Fragments))
	{
		return;
	}

	if ((Received_Fragment_Index >= Expected_Total_Fragments) || (Received_Fragment_Index < 0))
	{
		return;
	}

	if (Received_Bytes.Num() > Expected_Each_Fragment_Size)
	{
		return;
	}

	FJPEG_Fragment_Struct* Fragment_Map_Reference = Received_Shared_Image_JPGE.Find(Target_Image_Share_Component);

	if (!(Received_Shared_Image_JPGE.Contains(Target_Image_Share_Component)))
	{
		Fragment_Map_Reference = &(Received_Shared_Image_JPGE.Add(Target_Image_Share_Component));

		Fragment_Map_Reference->Expected_Total_Fragments = Expected_Total_Fragments;
		Fragment_Map_Reference->Expected_Each_Fragment_Size = Expected_Each_Fragment_Size;
		Fragment_Map_Reference->JPEG_Buffer_Size = JPEG_Buffer_Size;
		Fragment_Map_Reference->Sent_Fragment_Num = 0;
	}
	else
	{
		if ((Fragment_Map_Reference->Expected_Total_Fragments != Expected_Total_Fragments) ||
			(Fragment_Map_Reference->Expected_Each_Fragment_Size != Expected_Each_Fragment_Size) ||
			(Fragment_Map_Reference->JPEG_Buffer_Size != JPEG_Buffer_Size))
		{
			Fragment_Map_Reference->Expected_Total_Fragments = Expected_Total_Fragments;
			Fragment_Map_Reference->Expected_Each_Fragment_Size = Expected_Each_Fragment_Size;
			Fragment_Map_Reference->JPEG_Buffer_Size = JPEG_Buffer_Size;
			Fragment_Map_Reference->Sent_Fragment_Num = 0;
		}
	}
	

	if (Fragment_Map_Reference->JPEG_Buffer.Num() != JPEG_Buffer_Size)
	{
		Fragment_Map_Reference->JPEG_Buffer.Init(0, JPEG_Buffer_Size);//Colour Black
		//Fragment_Map_Reference.JPEG_Buffer.Reserve(JPEG_Buffer_Size);
	}

	int Start = Received_Fragment_Index * Expected_Each_Fragment_Size;

	if ((Start + Received_Bytes.Num()) > (int)JPEG_Buffer_Size)
	{
		return;
	}

	//int End = Start + Expected_Each_Fragment_Size;

	for (int i = 0; i < Received_Bytes.Num(); ++i)
	{
		Fragment_Map_Reference->JPEG_Buffer[(Start + i)] = Received_Bytes[i];
	}
	
	if (Fragment_Map_Reference->Sent_Fragment_Num != Received_Fragment_Index)
	{
		Fragment_Map_Reference->Sent_Fragment_Num = 0;
	}
	else
	{
		Fragment_Map_Reference->Sent_Fragment_Num += 1;// TODO WARNING ERROR Bad, But Works For Now

		if (Fragment_Map_Reference->Sent_Fragment_Num == Fragment_Map_Reference->Expected_Total_Fragments)
		{
			Fragment_Map_Reference->Sent_Fragment_Num = 0;
			Target_Image_Share_Component->On_JPG_Fragment_Received(*Fragment_Map_Reference);
		}
	}	
}

void AWrexa_Share_Player_Controller::Share_RGB_Image(UWrexa_Image_Share_Component* Key, const TArray<uint8>& Buffer, const int& Max_Array_Size, const int& Offset_Order, const bool& Is_RGBA,
	const int& Width, const int& Height)
{
	if (Key == nullptr)
	{
		return;
	}

	bool IsSuccessful = false;
	FRGB_Fragment_Map* Fragment_Map_Reference = Sending_Shared_Image_RGB.Find(Key);

	if (Fragment_Map_Reference == nullptr)
	{
		Fragment_Map_Reference = &(Sending_Shared_Image_RGB.Add(Key));
		Fragment_Map_Reference->Offset_Index = 0;
		Fragment_Map_Reference->Offset_Order = Offset_Order;
	}
	Fragment_Map_Reference->Resolution_Width = Width;
	Fragment_Map_Reference->Resolution_Height = Height;

	int Total_Fragments = 0;
	if (Is_RGBA)
	{
		IsSuccessful = UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_RGBA_By_Pointer(Buffer, Max_Array_Size, Total_Fragments, Fragment_Map_Reference);
	}
	else
	{
		IsSuccessful = UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_RGB_By_Pointer(Buffer, Max_Array_Size, Total_Fragments, Fragment_Map_Reference);
	}

	if (IsSuccessful)
	{

		//Key->On_RGB_Fragment_Received_Pointer(Fragment_Map_Reference);
		Key->On_RGB_Fragment_Received(Sending_Shared_Image_RGB.FindRef(Key));
		Share_Image_Helper_Stage_1();
	}
}

//#pragma optimize("", off)
void AWrexa_Share_Player_Controller::Share_RGB_Image_Screen_Capture_Component(UWrexa_Image_Share_Component* Key, UWrexa_Screen_Capture* Screen_Capture_Contex, const int& Max_Array_Size, const int& Offset_Order)
{
	if (Key == nullptr)
	{
		return;
	}

	if (Screen_Capture_Contex == nullptr)
	{
		return;
	}

	bool IsSuccessful = false;
	FRGB_Fragment_Map* Fragment_Map_Reference = Sending_Shared_Image_RGB.Find(Key);

	if (Fragment_Map_Reference == nullptr)
	{
		Fragment_Map_Reference = &(Sending_Shared_Image_RGB.Add(Key));
		Fragment_Map_Reference->Offset_Index = 0;
		Fragment_Map_Reference->Offset_Order = Offset_Order;
	}

	int Width = 0;
	int Height = 0;
	

	int Total_Fragments = 0;
	//if (Is_RGBA)
	//{
	//	TArray<uint8> RGBA_Buffer;
	//	Screen_Capture_Contex->RGBA_Image_From_Screen_Capture(IsSuccessful, Width, Height, RGBA_Buffer);
	//	if (!IsSuccessful)
	//	{
	//		return;
	//	
	//	}
	//	Fragment_Map_Reference->Resolution_Width = Width;
	//	Fragment_Map_Reference->Resolution_Height = Height;
	//	IsSuccessful = UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_RGBA_By_Pointer(RGBA_Buffer, Max_Array_Size, Total_Fragments, Fragment_Map_Reference);
	//}
	//else
	//{
	bool True_For_RGBA_False_For_RGB = false;
	TArray<uint8> Buffer;

	Screen_Capture_Contex->Image_From_Screen_Capture(IsSuccessful, Width, Height, Buffer, True_For_RGBA_False_For_RGB);
	if (!IsSuccessful)
	{
		return;
	}

	if (True_For_RGBA_False_For_RGB)
	{
		Buffer = UWrexa_Blueprint_Functions::RGBA_To_RGB_Buffer(Width, Height, Buffer);
	}

	Fragment_Map_Reference->Resolution_Width = Width;
	Fragment_Map_Reference->Resolution_Height = Height;
	IsSuccessful = UWrexa_Blueprint_Functions::Make_Fragmented_Data_From_RGB_By_Pointer(Buffer, Max_Array_Size, Total_Fragments, Fragment_Map_Reference);
	//}

	if (IsSuccessful)
	{
		//Key->On_RGB_Fragment_Received_Pointer(Fragment_Map_Reference);
		Key->On_RGB_Fragment_Received(Sending_Shared_Image_RGB.FindRef(Key));
		Share_Image_Helper_Stage_1();
	}
}

void AWrexa_Share_Player_Controller::Share_RGB_Image_Bilinear_Resize(UWrexa_Image_Share_Component* Key, TArray<uint8>& Buffer, const int& Max_Array_Size, const int& Offset_Order, const bool& Is_RGBA,
	const int& Original_Width, const int& Original_Height, const int& Resize_Width, const int& Resize_Height)
{
	if (Key == nullptr)
	{
		return;
	}

	if (Is_RGBA)
	{
		Buffer = UWrexa_Blueprint_Functions::RGBA_To_RGB_Buffer(Original_Width, Original_Height, Buffer);
	}
	Buffer = UWrexa_Blueprint_Functions::RGB_Bilinear_Resize(Original_Width, Original_Height, Buffer, Resize_Width, Resize_Height);	
	
	Share_RGB_Image(Key, Buffer, Max_Array_Size, Offset_Order, false, Resize_Width, Resize_Height);
}

void AWrexa_Share_Player_Controller::Share_RGB_Image_Screen_Capture_Component_Bilinear_Resize(UWrexa_Image_Share_Component* Key, UWrexa_Screen_Capture* Screen_Capture_Contex, const int& Max_Array_Size, const int& Offset_Order,
	const int& Resize_Width, const int& Resize_Height)
{
	if (Key == nullptr)
	{
		return;
	}

	if (Screen_Capture_Contex == nullptr)
	{
		return;
	}

	bool IsSuccessful = false;

	bool True_For_RGBA_False_For_RGB = false;	
	TArray<uint8> Buffer;

	int Original_Width = 0;
	int Original_Height = 0;

	Screen_Capture_Contex->Image_From_Screen_Capture(IsSuccessful, Original_Width, Original_Height, Buffer, True_For_RGBA_False_For_RGB);
	if (!IsSuccessful)
	{
		return;
	}

	if (True_For_RGBA_False_For_RGB)
	{
		Buffer = UWrexa_Blueprint_Functions::RGBA_To_RGB_Buffer(Original_Width, Original_Height, Buffer);
	}

	Buffer = UWrexa_Blueprint_Functions::RGB_Bilinear_Resize(Original_Width, Original_Height, Buffer, Resize_Width, Resize_Height);

	Share_RGB_Image(Key, Buffer, Max_Array_Size, Offset_Order, false, Resize_Width, Resize_Height);
}

void AWrexa_Share_Player_Controller::Share_RGB_Image_Nearest_Neighbour_Resize(UWrexa_Image_Share_Component* Key, TArray<uint8>& Buffer, const int& Max_Array_Size, const int& Offset_Order, const bool& Is_RGBA,
	const int& Original_Width, const int& Original_Height, const int& Resize_Width, const int& Resize_Height)
{
	if (Key == nullptr)
	{
		return;
	}

	if (Is_RGBA)
	{
		Buffer = UWrexa_Blueprint_Functions::RGBA_Nearest_Neighbour_Resize(Original_Width, Original_Height, Buffer, Resize_Width, Resize_Height, true);
	}
	else
	{
		Buffer = UWrexa_Blueprint_Functions::RGB_Nearest_Neighbour_Resize(Original_Width, Original_Height, Buffer, Resize_Width, Resize_Height);
	}

	Share_RGB_Image(Key, Buffer, Max_Array_Size, Offset_Order, false, Resize_Width, Resize_Height);
}

void AWrexa_Share_Player_Controller::Share_RGB_Image_Screen_Capture_Component_Nearest_Neighbour_Resize(UWrexa_Image_Share_Component* Key, UWrexa_Screen_Capture* Screen_Capture_Contex, const int& Max_Array_Size, const int& Offset_Order,
	const int& Resize_Width, const int& Resize_Height)
{
	if (Key == nullptr)
	{
		return;
	}

	if (Screen_Capture_Contex == nullptr)
	{
		return;
	}

	bool IsSuccessful = false;

	bool True_For_RGBA_False_For_RGB = false;
	TArray<uint8> Buffer;

	int Original_Width = 0;
	int Original_Height = 0;

	Screen_Capture_Contex->Image_From_Screen_Capture(IsSuccessful, Original_Width, Original_Height, Buffer, True_For_RGBA_False_For_RGB);
	if (!IsSuccessful)
	{
		return;
	}

	if (True_For_RGBA_False_For_RGB)
	{
		Buffer = UWrexa_Blueprint_Functions::RGBA_To_RGB_Buffer(Original_Width, Original_Height, Buffer);
	}

	Buffer = UWrexa_Blueprint_Functions::RGB_Nearest_Neighbour_Resize(Original_Width, Original_Height, Buffer, Resize_Width, Resize_Height);

	Share_RGB_Image(Key, Buffer, Max_Array_Size, Offset_Order, false, Resize_Width, Resize_Height);
}

void AWrexa_Share_Player_Controller::Share_Image_RGB_To_Server_Implementation(UWrexa_Image_Share_Component* Target_Image_Share_Component,
	const int& Expected_Total_Fragments,
	const int& Expected_Each_Fragment_Size,
	const int& Resolution_Width,
	const int& Resolution_Height,
	const int& Received_Fragment_Index,
	const ERGB_Colour_Channel& Received_Colour_Channel,
	const TArray<uint8>& Received_Bytes)
{
	if (Target_Image_Share_Component == nullptr)
	{
		return;
	}

	if (!IsValid(Target_Image_Share_Component))
	{
		return;
	}

	if (Target_Image_Share_Component->Owning_Controller != this)
	{
		return;
	}

	if ((Resolution_Width * Resolution_Height) >
		(Expected_Each_Fragment_Size * Expected_Total_Fragments))
	{
		return;
	}

	if ((Received_Fragment_Index >= Expected_Total_Fragments) || (Received_Fragment_Index < 0))
	{
		return;
	}

	if (Received_Bytes.Num() > Expected_Each_Fragment_Size)
	{
		return;
	}

	FRGB_Fragment_Map *Fragment_Map_Reference = Received_Shared_Image_RGB.Find(Target_Image_Share_Component);

	if (!(Received_Shared_Image_RGB.Contains(Target_Image_Share_Component)))
	{
		Fragment_Map_Reference = &(Received_Shared_Image_RGB.Add(Target_Image_Share_Component));
	}

	Fragment_Map_Reference->Expected_Total_Fragments = Expected_Total_Fragments;
	Fragment_Map_Reference->Expected_Each_Fragment_Size = Expected_Each_Fragment_Size;
	Fragment_Map_Reference->Resolution_Width = Resolution_Width;
	Fragment_Map_Reference->Resolution_Height = Resolution_Height;

	int Total_Size = Resolution_Width * Resolution_Height;

	if (Fragment_Map_Reference->RED_Channel.Num() != Total_Size)
	{
		Fragment_Map_Reference->RED_Channel.Init(0, Total_Size);//Colour Black
		//Fragment_Map_Reference.RED_Channel.Reserve(Total_Size);
	}
	if (Fragment_Map_Reference->GREEN_Channel.Num() != Total_Size)
	{
		Fragment_Map_Reference->GREEN_Channel.Init(0, Total_Size);//Colour Black
		//Fragment_Map_Reference.GREEN_Channel.Reserve(Total_Size);
	}
	if (Fragment_Map_Reference->BLUE_Channel.Num() != Total_Size)
	{
		Fragment_Map_Reference->BLUE_Channel.Init(0, Total_Size);//Colour Black
		//Fragment_Map_Reference.BLUE_Channel.Reserve(Total_Size);
	}

	int Start = Received_Fragment_Index * Expected_Each_Fragment_Size;

	if ((Start + Received_Bytes.Num()) > Total_Size)
	{
		return;
	}
	//int End = Start + Expected_Each_Fragment_Size;

	switch (Received_Colour_Channel)
	{
	case ERGB_Colour_Channel::RED:
	{
		for (int i = 0; i < Received_Bytes.Num(); ++i)
		{
			Fragment_Map_Reference->RED_Channel[(Start + i)] = Received_Bytes[i];
		}
		break;
	}

	case ERGB_Colour_Channel::GREEN:
	{
		for (int i = 0; i < Received_Bytes.Num(); ++i)
		{
			Fragment_Map_Reference->GREEN_Channel[(Start + i)] = Received_Bytes[i];
		}
		break;
	}

	case ERGB_Colour_Channel::BLUE:
	{
		for (int i = 0; i < Received_Bytes.Num(); ++i)
		{
			Fragment_Map_Reference->BLUE_Channel[(Start + i)] = Received_Bytes[i];
		}
		break;
	}

	default:
	{
		break;
	}
	}

	APlayerController* PlayerController = nullptr;
	AWrexa_Share_Player_Controller* The_Wrexa_Player_Controller;
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
	{
		PlayerController = iter->Get();
		if ((PlayerController != nullptr) && (PlayerController != this))
		{
			The_Wrexa_Player_Controller = Cast<AWrexa_Share_Player_Controller>(PlayerController);
			if (The_Wrexa_Player_Controller != nullptr)
			{
				The_Wrexa_Player_Controller->Share_Image_RGB_To_Client(
											Target_Image_Share_Component,
											Expected_Total_Fragments,
											Expected_Each_Fragment_Size,
											Resolution_Width,
											Resolution_Height,
											Received_Fragment_Index,
											Received_Colour_Channel,
											Received_Bytes);
			}
		}		
	}
}

void AWrexa_Share_Player_Controller::Share_Image_RGB_To_Client_Implementation(UWrexa_Image_Share_Component* Target_Image_Share_Component,
	const int& Expected_Total_Fragments,
	const int& Expected_Each_Fragment_Size,
	const int& Resolution_Width,
	const int& Resolution_Height,
	const int& Received_Fragment_Index,
	const ERGB_Colour_Channel& Received_Colour_Channel,
	const TArray<uint8>& Received_Bytes)
{	
	if (Target_Image_Share_Component == nullptr)
	{
		return;
	}

	if (!IsValid(Target_Image_Share_Component))
	{
		return;
	}

	if ((Resolution_Width * Resolution_Height) >
		(Expected_Each_Fragment_Size * Expected_Total_Fragments))
	{
		return;
	}

	if ((Received_Fragment_Index >= Expected_Total_Fragments) || (Received_Fragment_Index < 0))
	{
		return;
	}

	if (Received_Bytes.Num() > Expected_Each_Fragment_Size)
	{
		return;
	}
	
	FRGB_Fragment_Map* Fragment_Map_Reference = Received_Shared_Image_RGB.Find(Target_Image_Share_Component);

	if (!(Received_Shared_Image_RGB.Contains(Target_Image_Share_Component)))
	{
		Fragment_Map_Reference = &(Received_Shared_Image_RGB.Add(Target_Image_Share_Component));
	}

	Fragment_Map_Reference->Expected_Total_Fragments = Expected_Total_Fragments;
	Fragment_Map_Reference->Expected_Each_Fragment_Size = Expected_Each_Fragment_Size;
	Fragment_Map_Reference->Resolution_Width = Resolution_Width;
	Fragment_Map_Reference->Resolution_Height = Resolution_Height;
	
	int Total_Size = Resolution_Width * Resolution_Height;

	if (Fragment_Map_Reference->RED_Channel.Num() != Total_Size)
	{
		Fragment_Map_Reference->RED_Channel.Init(0, Total_Size);//Colour Black
		//Fragment_Map_Reference.RED_Channel.Reserve(Total_Size);
	}
	if (Fragment_Map_Reference->GREEN_Channel.Num() != Total_Size)
	{
		Fragment_Map_Reference->GREEN_Channel.Init(0, Total_Size);//Colour Black
		//Fragment_Map_Reference.GREEN_Channel.Reserve(Total_Size);
	}
	if (Fragment_Map_Reference->BLUE_Channel.Num() != Total_Size)
	{
		Fragment_Map_Reference->BLUE_Channel.Init(0, Total_Size);//Colour Black
		//Fragment_Map_Reference.BLUE_Channel.Reserve(Total_Size);
	}

	int Start = Received_Fragment_Index * Expected_Each_Fragment_Size;

	if ((Start + Received_Bytes.Num()) > Total_Size)
	{
		return;
	}
	//int End = Start + Expected_Each_Fragment_Size;

	switch (Received_Colour_Channel)
	{
		case ERGB_Colour_Channel::RED:
		{
			for (int i = 0; i < Received_Bytes.Num(); ++i)
			{
				Fragment_Map_Reference->RED_Channel[(Start + i)] = Received_Bytes[i];
			}
			break;
		}

		case ERGB_Colour_Channel::GREEN:
		{
			for (int i = 0; i < Received_Bytes.Num(); ++i)
			{
				Fragment_Map_Reference->GREEN_Channel[(Start + i)] = Received_Bytes[i];
			}
			break;
		}

		case ERGB_Colour_Channel::BLUE:
		{
			for (int i = 0; i < Received_Bytes.Num(); ++i)
			{
				Fragment_Map_Reference->BLUE_Channel[(Start + i)] = Received_Bytes[i];
			}
			break;
		}

		default:
		{
			break;
		}
	}	

	Target_Image_Share_Component->On_RGB_Fragment_Received(*Fragment_Map_Reference);
}