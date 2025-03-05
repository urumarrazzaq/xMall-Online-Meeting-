// Code Done By Punal Manalan 2022


#include "Wrexa_Instance.h"
#include "SocketSubsystem.h"
#include "Kismet/GameplayStatics.h"
#if UE_SERVER
#include "Misc/DateTime.h"
#include "IWebSocket.h"
#include "WebSocketsModule.h"

#include "CSSocketSubsystem.h"//For The Functions
#endif // UE_SERVER

#include "WrexaGameMode.h"
#include "Wrexa_Game_State.h"

#include "Wrexa_Screen_Capture.h"

// Called when the game starts or when spawned
void UWrexa_Instance::OnStart()
{
	Super::OnStart();
	BPEvent_Instance_On_LevelLoaded.AddDynamic(this, &UWrexa_Instance::OnLevelLoadedDelegate);

	//Server Only
#if UE_SERVER
	if(Read_Values_From_Custom_INI("ServerConfig.ini"))
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully Read ServerConfig.ini"));
		if (Get_Required_Commandline_Arguments())
		{
			UE_LOG(LogTemp, Log, TEXT("Successfully Fetched Required Command Line Arguments"));
			UE_LOG(LogTemp, Log, TEXT("Wrexa Server Socket Initializing"));
			RunServerCode();//Runs the Server Code Only If Successful
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to Get Required Command Line Arguments"));
			FPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Read ServerConfig.ini"));
		FPlatformMisc::RequestExit(false);
	}

#endif
}

void UWrexa_Instance::OnLevelLoadedDelegate(FString LevelName)
{
	UE_LOG(LogTemp, Log, TEXT("Level Loaded And Function Called"));

	if (UGameplayStatics::GetGameMode(this) != nullptr)
	{
		FString ReturnedClassName = UGameplayStatics::GetGameMode(this)->GetName();
		UE_LOG(LogTemp, Log, TEXT("Current Game_Mode is \"%s\""), *ReturnedClassName);

		Current_Game_State = Cast<AWrexa_Game_State>(UGameplayStatics::GetGameMode(this)->GameState);
		if (Current_Game_State == nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("Current_Game_State Is Not a SubClass of AWrexa_Game_State"));
		}
		else
		{
			if (UGameplayStatics::GetGameMode(this) != nullptr)
			{
				UE_LOG(LogTemp, Log, TEXT("Successfully Casted to AWrexa_Game_State"));

				Current_Game_State->Server_Match_ID = Match_ID;
				Current_Game_State->Server_Host_User_At_Name = Host_User_At_Name;
				Current_Game_State->Server_Match_Room_Name = Match_Room_Name;
				Current_Game_State->Server_Match_Description = Match_Description;
				Current_Game_State->Server_Game_Type = Game_Type;
				Current_Game_State->Server_Ticket_Cost = Ticket_Cost;

				UE_LOG(LogTemp, Log, TEXT("Current_Game_State AWrexa_Game_State Info Start"));
				UE_LOG(LogTemp, Log, TEXT("Current_Game_State->Server_Match_ID = %s"), *(Current_Game_State->Server_Match_ID));
				UE_LOG(LogTemp, Log, TEXT("Current_Game_State->Server_Host_User_At_Name = %s"), *(Current_Game_State->Server_Host_User_At_Name));
				UE_LOG(LogTemp, Log, TEXT("Current_Game_State->Server_Match_Room_Name = %s"), *(Current_Game_State->Server_Match_Room_Name));
				UE_LOG(LogTemp, Log, TEXT("Current_Game_State->Server_Match_ID = %s"), *(Current_Game_State->Server_Match_Description));
				UE_LOG(LogTemp, Log, TEXT("Current_Game_State->Server_Game_Type = %s"), *(Current_Game_State->Server_Game_Type));
				UE_LOG(LogTemp, Log, TEXT("Current_Game_State->Server_Ticket_Cost = %s"), *(Current_Game_State->Server_Ticket_Cost));
				UE_LOG(LogTemp, Log, TEXT("Current_Game_State AWrexa_Game_State Info End"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to Cast to AWrexa_Game_State"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Current Game_Mode is Not Initalized and is Nullptr"));
	}

#if UE_SERVER
	if (!Is_Server_Match_Ready)
	{//First Time Ready Acknowledgement Request to the Main Server
		WebSocket->Send(SerializeDataFor_Server_Match_Server_Ready_Request(true));
	}
	//else
	//{
	//}
#else
	if (IsServer)
	{
		FURL ListenSettings;
		//FString FstringVal = ServerPort.ToString();
		//ListenSettings.Host = ServerIP;// "127.0.0.1"; TODO LOOK INTO THIS
		ListenSettings.Host = "127.0.0.1";//Seems Like the Server has to Run in Local Host
		//ListenSettings.Port = ServerPort;// FCString::Atoi(*(FstringVal));
		ListenSettings.Port = 0;// Chooses Random Port
		ListenSettings.Protocol = "unreal";
		//ListenSettings.Map = MapName;
		//FstringVal = "listen";
		//ListenSettings.AddOption(*(FstringVal));
		//CurrentPlayer->GetWorld()->Listen(ListenSettings);
		EGetWorldErrorMode TheError = EGetWorldErrorMode::ReturnNull;
		UWorld* World = GetEngine()->GetWorldFromContextObject((UObject*)this, TheError);
		if (World != nullptr)
		{
			// Listen was successful
			UE_LOG(LogTemp, Log, TEXT("Attempting to Start Listen Server"));
			if (!(World->Listen(ListenSettings)))
			{
				UE_LOG(LogTemp, Error, TEXT("Listen Failed"));
				//FPlatformMisc::RequestExit(false);
			}
			else
			{
				//Server Only#if UE_SERVER
				UE_LOG(LogTemp, Log, TEXT("Successfully Started Listen Server"));
				if (Match_Room_Name != "")
				{
					bool canBind = false;
					TSharedRef<FInternetAddr> localIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, canBind);
					if (localIp->IsValid())
					{
						ServerIP = localIp->ToString(false);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Failed to Get Local IP"));
						FPlatformMisc::RequestExit(false);
					}
					ServerPort = World->URL.Port;
				}
			}
		}
		else
		{
			// Listen failed
			UE_LOG(LogTemp, Error, TEXT("Failed to get world with error %d"), TheError);
			FPlatformMisc::RequestExit(false);
		}
	}
#endif
	IsLoadingLevel = false;
	//TODO ONCE THE LEVEL IS LOADED SEND CONFIRMATION TO THE MAIN SERVER
}

bool UWrexa_Instance::Read_Values_From_Custom_INI(FString File_Name)
{
	File_Name = FPaths::ConvertRelativePathToFull(FPaths::LaunchDir() + "/" + File_Name);
	FPaths::RemoveDuplicateSlashes(File_Name);
	FPaths::NormalizeFilename(File_Name);

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*File_Name))
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't find configuration file: %s"), *File_Name);
		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("Opening %s Config File"), *File_Name);

	FString section = "Match_Making_Server";
	FString name = "";

	GConfig->Flush(true, File_Name);

	name = "MODNET_ServerAddress";
	if (GConfig->GetString(*section, *name, Main_Server_Address, *File_Name))
	{
		Main_Server_Address = Main_Server_Address.Replace(TEXT("\""), TEXT(""));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't find MODNET_ServerAddress in the Config: %s"));
		return false;
	}
	
	name = "MODNET_Bypass_ServerIP";
	if (GConfig->GetString(*section, *name, Bypass_ServerIP, *File_Name))
	{
		Bypass_ServerIP = Bypass_ServerIP.Replace(TEXT("\""), TEXT(""));
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't find MODNET_Bypass_ServerIP in the Config: %s"));
		return false;
	}
	//GConfig->Flush(false, GGameIni)
}

void UWrexa_Instance::LoadComplete(const float LoadTime,	const FString& a_MapName)
{
	Super::LoadComplete(LoadTime, a_MapName);
	BPEvent_Instance_On_LevelLoaded.Broadcast(a_MapName);
}

void UWrexa_Instance::Connect_To_Server(FName a_ServerAddress, FName a_Server_Port)
{
	FString CmdTravel = "open " + a_ServerAddress.ToString() + ":" + a_Server_Port.ToString();
	//CurrentPlayer->ConsoleCommand(*CmdTravel, true);	
	EGetWorldErrorMode TheError = EGetWorldErrorMode::ReturnNull;
	if (GetEngine()->Exec(GetEngine()->GetWorldFromContextObject((UObject*)this, TheError), *CmdTravel))//PEDNING CHECK
	{
		IsLoadingLevel = true;
	}
}

void UWrexa_Instance::Host_Server_Int(int a_Server_Port, FString a_MapName)
{
#if UE_SERVER
	EGetWorldErrorMode TheError = EGetWorldErrorMode::ReturnNull;
	//if (GetEngine()->Exec(GetEngine()->GetWorldFromContextObject((UObject*)this, TheError), *CmdTravel))
	if(GetWorld()->ServerTravel(a_MapName, true))//Absolute Travel, TODO WARNING ERROR Absolute vs Relative Travel
	{
		ServerPort = a_Server_Port;
		IsLoadingLevel = true;
		IsServer = true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Server Travel To Map: %s"), *a_MapName);
	}
#else
	//FString CmdTravel = "open " + MapName + "?listen?port=" + ServerPort.ToString();
	//FString CmdTravel = "open " + MapName;
	//CurrentPlayer->ConsoleCommand(*CmdTravel, true);
	EGetWorldErrorMode TheError = EGetWorldErrorMode::ReturnNull;
	FString CmdTravel = "open " + a_MapName;
	if (GetEngine()->Exec(GetEngine()->GetWorldFromContextObject((UObject*)this, TheError), *CmdTravel))
	{
		ServerPort = a_Server_Port;
		IsLoadingLevel = true;
		IsServer = true;
	}
#endif
	//CurrentPlayer->GetWorld()->ServerTravel(MapName,true);
}

void UWrexa_Instance::Host_Server(FName a_Server_Port, FString a_MapName)
{
	Host_Server_Int(FCString::Atoi(*(a_Server_Port.ToString())), a_MapName);
}

void UWrexa_Instance::Get_Connected_Client_IP(APlayerController* Player_Controller, FString& IP, int& Port)
{
	//Querry_DNS_String(
	uint32 OutIP = 0;
	Port = Player_Controller->NetConnection->GetRemoteAddr()->GetPort();
	Player_Controller->NetConnection->GetRemoteAddr()->GetIp(OutIP);
	IP = FString::FromInt((0xff & (OutIP >> 24)));
	IP += ".";
	IP += FString::FromInt((0xff & (OutIP >> 16)));
	IP += ".";
	IP += FString::FromInt((0xff & (OutIP >> 8)));
	IP += ".";
	IP += FString::FromInt((0xff & OutIP));
}

APlayerController* UWrexa_Instance::GetMainController(const UObject* WorldContextObject)
{
	// Get world context (containing player controllers)
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		// Loop on player controllers
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			// Get player controller from iterator
			APlayerController* PlayerController = Iterator->Get();

			// Get local player if exist
			ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();

			// If it's local and id is 0, it's the main controller
			if (LocalPlayer != nullptr && LocalPlayer->GetControllerId() == 0)
			{
				return PlayerController;
			}
		}
	}

	// Not found
	return nullptr;
}

void UWrexa_Instance::BeginDestroy()
{
#if UE_SERVER
	if (WebSocket->IsConnected())
		WebSocket->Close();
#else
#ifdef _WIN32
#ifdef ENABLE_SCREEN_CAPTURE
	FPlatformProcess::Sleep(0.1);
	Singleton_Screen_Capture_Thread::Stop_Externally();
	FPlatformProcess::Sleep(0.1);
	Singleton_DirectX_Screen_Capture_Direct_Access_Buffer::Stop_Externally();
	FPlatformProcess::Sleep(0.1);
#endif
#endif

#endif // UE_SERVER

	Super::BeginDestroy();
}

//Server Only
#if UE_SERVER
void UWrexa_Instance::RunServerCode()
{
	//Server Only
	FString ASD = "Starting Wrexa Dedicated Server";
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ASD);
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
		FModuleManager::Get().LoadModule("WebSockets");

	if (!FModuleManager::Get().IsModuleLoaded("Json"))
		FModuleManager::Get().LoadModule("Json");

	if (!FModuleManager::Get().IsModuleLoaded("JsonUtilities"))
		FModuleManager::Get().LoadModule("JsonUtilities");

	ASD = "Connecting to the Server Address: \" " + Main_Server_Address+ " \"";
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ASD);

	WebSocket = FWebSocketsModule::Get().CreateWebSocket(Main_Server_Address, (Is_Web_Socket_Https_SERVER?"wss":"ws"));
	WebSocket->OnConnected().AddUObject(this, &UWrexa_Instance::SocketConnected);
	WebSocket->OnConnectionError().AddUObject(this, &UWrexa_Instance::SocketConnectionError);
	WebSocket->OnClosed().AddUObject(this, &UWrexa_Instance::SocketClosed);
	WebSocket->OnMessage().AddUObject(this, &UWrexa_Instance::SocketMessageReceived);
	WebSocket->OnMessageSent().AddUObject(this, &UWrexa_Instance::SocketMessageSent);

	WebSocket->Connect();
}

void UWrexa_Instance::SocketConnected()
{
	FString ASD = "Successfully Connected";
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ASD);

	EGetWorldErrorMode TheError = EGetWorldErrorMode::ReturnNull;
	UWorld* World = GetEngine()->GetWorldFromContextObject((UObject*)this, TheError);
	if (World != nullptr)
	{
		//FString CmdTravel = "open " + Available_Maps[Game_Type];
		bool canBind = false;
		TSharedRef<FInternetAddr> localIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, canBind);
		if (localIp->IsValid())
		{
			ServerIP = localIp->ToString(false);
			ServerPort = World->URL.Port;
			ASD = "Server IP  : \"" + ServerIP + "\"\nServer Port: \"" + FString::FromInt(ServerPort) + "\"";
			WebSocket->Send(SerializeDataFor_Server_Match_Server_Verify_Request());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to Get Local IP"));
			FPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get world with error %d"), TheError);
		FPlatformMisc::RequestExit(false);
	}


		
	//WebSocket->Send(MessageString);//NOTE: Message will be Sent after the Level is Loaded Check Above
}

void UWrexa_Instance::SocketConnectionError(const FString& Error)
{
	FString ASD = Error;
	UE_LOG(LogTemp, Warning, TEXT("Socket Connection Error: %s"), *ASD);
	FPlatformMisc::RequestExit(false);
}

void UWrexa_Instance::SocketClosed(const int32 StatusCode, const FString& Reason, const bool bWasClean)
{
	FString ASD = "Connection Closed " + Reason;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ASD);
	FPlatformMisc::RequestExit(false);
}

void UWrexa_Instance::SocketMessageReceived(const FString& MessageString)
{
	FString ASD = MessageString;
	UE_LOG(LogTemp, Warning, TEXT("The Received Message From Internal Server: %s"), *ASD);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(MessageString);
	//FString OutputString;
	//TSharedRef< TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	//FReceived_Socket_JSON TestMessage = {"asdasd","asdasdasd"};

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		bool IsRequestSuccessful = true;
		const FString Type = JsonObject->GetStringField("Data_Type");
		const TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField("Data");
		TSharedPtr<FJsonObject> TempData = MakeShareable(new FJsonObject());

		UE_LOG(LogTemp, Log, TEXT("The Received Data Type: %s"), *Type);

		if (Type == "Server_Match_Server_Verify_Response")
		{
			if ((Data->GetObjectField("Is_Request_Successful")->GetStringField("Is_Successful") == "False"))
			{
				UE_LOG(LogTemp, Error, TEXT("Match Server Verification Failed"));
				FPlatformMisc::RequestExit(false);
				return;
			}
			else
			{
				FString Received_Host_User_At_Name = Data->GetStringField("Host_User_At_Name");
				FString Received_Match_Room_Name = Data->GetStringField("Match_Room_Name");
				FString Received_Match_Description = Data->GetStringField("Match_Description");
				FString Received_Game_Type = Data->GetStringField("Game_Type");
				FString Received_Ticket_Cost = Data->GetStringField("Ticket_Cost");
				FString Received_Match_Flag = Data->GetStringField("Match_Flag");
				FString Received_Data = Data->GetStringField("Data");

				//Match_ID = Match_ID;
				Host_User_At_Name = Received_Host_User_At_Name;
				Match_Room_Name = Received_Match_Room_Name;
				Match_Description = Received_Match_Description;
				Game_Type = Received_Game_Type;
				Ticket_Cost = Received_Ticket_Cost;
				Match_Flag = Received_Match_Flag;
				Received_Extra_Data = Received_Data;

				
				UE_LOG(LogTemp, Log, TEXT("Host_User_At_Name: %s"), *Host_User_At_Name);
				
				UE_LOG(LogTemp, Log, TEXT("Match_Room_Name: %s"), *Match_Room_Name);
				
				UE_LOG(LogTemp, Log, TEXT("User_Match_Description: %s"), *Match_Description);
				
				UE_LOG(LogTemp, Log, TEXT("User_Game_Type: %s"), *Game_Type);
				
				UE_LOG(LogTemp, Log, TEXT("Ticket_Cost: %s"), *Ticket_Cost);
				
				UE_LOG(LogTemp, Log, TEXT("Match_Flag: %s"), *Match_Flag);

				UE_LOG(LogTemp, Log, TEXT("Extra_Data: %s"), *Received_Extra_Data);

				bool Does_Map_Exist_In_Server = Available_Maps.Contains(Game_Type);
				if (Does_Map_Exist_In_Server)
				{
					UE_LOG(LogTemp, Log, TEXT("Opening Map By the Name \' %s \'"), *Game_Type);
					Received_Game_Type = Available_Maps[Game_Type];
					UE_LOG(LogTemp, Log, TEXT("Opening Map Is At Location \' %s \'"), *Received_Game_Type);
					Received_Game_Type = Game_Type;
					EGetWorldErrorMode TheError = EGetWorldErrorMode::ReturnNull;
					UWorld* World = GetEngine()->GetWorldFromContextObject((UObject*)this, TheError);
					if (World != nullptr)
					{
						//FString CmdTravel = "open " + Available_Maps[Game_Type];
						Host_Server_Int(0, Available_Maps[Game_Type]);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Failed to get world with error %d"), TheError);
						FPlatformMisc::RequestExit(false);
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Map By the Name \' %s \' Does not Exist"), *Game_Type);
					FPlatformMisc::RequestExit(false);
				}
				//TODO LOAD MAP HERE
			}
		}
		else if (Type == "Server_Match_Server_Ready_Response")
		{
			if ((Data->GetObjectField("Is_Request_Successful")->GetStringField("Is_Successful") == "False"))
			{
				UE_LOG(LogTemp, Error, TEXT("Match Server Ready Failed"));
				FPlatformMisc::RequestExit(false);
				return;
			}
			else
			{
				if (!Is_Server_Match_Ready)
				{
					FString Received_Data = Data->GetStringField("Data");
					UE_LOG(LogTemp, Log, TEXT("Match Server Is Now Fully Ready And Acknowleded by the Server\nReceived Extra_Data: %s"), *Received_Extra_Data);
					Is_Server_Match_Ready = true;
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Match Server Is Already Ready But Server Sent Ready Acknowledgement Again\nReceived Extra_Data: %s"), *Received_Extra_Data);
				}
			}
		}
		else if (Type == "Server_Stop_Match_Response")
		{
			//TODO COMPLETE THIS
		}
		else if (Type == "Server_Match_Allow_User_Request")
		{
			
			UE_LOG(LogTemp, Log, TEXT("Server_Match_Allow_User_Request"));

			FString Received_User_At_Name = Data->GetStringField("User_At_Name");
			FString Received_User_Index_Key = Data->GetStringField("User_Index_Key");
			FString Received_Data = Data->GetStringField("Data");

			
			UE_LOG(LogTemp, Log, TEXT("User_At_Name: %s"), *Received_User_At_Name);
			
			UE_LOG(LogTemp, Log, TEXT("User_Index_Key: %s"), *Received_User_Index_Key);
			
			UE_LOG(LogTemp, Log, TEXT("Extra_Data:%s"), *Received_Data);


			bool Does_User_Already_Exist_In_Server = Allowed_Users_With_Unique_Join_Key.Contains(Received_User_At_Name);
			if (Does_User_Already_Exist_In_Server)
			{
				WebSocket->Send(SerializeDataFor_Server_Match_Allow_User_Response(Received_User_At_Name, Received_User_Index_Key, Allowed_Users_With_Unique_Join_Key[Received_User_At_Name], Received_Data));
			}
			else
			{
				FString New_Unique_Join_Key = FString::FromInt(FMath::RandRange(0, 2147483645)) + FString::FromInt(FMath::RandRange(0, 2147483645)) + FString::FromInt(FMath::RandRange(0, 2147483645));
				Allowed_Users_With_Unique_Join_Key.Add(Received_User_At_Name, New_Unique_Join_Key);//TODO ADD VERIFICATION for Public and Private
				WebSocket->Send(SerializeDataFor_Server_Match_Allow_User_Response(Received_User_At_Name, Received_User_Index_Key, New_Unique_Join_Key, Received_Data));
			}
		}
		else
		{
			
				UE_LOG(LogTemp, Error, TEXT("Invaild_Request Received"));
		}
	}
}

void UWrexa_Instance::SocketMessageSent(const FString& MessageString)
{
	FString ASD = MessageString;
	UE_LOG(LogTemp, Warning, TEXT("The Message to send to the Internal Server: %s"), *ASD);
}

bool UWrexa_Instance::Get_Required_Commandline_Arguments()
{
	FString InputArg = "";//EXAMLE: Wrexa.exe -GameType="Open_Test_Map"
	
	//if (FParse::Value(FCommandLine::Get(), TEXT("MODNET_ServerAddress"), InputArg))//Epects a Map Name
	//{
	//	//MODNET_ServerAddress = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//	Main_Server_Address = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Failed Find Required Argument MODNET_ServerAddress"));
	//	FPlatformMisc::RequestExit(false);
	//}

	if (FParse::Value(FCommandLine::Get(), TEXT("a_Ticket_ID"), InputArg))//Epects a Map Name
	{
		//a_Ticket_ID = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
		Match_ID = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
		UE_LOG(LogTemp, Log, TEXT("Inputed Command Line Argument For a_Ticket_ID: %s"), *Match_ID);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed Find Required Argument a_Ticket_ID"));
		FPlatformMisc::RequestExit(false);
		return false;
	}

	if (FParse::Value(FCommandLine::Get(), TEXT("a_Password"), InputArg))//Epects a Map Name
	{
		//a_Password = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
		Internal_Verification_Password = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
		UE_LOG(LogTemp, Log, TEXT("Inputed Command Line Argument For a_Password: %s"), *Internal_Verification_Password);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed Find Required Argument a_Password"));
		FPlatformMisc::RequestExit(false);
		return false;
	}

	//if (FParse::Value(FCommandLine::Get(), TEXT("a_Host_User_Name"), InputArg))//Epects a Map Name
	//{
	//	//a_Host_User_Name = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//	Host_User_At_Name = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Failed Find Required Argument a_Host_User_Name"));
	//	FPlatformMisc::RequestExit(false);
	// return;
	//}

	//if (FParse::Value(FCommandLine::Get(), TEXT("a_Bypass_User_At_Name"), InputArg))//Epects a Map Name
	//{
	//	//a_Host_User_Name = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//	Bypass_User_At_Name = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Failed Find Required Argument a_Host_Index_Key"));
	//	FPlatformMisc::RequestExit(false);
	// return;
	//}

	//if (FParse::Value(FCommandLine::Get(), TEXT("a_Match_Room_Name"), InputArg))//Epects a Map Name
	//{
	//	//a_Match_Room_Name = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//	Match_Room_Name = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Failed Find Required Argument a_Match_Room_Name"));
	//	FPlatformMisc::RequestExit(false);
	// return;
	//}

	//if (FParse::Value(FCommandLine::Get(), TEXT("a_Match_Description"), InputArg))//Epects a Map Name
	//{
	//	//a_Match_Description = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//	Match_Description = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Failed Find Required Argument a_Match_Description"));
	//	FPlatformMisc::RequestExit(false);
	// return;
	//}

	//if (FParse::Value(FCommandLine::Get(), TEXT("a_Game_Type"), InputArg))//Epects a Map Name
	//{
	//	//a_Game_Type = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//	InputArg = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//	Game_Type = InputArg;
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Failed Find Required Argument a_Game_Type"));
	//	FPlatformMisc::RequestExit(false);
	// return;
	//}

	//if (FParse::Value(FCommandLine::Get(), TEXT("a_Ticket_Cost"), InputArg))//Epects a Map Name
	//{
	//	//a_Ticket_Cost = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//	Ticket_Cost = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Failed Find Required Argument a_Ticket_Cost"));
	//	FPlatformMisc::RequestExit(false);
	// return;
	//}

	//if (FParse::Value(FCommandLine::Get(), TEXT("a_Data"), InputArg))//Epects a Map Name
	//{
	//	//CMD_a_Data = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//	Received_Extra_Data = InputArg.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT("")); // replace quotes
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Failed Find Required Argument a_Data"));
	//	FPlatformMisc::RequestExit(false);
	// return;
	//}

	return true;
}

FString UWrexa_Instance::SerializeDataFor_Server_Match_Server_Verify_Request()
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> TempJsonObject = MakeShareable(new FJsonObject);

	JsonObject->SetStringField("Data_Type", "Server_Match_Server_Verify_Request");
	//TempJsonObjectTwo->SetStringField("Is_Successful", "True");
	//TempJsonObjectTwo->SetStringField("Reason", "None");
	//TempJsonObject->SetObjectField("Is_Request_Successful", TempJsonObjectTwo);
	//TempJsonObject->SetStringField("Bypass_User_At_Name", Bypass_User_At_Name);
	TempJsonObject->SetStringField("Match_ID", Match_ID);
	TempJsonObject->SetStringField("Password", Internal_Verification_Password);
	TempJsonObject->SetStringField("IP", Bypass_ServerIP);
	TempJsonObject->SetStringField("Port", FString::FromInt(ServerPort));
	TempJsonObject->SetStringField("Data", " ");

	JsonObject->SetObjectField("Data", TempJsonObject);
	//return JsonToString(JsonObject);
	FString JsonString;
	const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter))
		return JsonString;
	return "";
}
FString UWrexa_Instance::SerializeDataFor_Server_Match_Server_Ready_Request(bool Is_Match_Ready)
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> TempJsonObject = MakeShareable(new FJsonObject);

	JsonObject->SetStringField("Data_Type", "Server_Match_Server_Ready_Request");
	//TempJsonObjectTwo->SetStringField("Is_Successful", "True");
	//TempJsonObjectTwo->SetStringField("Reason", "None");
	//TempJsonObject->SetObjectField("Is_Request_Successful", TempJsonObjectTwo);
	//TempJsonObject->SetStringField("Bypass_User_At_Name", Bypass_User_At_Name);
	TempJsonObject->SetStringField("Match_ID", Match_ID);
	TempJsonObject->SetStringField("Is_Ready", (Is_Match_Ready?("True"):("False")));
	TempJsonObject->SetStringField("Data", " ");

	JsonObject->SetObjectField("Data", TempJsonObject);
	//return JsonToString(JsonObject);
	FString JsonString;
	const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter))
		return JsonString;
	return "";
}
FString UWrexa_Instance::SerializeDataFor_Server_Match_Allow_User_Response(FString a_User_At_Name, FString a_User_Index_Key, FString a_Unique_Join_Key, FString a_Data)
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> TempJsonObject = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> TempJsonObjectTwo = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> TempSecondJsonObject;
	JsonObject->SetStringField("Data_Type", "Server_Match_Allow_User_Response");
	
	TempJsonObjectTwo->SetStringField("Is_Successful", "True");
	TempJsonObjectTwo->SetStringField("Reason", "None");
	TempJsonObject->SetObjectField("Is_Request_Successful", TempJsonObjectTwo);
	
	TempJsonObject->SetStringField("User_At_Name", a_User_At_Name);
	TempJsonObject->SetStringField("User_Index_Key", a_User_Index_Key);
	TempJsonObject->SetStringField("Unique_Join_Key", a_Unique_Join_Key);
	TempJsonObject->SetStringField("Data", a_Data);

	JsonObject->SetObjectField("Data", TempJsonObject);
	//return JsonToString(JsonObject);
	FString JsonString;
	const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter))
		return JsonString;
	return "";
}
#endif // UE_SERVER

void UWrexa_Instance::On_New_Screen_Frame_Received_Implementation(const TArray <uint8>& RGBA_Buffer, int Width, int Height)
{
	return;
}