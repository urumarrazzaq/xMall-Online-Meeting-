// Developed By Wrexa Technologies 2023


#include "Wrexa_Image_Share_Component.h"

#include "Wrexa_Blueprint_Functions.h"
#include "Wrexa_Share_Player_Controller.h"
//#pragma optimize("", off)
// Sets default values for this component's properties
UWrexa_Image_Share_Component::UWrexa_Image_Share_Component()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWrexa_Image_Share_Component::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UWrexa_Image_Share_Component::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWrexa_Image_Share_Component::On_RGB_Fragment_Received_Implementation(const FRGB_Fragment_Map& Received_Fragments_Map)
{
	if ((Received_Fragments_Map.Resolution_Width < 2) ||
		(Received_Fragments_Map.Resolution_Height < 2) ||
		(Received_Fragments_Map.Expected_Each_Fragment_Size < 1) ||
		(Received_Fragments_Map.Expected_Total_Fragments < 1))
	{
		return;
	}
	else
	{
		if ((Received_Fragments_Map.Resolution_Width * Received_Fragments_Map.Resolution_Height) >
			(Received_Fragments_Map.Expected_Each_Fragment_Size * Received_Fragments_Map.Expected_Total_Fragments))
		{
			return;
		}
	}

	//if ((Received_Fragments_Map.Expected_Each_Fragment_Size != Received_Fragments_Map.RED_Channel.Num()) ||
	//	(Received_Fragments_Map.Expected_Each_Fragment_Size != Received_Fragments_Map.GREEN_Channel.Num()) ||
	//	(Received_Fragments_Map.Expected_Each_Fragment_Size != Received_Fragments_Map.BLUE_Channel.Num()))
	//{
	//	return;
	//}

	UTexture2D *Returned_Texture = UWrexa_Blueprint_Functions::TextureFrom_RGB_Image(
		false,
		Received_Fragments_Map.Resolution_Width,
		Received_Fragments_Map.Resolution_Height,
		Received_Fragments_Map.RED_Channel,
		Received_Fragments_Map.GREEN_Channel,
		Received_Fragments_Map.BLUE_Channel);
	
	if (Returned_Texture == nullptr)
	{
		return;
	}

	Constructed_Texture = Returned_Texture;
	On_New_Texture_Received(Constructed_Texture);
	
	Set_Texture_For_Mirrors();
}

void UWrexa_Image_Share_Component::On_RGB_Fragment_Received_Pointer(FRGB_Fragment_Map* Received_Fragments_Map)
{
	if ((Received_Fragments_Map->Resolution_Width < 2) ||
		(Received_Fragments_Map->Resolution_Height < 2) ||
		(Received_Fragments_Map->Expected_Each_Fragment_Size < 1) ||
		(Received_Fragments_Map->Expected_Total_Fragments < 1))
	{
		return;
	}
	else
	{
		if ((Received_Fragments_Map->Resolution_Width * Received_Fragments_Map->Resolution_Height) >
			(Received_Fragments_Map->Expected_Each_Fragment_Size * Received_Fragments_Map->Expected_Total_Fragments))
		{
			return;
		}
	}

	//if ((Received_Fragments_Map.Expected_Each_Fragment_Size != Received_Fragments_Map.RED_Channel.Num()) ||
	//	(Received_Fragments_Map.Expected_Each_Fragment_Size != Received_Fragments_Map.GREEN_Channel.Num()) ||
	//	(Received_Fragments_Map.Expected_Each_Fragment_Size != Received_Fragments_Map.BLUE_Channel.Num()))
	//{
	//	return;
	//}

	UTexture2D* Returned_Texture = UWrexa_Blueprint_Functions::TextureFrom_RGB_Image(
		false,
		Received_Fragments_Map->Resolution_Width,
		Received_Fragments_Map->Resolution_Height,
		Received_Fragments_Map->RED_Channel,
		Received_Fragments_Map->GREEN_Channel,
		Received_Fragments_Map->BLUE_Channel);

	if (Returned_Texture == nullptr)
	{
		return;
	}

	Constructed_Texture = Returned_Texture;
	On_New_Texture_Received(Constructed_Texture);

	Set_Texture_For_Mirrors();
}

void UWrexa_Image_Share_Component::On_JPG_Fragment_Received_Implementation(const FJPEG_Fragment_Struct& Received_Fragments_Map)
{
	if ((Received_Fragments_Map.JPEG_Buffer_Size < 2) ||
		(Received_Fragments_Map.Expected_Each_Fragment_Size < 1) ||
		(Received_Fragments_Map.Expected_Total_Fragments < 1))
	{
		return;
	}
	else
	{
		if ((Received_Fragments_Map.JPEG_Buffer_Size) >
			(Received_Fragments_Map.Expected_Each_Fragment_Size * Received_Fragments_Map.Expected_Total_Fragments))
		{
			return;
		}
	}

	//if ((Received_Fragments_Map.Expected_Each_Fragment_Size != Received_Fragments_Map.RED_Channel.Num()) ||
	//	(Received_Fragments_Map.Expected_Each_Fragment_Size != Received_Fragments_Map.GREEN_Channel.Num()) ||
	//	(Received_Fragments_Map.Expected_Each_Fragment_Size != Received_Fragments_Map.BLUE_Channel.Num()))
	//{
	//	return;
	//}

	UTexture2D* Returned_Texture = UWrexa_Blueprint_Functions::TextureFrom_JPGE_Image(
		false,
		Received_Fragments_Map.JPEG_Buffer);

	if (Returned_Texture == nullptr)
	{
		return;
	}

	Set_New_Texture(Returned_Texture);
}

void UWrexa_Image_Share_Component::On_JPG_Fragment_Received_Pointer(FJPEG_Fragment_Struct* Received_Fragments_Map)
{
	if ((Received_Fragments_Map->JPEG_Buffer_Size < 2) ||
		(Received_Fragments_Map->Expected_Each_Fragment_Size < 1) ||
		(Received_Fragments_Map->Expected_Total_Fragments < 1))
	{
		return;
	}
	else
	{
		if ((Received_Fragments_Map->JPEG_Buffer_Size) >
			(Received_Fragments_Map->Expected_Each_Fragment_Size * Received_Fragments_Map->Expected_Total_Fragments))
		{
			return;
		}
	}

	//if ((Received_Fragments_Map.Expected_Each_Fragment_Size != Received_Fragments_Map.RED_Channel.Num()) ||
	//	(Received_Fragments_Map.Expected_Each_Fragment_Size != Received_Fragments_Map.GREEN_Channel.Num()) ||
	//	(Received_Fragments_Map.Expected_Each_Fragment_Size != Received_Fragments_Map.BLUE_Channel.Num()))
	//{
	//	return;
	//}

	UTexture2D* Returned_Texture = UWrexa_Blueprint_Functions::TextureFrom_JPGE_Image(
		false,
		Received_Fragments_Map->JPEG_Buffer);

	if (Returned_Texture == nullptr)
	{
		return;
	}

	Set_New_Texture(Returned_Texture);
}

void UWrexa_Image_Share_Component::Set_New_Texture_Implementation(UTexture2D* New_Texture)
{
	Constructed_Texture = New_Texture;
	On_New_Texture_Received(Constructed_Texture);

	Set_Texture_For_Mirrors();
}

void UWrexa_Image_Share_Component::Set_Texture_For_Mirrors()
{
	UWrexa_Image_Share_Component *Mirror_Object = nullptr;
	for (UWrexa_Share_Component* Each_Share_Component : Mirroring_To_Components)
	{
		if (Each_Share_Component != nullptr)
		{
			Mirror_Object = Cast<UWrexa_Image_Share_Component>(Each_Share_Component);
			if (Mirror_Object != nullptr)
			{
				Mirror_Object->Constructed_Texture = Constructed_Texture;
				Mirror_Object->On_New_Texture_Received(Mirror_Object->Constructed_Texture);
			}
		}
	}
}

void UWrexa_Image_Share_Component::On_New_Texture_Received_Implementation(UTexture2D* Received_Texture)
{

}