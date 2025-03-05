// Developed By Wrexa Technologies 2023

#pragma once

#include "CoreMinimal.h"
#include "Wrexa_Share_Component.h"
#include "Wrexa_Image_Share_Component.generated.h"

struct FRGB_Fragment_Map;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WREXA_API UWrexa_Image_Share_Component : public UWrexa_Share_Component
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWrexa_Image_Share_Component();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UTexture2D* Constructed_Texture = nullptr;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void On_RGB_Fragment_Received(const FRGB_Fragment_Map& Received_Fragments_Map);

	void On_RGB_Fragment_Received_Pointer(FRGB_Fragment_Map* Received_Fragments_Map);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void On_JPG_Fragment_Received(const FJPEG_Fragment_Struct& Received_Fragments_Map);

	void On_JPG_Fragment_Received_Pointer(FJPEG_Fragment_Struct* Received_Fragments_Map);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void Set_New_Texture(UTexture2D* New_Texture);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wrexa | Share_Component | Image")
		void On_New_Texture_Received(UTexture2D* Received_Texture);

	void Set_Texture_For_Mirrors();
};
