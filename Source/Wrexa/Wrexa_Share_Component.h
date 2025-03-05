// Developed By Wrexa Technologies 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Controller.h"
#include "Wrexa_Share_Component.generated.h"

// THIS SHOULD NOT BE REPLICATED
// ALL REPLICATION SHOULD HAPPEN ONLY on PlayerController
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WREXA_API UWrexa_Share_Component : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWrexa_Share_Component();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wrexa | Share_Component")
		bool Has_To_Be_Same_Owner = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wrexa | Share_Component")
		bool Currently_Sharing = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wrexa | Share_Component")
		AController* Owning_Controller = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wrexa | Share_Component")
		bool Is_Currently_Mirroring_From = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wrexa | Share_Component")
		UWrexa_Share_Component* Currently_Mirrored_From = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Share_Component")
		TArray<UWrexa_Share_Component*> Mirroring_To_Components;

public:
		bool Remove_Owner(AController* Removed_Controller);
		void Remove_Mirror_From(UWrexa_Share_Component* New_Mirror_From);
		
		bool Set_New_Owner(AController* New_Controller, bool Override_Previous);
		bool Set_New_Mirror_From(UWrexa_Share_Component* New_Mirror_From, bool Override_Previous);

		bool Stop_Mirror(AController* Requesting_Controller, bool Override_Previous);

		UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wrexa | Share_Component")
			void On_Value_Change();
};
