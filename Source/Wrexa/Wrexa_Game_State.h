// Developed By Wrexa Technologies 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Wrexa_Game_State.generated.h"

/**
 * 
 */
UCLASS()
class WREXA_API AWrexa_Game_State : public AGameStateBase
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Server_Match_ID = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Server_Host_User_At_Name = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Server_Match_Room_Name = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Server_Match_Description = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Server_Game_Type = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Server_Ticket_Cost = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		TArray<FString> Users_IDs_In_Match;//User_At_Name of Users
};
