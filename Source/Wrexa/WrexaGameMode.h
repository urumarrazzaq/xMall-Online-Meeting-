// Code Done By Punal Manalan 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WrexaGameMode.generated.h"


UCLASS(minimalapi)
class AWrexaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWrexaGameMode();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PreLogin(
		const FString& Options,
		const FString& Address,
		const FUniqueNetIdRepl& UniqueId,
		FString& ErrorMessage
	) override;
	virtual APlayerController* Login
	(
		UPlayer* NewPlayer,
		ENetRole InRemoteRole,
		const FString& Portal,
		const FString& Options,
		const FUniqueNetIdRepl& UniqueId,
		FString& ErrorMessage
	) override;
	virtual void Logout(AController* Exiting) override;

	class UGameInstance* WrexaGetGameInstance(const UObject* WorldContextObject);
};



