// Code Done By Punal Manalan 2022

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Wrexa_PlayerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class WREXA_API UWrexa_PlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Level | Async | Load")
		bool IsLoadingLevel = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Level | Async | Load")
		bool IsLevelLoaded = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Level | Async | Load")
		FString Name_Of_Level_Being_Loaded;

	FTimerHandle LoadAsyncTimer;

	void OnLoadPackageSucceeded(const FName& PackageName, class UPackage* LoadedPackage, EAsyncLoadingResult::Type Result);// Event

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Level | Async | Load")
		bool LoadLevel_Async(FString Level_Name_Address);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Level | Async | Load")
		void BlueprintOnLoadPackageSucceeded();

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Level | Async | Load")
		void LoadTheLoadedLevel();

	//UFUNCTION(BlueprintCallable, Category = "Wrexa | Level | Async | Load")
	void OnPreHeatFinished();
};
