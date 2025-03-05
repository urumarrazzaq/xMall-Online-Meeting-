// Code Done By Punal Manalan 2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Wrexa_Instance.generated.h"

class AWrexa_Game_State;

#if UE_SERVER
class IWebSocket;
#endif // UE_SERVER

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBP_Callable_Event);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBP_Callable_Event_OnInteract, int, Value);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBP_Instance_Delegate, FString, LevelName);//TODO

UCLASS(EditInlineNew)
class WREXA_API UWrexa_Instance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void OnStart() override;
	virtual void LoadComplete(const float LoadTime, const FString& MapName);
	bool Read_Values_From_Custom_INI(FString File_Name);

public:
	//This Only Exists On the Server
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | GameStates")
		AWrexa_Game_State* Current_Game_State = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrexa | Maps")
		TMap<FString,FString> Available_Maps;//First is Map Name, Second Is Map Path, Available Maps and Their Path

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		bool Is_Web_Socket_Https_CLIENT = true;//Used to Set the Type of Websocket WS/WSS

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		bool Is_Web_Socket_Https_SERVER = true;//Used to Set the Type of Websocket WS/WSS

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Main_Server_Address = "wss://www.exampleserver:443";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Matchmaking_Server_Address = "wss://www.exampleserver:443";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		bool IsLoadingLevel = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		bool IsServer = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		int ServerPort = 7777;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString ServerIP = "127.0.0.1";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Bypass_ServerIP = "Some_Global_IP";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		bool LoggedInSuccessfully = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		bool JoinMatch = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		bool HostMatch = false;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
	//	bool Is_Server_Public = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		bool Is_Server_Match_Ready = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Match_ID = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Internal_Verification_Password = "";// Just Password in API

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Host_User_At_Name = "";

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
	//	FString Bypass_User_At_Name = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Match_Room_Name = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Match_Description = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Game_Type = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Ticket_Cost = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Match_Flag = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		FString Received_Extra_Data = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		TMap<FString, FString> Allowed_Users_With_Unique_Join_Key;//First is User_At_Name, Second is Unique_Join_Key

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		TMap<FString, AController*> Users_In_Match;//User_At_Name of Users

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Networking")
		TMap<AController*, FString> Users_In_Match_Controllers;//Controllers of Users

	//Networking
	UFUNCTION(BlueprintCallable, Category = "Wrexa | Networking | Connect To Server")
		void Connect_To_Server(FName a_ServerAddress, FName a_Server_Port);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Networking | Host Server")
		void Host_Server_Int(int a_Server_Port, FString a_MapName);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Networking | Host Server")
		void Host_Server(FName a_Server_Port, FString a_MapName);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Networking | Get IP from Client Side")
		void Get_Connected_Client_IP(APlayerController* Player_Controller, FString& IP, int& Port);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Networking")
		APlayerController* GetMainController(const UObject* WorldContextObject);

	//Event Delegate
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Wrexa | Networking")
		FBP_Instance_Delegate BPEvent_Instance_On_LevelLoaded;

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Networking")
		void OnLevelLoadedDelegate(FString LevelName);

	virtual void BeginDestroy() override;

	//Server Only
#if UE_SERVER
	bool bEnableSocketDebugMessage = true;
	TSharedPtr<IWebSocket>WebSocket;

	void RunServerCode();
	void SocketConnected();
	void SocketConnectionError(const FString& Error);
	void SocketClosed(int32 StatusCode, const FString& Reason, const bool bWasClean);
	void SocketMessageReceived(const FString& MessageString);
	void SocketMessageSent(const FString& MessageString);

	bool Get_Required_Commandline_Arguments();

	FString SerializeDataFor_Server_Match_Server_Verify_Request();
	FString SerializeDataFor_Server_Match_Server_Ready_Request(bool Is_Match_Ready);
	FString SerializeDataFor_Server_Match_Allow_User_Response(FString a_User_At_Name, FString a_User_Index_Key, FString a_Unique_Join_Key, FString a_Data);
#endif // UE_SERVER

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wrexa | Screen Capture")
		void On_New_Screen_Frame_Received(const TArray <uint8>& RGBA_Buffer, int Width, int Height);
};
