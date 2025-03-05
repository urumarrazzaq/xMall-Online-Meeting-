// Developed By Wrexa Technologies 2023

#include "Wrexa_Share_Component.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UWrexa_Share_Component::UWrexa_Share_Component()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWrexa_Share_Component::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UWrexa_Share_Component::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UWrexa_Share_Component::Remove_Owner(AController* Removed_Controller)
{
	if (Removed_Controller == Owning_Controller)
	{
		Owning_Controller = nullptr;
		Currently_Sharing = false;

		for (UWrexa_Share_Component* Each_Share_Component : Mirroring_To_Components)
		{
			if (Each_Share_Component != nullptr)
			{
				Each_Share_Component->Mirroring_To_Components.Remove(Each_Share_Component);
				Each_Share_Component->Currently_Mirrored_From = nullptr;
				Each_Share_Component->Is_Currently_Mirroring_From = false;
			}
		}
		Remove_Mirror_From(Currently_Mirrored_From);
		return true;
	}
	Remove_Mirror_From(Currently_Mirrored_From);
	return false;
}

void UWrexa_Share_Component::Remove_Mirror_From(UWrexa_Share_Component* New_Mirror_From)
{
	if (New_Mirror_From == Currently_Mirrored_From)
	{
		if (Currently_Mirrored_From != nullptr)
		{
			Currently_Mirrored_From->Mirroring_To_Components.Remove(this);
			Currently_Mirrored_From = nullptr;
			Is_Currently_Mirroring_From = false;
		}
	}
}

bool UWrexa_Share_Component::Set_New_Owner(AController* New_Controller, bool Override_Previous)
{
	if (New_Controller == nullptr)
	{
		return false;
	}

	if (!(IsValid(New_Controller)))
	{
		return false;
	}

	if (Has_To_Be_Same_Owner)
	{		
		ACharacter *Owning_Character = Cast<ACharacter>(GetOwner());
		if (Owning_Character == nullptr)
		{
			return false;
		}

		if (New_Controller != Owning_Character->GetController())
		{
			return false;
		}
	}

	if (!Override_Previous)
	{
		if (Owning_Controller == New_Controller)
		{
			return true;
		}

		if (Owning_Controller != nullptr)
		{
			return false;
		}

		if (Currently_Mirrored_From != nullptr)
		{
			return false;
		}
	}
	else
	{		
		Remove_Owner(Owning_Controller);
	}

	Remove_Mirror_From(Currently_Mirrored_From);

	Owning_Controller = New_Controller;
	Currently_Sharing = true;

	On_Value_Change();
	return true;
}

bool UWrexa_Share_Component::Set_New_Mirror_From(UWrexa_Share_Component* New_Mirror_From, bool Override_Previous)
{
	if (New_Mirror_From == nullptr)
	{
		return false;
	}

	if (!(IsValid(New_Mirror_From)))
	{
		return false;
	}

	if (!Override_Previous)
	{
		if (Currently_Mirrored_From == New_Mirror_From)
		{
			return true;
		}

		if (Owning_Controller == nullptr)
		{
			return false;
		}

		//if (Currently_Mirrored_From != nullptr)
		//{
		//	return false;
		//}
	}
	else
	{		
		Remove_Owner(Owning_Controller);
	}

	Remove_Mirror_From(Currently_Mirrored_From);

	Currently_Mirrored_From = New_Mirror_From;
	Currently_Mirrored_From->Mirroring_To_Components.AddUnique(this);
	Is_Currently_Mirroring_From = true;
	
	Currently_Mirrored_From->On_Value_Change();
	return true;
}

bool UWrexa_Share_Component::Stop_Mirror(AController* Requesting_Controller, bool Override_Previous)
{
	if (!Override_Previous)
	{

		if (Requesting_Controller == nullptr)
		{
			return false;
		}

		if (!(IsValid(Requesting_Controller)))
		{
			return false;
		}
	}

	Remove_Mirror_From(Currently_Mirrored_From);
	On_Value_Change();
	return true;
}

void UWrexa_Share_Component::On_Value_Change_Implementation()
{
	
}