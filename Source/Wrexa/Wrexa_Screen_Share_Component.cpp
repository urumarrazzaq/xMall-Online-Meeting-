// Developed By Wrexa Technologies 2023


#include "Wrexa_Screen_Share_Component.h"

// Sets default values for this component's properties
UWrexa_Screen_Share_Component::UWrexa_Screen_Share_Component()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWrexa_Screen_Share_Component::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWrexa_Screen_Share_Component::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

