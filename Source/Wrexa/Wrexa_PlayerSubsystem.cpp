// Code Done By Punal Manalan 2022


#include "Wrexa_PlayerSubsystem.h"
#include "Misc/PackageName.h"
#include "Engine/GameInstance.h"
#include "Engine/EngineTypes.h"
#include "UObject/UObjectGlobals.h"


void UWrexa_PlayerSubsystem::OnLoadPackageSucceeded(const FName& MapName, UPackage* ThePackage, EAsyncLoadingResult::Type TheResult)
{
	// cLoadTimerHandler is a FTimeHandler
	BlueprintOnLoadPackageSucceeded();
	IsLevelLoaded = true;
	//GetWorld()->GetTimerManager().SetTimer(LoadAsyncTimer, this, &UWrexa_PlayerSubsystem::OnPreHeatFinished, 3.0f, false);
}

bool UWrexa_PlayerSubsystem::LoadLevel_Async(FString Level_Name_Address)
{
	if (!IsLevelLoaded)
	{
		if (!IsLoadingLevel)
		{
			if (FPackageName::DoesPackageExist(Level_Name_Address) && (!IsLoadingLevel))
			{
				Name_Of_Level_Being_Loaded = FPaths::GetCleanFilename(Level_Name_Address);
				LoadPackageAsync(Level_Name_Address, FLoadPackageAsyncDelegate::CreateUObject(this, &UWrexa_PlayerSubsystem::OnLoadPackageSucceeded), 0, PKG_ContainsMap);
				IsLoadingLevel = true;
				return true;
			}
		}
	}
	return false;
}
void UWrexa_PlayerSubsystem::BlueprintOnLoadPackageSucceeded()
{

}
void UWrexa_PlayerSubsystem::LoadTheLoadedLevel()
{
	GetLocalPlayer()->PlayerController->ClientTravel(Name_Of_Level_Being_Loaded, ETravelType::TRAVEL_Absolute);
}
void UWrexa_PlayerSubsystem::OnPreHeatFinished()
{
	IsLoadingLevel = false;
	IsLevelLoaded = false;
	LoadTheLoadedLevel();
}