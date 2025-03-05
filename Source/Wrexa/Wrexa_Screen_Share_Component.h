// Developed By Wrexa Technologies 2023

#pragma once

#include "CoreMinimal.h"
#include "Wrexa_Image_Share_Component.h"
#include "Wrexa_Screen_Share_Component.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WREXA_API UWrexa_Screen_Share_Component : public UWrexa_Image_Share_Component
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWrexa_Screen_Share_Component();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
