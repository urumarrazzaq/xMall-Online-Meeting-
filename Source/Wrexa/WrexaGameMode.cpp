// Code Done By Punal Manalan 2022

#include "WrexaGameMode.h"
#include "Wrexa_Instance.h"
#include "Kismet/GameplayStatics.h"
#include "Wrexa_Game_State.h"
#include "Wrexa_Player_State.h"
#include "Wrexa_Player_Controller.h"
#include "Wrexa_Share_Component.h"
//#include "UObject/ConstructorHelpers.h"

AWrexaGameMode::AWrexaGameMode()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
	//static ConstructorHelpers::FClassFinder<AGameStateBase> GameStateBPClass(TEXT("/Game/Wrexa/Blueprints/GameStates/Wrexa_Base_GameState_BP"));
	//if (GameStateBPClass.Class != NULL)
	//{
	//	//DefaultGameStateClass = GameStateBPClass.Class;
	//	GameStateClass = GameStateBPClass.Class;
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Failed to Find /Game/Wrexa/Blueprints/GameStates/Wrexa_Base_GameState_BP"));
	//}
	// Set the default game state
	//GameStateClass = AWrexa_Game_State::StaticClass();

	//static ConstructorHelpers::FClassFinder<APlayerState> PlayerStateBPClass(TEXT("/Game/Wrexa/Blueprints/PlayerStates/Wrexa_Base_PlayerState_BP"));
	//if (PlayerStateBPClass.Class != NULL)
	//{
	//	//DefaultPlayerStateClass = PlayerStateBPClass.Class;
	//	PlayerStateClass = PlayerStateBPClass.Class;
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Failed to Find /Game/Wrexa/Blueprints/PlayerStates/Wrexa_Base_PlayerState_BP"));
	//}

	//When No Blueprint is Available use This
	TSubclassOf<AGameStateBase> MyGameStateClass = AWrexa_Game_State::StaticClass();
	GameStateClass = MyGameStateClass;
	TSubclassOf<APlayerState> MyPlayerStateClass = AWrexa_Player_State::StaticClass();
	PlayerStateClass = MyPlayerStateClass;
	TSubclassOf<APlayerController> MyPlayerControllerClass = AWrexa_Player_Controller::StaticClass();
	PlayerControllerClass = MyPlayerControllerClass;
}

void AWrexaGameMode::BeginPlay()
{
	Super::BeginPlay();
	//Server Only
/*#if UE_SERVER
	//Take In Command Line Arguments First For the Map Lodaing ONLY FOR SERVER
	FString MapName;
	bool MapName_Received = false;
	const TCHAR* cmdLineStr = FCommandLine::Get();
	FString MatchRoomName;
	FString PlayerHostName;
	FString MatchIP;
	if (cmdLineStr != NULL)
	{
		FString InputArg;//EXAMLE: Wrexa.exe -GameType="Open_Test_Map"
		//TODO REMAINING
		if (FParse::Value(FCommandLine::Get(), TEXT("MatchRoomName"), InputArg))//Epects a Map Name
		{
			MatchRoomName = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
		}
		if (FParse::Value(FCommandLine::Get(), TEXT("HostPlayerName"), InputArg))//Epects a Map Name
		{
			PlayerHostName = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
		}
		if (FParse::Value(FCommandLine::Get(), TEXT("GameType"), InputArg))//Epects a Map Name
		{
			MapName_Received = true;
			InputArg = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
			MapName = InputArg;
		}
		if (FParse::Value(FCommandLine::Get(), TEXT("MatchIP"), InputArg))//Epects a Map Name
		{
			MatchIP = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
		}
	}
	Super::BeginPlay();
	FString ASD = "Wrexa GAME MODE BEGIN PLAY";
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ASD);
	if (MapName_Received)
	{
		ASD = (WrexaGetGameInstance(this) == nullptr) ? "Instance Is NullPtr" : "Instance is Success";
		UE_LOG(LogTemp, Warning, TEXT("%s"), *ASD);
		UWrexa_Instance* InstancePtr = Cast<UWrexa_Instance>(WrexaGetGameInstance(this));
		ASD = (InstancePtr == nullptr)?"Instance Cast Is NullPtr":"Instance Cast is Success";
		UE_LOG(LogTemp, Warning, TEXT("%s"), *ASD);
		FString CmdTravel = "open " + MapName;

		//TODO CHECK THIS
		//InstancePtr->Host_Server_Int(7987, MapName);
		if (WrexaGetGameInstance(this)->Exec(GEngine->GetWorldFromContextObject((UObject*)this, EGetWorldErrorMode::LogAndReturnNull), *CmdTravel))
		{
			//InstancePtr->IsServer = true;//THIS IS ONLY TRUE FOR CLIENT SIDE when the Client is Hosting A Match
			InstancePtr->Match_Room_Name = MatchRoomName;
			InstancePtr->Host_Player_Name = PlayerHostName;
			InstancePtr->Game_Type = MapName;
			InstancePtr->ServerPort = this->GetWorld()->URL.Port;
			InstancePtr->ServerIP = MatchIP;
		}
	}
#endif // UE_SERVER*/
}

void AWrexaGameMode::PreLogin(
	const FString& Options,
	const FString& Address,
	const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage
)
{
#if !UE_SERVER
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
#else
	//FString Received_Option = Options;
	//UE_LOG(LogTemp, Log, TEXT("The Received Join Option From User: %s"), *Received_Option);
	//Allowed_Users_With_Unique_Join_Key
	TArray<FString> Out;
	TArray<FString> UserName;
	TArray<FString> Password;
	Options.ParseIntoArray(Out, TEXT("?"), true);

	bool bIsClientAllowed = (Out.Num() >= 3);

	if (!bIsClientAllowed)
	{
		ErrorMessage = "You are not allowed to join this server, Found No UserName And Key IN AWrexaGameMode::PreLogin";
		return;
	}
	else
	{
		(Out[0]).ParseIntoArray(UserName, TEXT("="), true);//1,2 Or 0,1 
		(Out[1]).ParseIntoArray(Password, TEXT("="), true);

		bIsClientAllowed = ((UserName.Num() >= 2) && (Password.Num() >= 2));
		if (!bIsClientAllowed)
		{
			ErrorMessage = "You are not allowed to join this server, Username Or Key After Parsing Does not the Values IN AWrexaGameMode::PreLogin";
			return;
		}
	}

	if (bIsClientAllowed)
	{
		UWrexa_Instance* Returned_UWrexa_Instance = (UWrexa_Instance*)(WrexaGetGameInstance(this));
		//TODO COMPLETE THIS
		if ((Returned_UWrexa_Instance != nullptr))
		{
			bIsClientAllowed = Returned_UWrexa_Instance->Allowed_Users_With_Unique_Join_Key.Contains(UserName[1]);
			if (!bIsClientAllowed)
			{
				ErrorMessage = "You are not allowed to join this server, Unauthorized User: \"" + UserName[1] + "\" IN AWrexaGameMode::PreLogin";
				return;
			}
			else
			{
				bIsClientAllowed = (((Returned_UWrexa_Instance->Allowed_Users_With_Unique_Join_Key)[UserName[1]]) == Password[1]);
				if (!bIsClientAllowed)
				{
					ErrorMessage = "You are not allowed to join this server, Invalid Unique Join Key: \"" + Password[1] + "\" For User: \"" + UserName[1] + "\" IN AWrexaGameMode::PreLogin";
					return;
				}
				else
				{
					//TODO WARNING ERROR, Very Bad, What How is Client Logout Checked?
					bIsClientAllowed = !((Returned_UWrexa_Instance->Users_In_Match).Contains(UserName[1]));

					if (!bIsClientAllowed)
					{
						ErrorMessage = "You are not allowed to join this server, User already In Server IN AWrexaGameMode::PreLogin";
						return;
					}
					//else
					//{
					//	(Returned_UWrexa_Instance->Users_In_Match).Add(UserName[1], New_Unique_Join_Key);
					//}
				}
			}
		}
		else
		{
			ErrorMessage = "Server Internal Failure IN AWrexaGameMode::PreLogin";
			return;
		}
	}

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
#endif
}

APlayerController* AWrexaGameMode::Login
(
	UPlayer* NewPlayer,
	ENetRole InRemoteRole,
	const FString& Portal,
	const FString& Options,
	const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage
)
{
#if !UE_SERVER
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
#else
	UWrexa_Instance* Returned_UWrexa_Instance = (UWrexa_Instance*)(WrexaGetGameInstance(this));
	//TODO COMPLETE THIS
	if ((Returned_UWrexa_Instance != nullptr))
	{
		TArray<FString> Out;
		TArray<FString> UserName;
		TArray<FString> Password;
		Options.ParseIntoArray(Out, TEXT("?"), true);

		bool bIsClientAllowed = (Out.Num() >= 3);

		if (!bIsClientAllowed)
		{
			ErrorMessage = "You are not allowed to join this server, Found No UserName And Key IN AWrexaGameMode::Login";
			return nullptr;
		}
		else
		{
			(Out[0]).ParseIntoArray(UserName, TEXT("="), true);//1,2 Or 0,1 
			(Out[1]).ParseIntoArray(Password, TEXT("="), true);

			bIsClientAllowed = ((UserName.Num() >= 2) && (Password.Num() >= 2));
			if (!bIsClientAllowed)
			{
				ErrorMessage = "You are not allowed to join this server, Username Or Key After Parsing Does not Contain the Values IN AWrexaGameMode::Login";
				return nullptr;
			}
		}

		APlayerController* Returned_Player_Controller = nullptr;
		Returned_Player_Controller = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

		if (Returned_Player_Controller != nullptr)
		{
			(Returned_UWrexa_Instance->Users_In_Match).Add(UserName[1], Returned_Player_Controller);
			(Returned_UWrexa_Instance->Users_In_Match_Controllers).Add(Returned_Player_Controller, UserName[1]);

			AWrexa_Game_State* Current_Game_State = Cast<AWrexa_Game_State>(UGameplayStatics::GetGameMode(this)->GameState);
			if (Current_Game_State != nullptr)
			{
				if (IsValid(Current_Game_State))
				{
					Current_Game_State->Users_IDs_In_Match.AddUnique(UserName[1]);

					AWrexa_Player_Controller* The_Wrexa_Player_Controller = Cast<AWrexa_Player_Controller>(Returned_Player_Controller);
					if (The_Wrexa_Player_Controller != nullptr)
					{
						The_Wrexa_Player_Controller->UserAtName = UserName[1];
					}
				}
			}

			return Returned_Player_Controller;
		}
		else
		{
			ErrorMessage = "Server Internal Failure IN AWrexaGameMode::Login";
			return nullptr;
		}
	}
	else
	{
		ErrorMessage = "Server Internal Failure IN AWrexaGameMode::Login";
		return nullptr;
	}
#endif
}

void AWrexaGameMode::Logout(AController* Exiting)
{
	UWrexa_Instance* Returned_UWrexa_Instance = (UWrexa_Instance*)(WrexaGetGameInstance(this));
	//TODO COMPLETE THIS
	if ((Returned_UWrexa_Instance != nullptr))
	{
		FString* User_At_Name = nullptr;
		User_At_Name = (Returned_UWrexa_Instance->Users_In_Match_Controllers).Find(Exiting);

		if (User_At_Name != nullptr)
		{
			(Returned_UWrexa_Instance->Users_In_Match).Remove(*User_At_Name);
			(Returned_UWrexa_Instance->Users_In_Match_Controllers).Remove(Exiting);
		}
	}

	AWrexa_Player_Controller* The_Wrexa_Player_Controller = Cast<AWrexa_Player_Controller>(Exiting);
	if (The_Wrexa_Player_Controller != nullptr)
	{
		for (UWrexa_Share_Component* Each_Share_Component : The_Wrexa_Player_Controller->Owned_Share_Components)
		{
			if (Each_Share_Component != nullptr)
			{
				Each_Share_Component->Remove_Owner(The_Wrexa_Player_Controller);
			}
		}

		AWrexa_Game_State* Current_Game_State = Cast<AWrexa_Game_State>(UGameplayStatics::GetGameMode(this)->GameState);
		if (Current_Game_State != nullptr)
		{
			if (IsValid(Current_Game_State))
			{
				Current_Game_State->Users_IDs_In_Match.AddUnique(The_Wrexa_Player_Controller->UserAtName);
			}
		}
	}

	Super::Logout(Exiting);
}

class UGameInstance* AWrexaGameMode::WrexaGetGameInstance(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	return World ? World->GetGameInstance() : nullptr;
}