// Developed By Wrexa Technologies 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Wrexa_Share_Component.h"
#include "Wrexa_Player_Controller.generated.h"

#define Can_Execute_Server_Code ((this->GetNetMode() == ENetMode::NM_Standalone) || (this->GetNetMode() == ENetMode::NM_DedicatedServer ) || (this->GetNetMode() == ENetMode::NM_ListenServer))
#define Can_Execute_Client_Code ((this->GetNetMode() == ENetMode::NM_Standalone) ||(this->GetNetMode() == ENetMode::NM_Client) || (this->GetNetMode() == ENetMode::NM_ListenServer))

/**
 * 
 */
UCLASS()
class WREXA_API AWrexa_Player_Controller : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wrexa | Player_Info")
		FString UserAtName = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wrexa | Share_Component | Default")
		TArray<UWrexa_Share_Component*> Owned_Share_Components;

	// Executes On Serer If Connected
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Share_Component | Default")
		void Remove_All_Owned_Share_Component();

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Wrexa | Share_Component | Default")
		void Share_Component_Ownership_Request(AController* Requesting_Controller, UWrexa_Share_Component* Target_Share_Component);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Wrexa | Share_Component | Default")
		void Share_Component_Ownership_Response(AController* Requesting_Controller, UWrexa_Share_Component* Target_Share_Component, bool IsSuccessful);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wrexa | Share_Component | Default")
		void On_Share_Component_Ownership_Response(UWrexa_Share_Component* Target_Share_Component, bool IsSuccessful);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Wrexa | Share_Component | Default")
		void Share_Component_Release_Request(AController* Requesting_Controller, UWrexa_Share_Component* Target_Share_Component);
	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Wrexa | Share_Component | Default")
		void Share_Component_Release_Response(AController* Requesting_Controller, UWrexa_Share_Component* Target_Share_Component, bool IsSuccessful);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wrexa | Share_Component | Default")
		void On_Share_Component_Release_Response(UWrexa_Share_Component* Target_Share_Component, bool IsSuccessful);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Wrexa | Share_Component | Default")
		void Share_Component_Ownership_Mulitcast(UWrexa_Share_Component* Target_Share_Component, bool Owning_True_Remove_False);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wrexa | Share_Component | Default")
		void On_Share_Component_Other_Ownership(UWrexa_Share_Component* Target_Share_Component, bool Owning_True_Remove_False);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Wrexa | Share_Component | Default")
		void Share_Component_Ownership_Mirror_Request(AController* Requesting_Controller, UWrexa_Share_Component* Parent_Share_Component, UWrexa_Share_Component* Child_Share_Component);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Wrexa | Share_Component | Default")
		void Share_Component_Ownership_Mirror_Response(AController* Requesting_Controller, UWrexa_Share_Component* Parent_Share_Component, UWrexa_Share_Component* Child_Share_Component, bool IsSuccessful);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wrexa | Share_Component | Default")
		void On_Share_Component_Mirror_Response(UWrexa_Share_Component* Parent_Share_Component, UWrexa_Share_Component* Child_Share_Component, bool IsSuccessful);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Wrexa | Share_Component | Default")
		void Share_Component_Ownership_Mirror_Mulitcast(UWrexa_Share_Component* Parent_Share_Component, UWrexa_Share_Component* Child_Share_Component, bool Mirrored_True_Remove_False);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wrexa | Share_Component | Default")
		void On_Share_Component_Other_Ownership_Mirror(UWrexa_Share_Component* Parent_Share_Component, UWrexa_Share_Component* Child_Share_Component, bool Mirrored_True_Remove_False);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Wrexa | Share_Component | Default")
		void Share_Component_Mirror_Release_Request(AController* Requesting_Controller, UWrexa_Share_Component* Target_Share_Component);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wrexa | Share_Component | Default")
		void Share_Component_Mirror_Release_Response(AController* Requesting_Controller, UWrexa_Share_Component* Target_Share_Component, bool IsSuccessful);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Wrexa | Share_Component | Default")
		void Share_Component_Ownership_Mirror_Release_Mulitcast(UWrexa_Share_Component* Target_Share_Component);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wrexa | Screen Capture")
		void On_New_Screen_Frame_Received(const TArray <uint8>& RGBA_Buffer, int Width, int Height);
};
