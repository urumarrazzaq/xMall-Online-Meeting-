// Code Done By Punal Manalan 2022

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CSSocketSubsystem.generated.h"

class IWebSocket;
class FJsonObject;
class FNFTs_Marketplace_ThreadWorker;

UENUM(BlueprintType)
enum class JSON_Common_Request_Types : uint8
{
	Invaild_Request = 0 UMETA(DisplayName = "Invaild_Request"),
	Client_User_Simple_Login_Request = 1 UMETA(DisplayName = "Client_User_Simple_Login_Request"),
	Client_User_Simple_Registration_Request = 2 UMETA(DisplayName = "Client_User_Simple_Registration_Request"),
	Client_Fetch_User_Full_Identification_Data_Request = 3 UMETA(DisplayName = "Client_Fetch_User_Full_Identification_Data_Request"),
	Client_Fetch_User_Main_Identification_Data_Request = 4 UMETA(DisplayName = "Client_Fetch_User_Main_Identification_Data_Request"),
	Client_Fetch_User_Partial_Identification_Data_Request = 5 UMETA(DisplayName = "Client_Fetch_User_Partial_Identification_Data_Request"),
	Client_Fetch_User_Social_Data_Request = 6 UMETA(DisplayName = "Client_Fetch_User_Social_Data_Request"),
	Client_Fetch_All_Ongoing_Match_Info_Request = 7 UMETA(DisplayName = "Client_Fetch_All_Ongoing_Match_Info_Request"),
	Client_Fetch_All_Specified_Info_Type_Request = 8 UMETA(DisplayName = "Client_Fetch_All_Specified_Info_Type_Request"),
	Client_Social_Command_Request = 9 UMETA(DisplayName = "Client_Social_Command_Request"),
	Client_Get_Tickets_Owned_Request = 10 UMETA(DisplayName = "Client_Get_Tickets_Owned_Request"),
	Client_Buy_Tickets_Request = 11 UMETA(DisplayName = "Client_Buy_Tickets_Request"),
	Client_Get_User_Appearance_Data_Request = 12 UMETA(DisplayName = "Client_Get_User_Appearance_Data_Request"),
	Client_Set_User_Appearance_Data_Request = 13 UMETA(DisplayName = "Client_Set_User_Appearance_Data_Request"),
	Client_Create_Match_Request = 14 UMETA(DisplayName = "Client_Create_Match_Request"),
	Client_Join_Match_Request = 15 UMETA(DisplayName = "Client_Join_Match_Request"),
	Server_Match_Server_Verify_Request = 16 UMETA(DisplayName = "Server_Match_Server_Verify_Request"),
	Server_Match_Server_Ready_Request = 17 UMETA(DisplayName = "Server_Match_Server_Ready_Request"),
	Server_Stop_Match_Request = 18 UMETA(DisplayName = "Server_Stop_Match_Request"),
	Server_Match_Allow_User_Request = 19 UMETA(DisplayName = "Server_Match_Allow_User_Request")
};



UENUM(BlueprintType)
enum class JSON_Common_Response_Types : uint8
{
	Invaild_Response = 0 UMETA(DisplayName = "Invaild_Response"),
	Client_User_Simple_Login_Response = 1 UMETA(DisplayName = "Client_User_Simple_Login_Response"),
	Client_User_Simple_Registration_Response = 2 UMETA(DisplayName = "Client_User_Simple_Registration_Response"),
	Client_Fetch_User_Full_Identification_Data_Response = 3 UMETA(DisplayName = "Client_Fetch_User_Full_Identification_Data_Response"),
	Client_Fetch_User_Main_Identification_Data_Response = 4 UMETA(DisplayName = "Client_Fetch_User_Main_Identification_Data_Response"),
	Client_Fetch_User_Partial_Identification_Data_Response = 5 UMETA(DisplayName = "Client_Fetch_User_Partial_Identification_Data_Response"),
	Client_Fetch_User_Social_Data_Response = 6 UMETA(DisplayName = "Client_Fetch_User_Social_Data_Response"),
	Client_Fetch_All_Ongoing_Match_Info_Response = 7 UMETA(DisplayName = "Client_Fetch_All_Ongoing_Match_Info_Response"),
	Client_Fetch_All_Specified_Info_Type_Response = 8 UMETA(DisplayName = "Client_Fetch_All_Specified_Info_Type_Response"),
	Client_Social_Command_Response = 9 UMETA(DisplayName = "Client_Social_Command_Response"),
	Client_Get_Tickets_Owned_Response = 10 UMETA(DisplayName = "Client_Get_Tickets_Owned_Response"),
	Client_Buy_Tickets_Response = 11 UMETA(DisplayName = "Client_Buy_Tickets_Response"),
	Client_Get_User_Appearance_Data_Response = 12 UMETA(DisplayName = "Client_Get_User_Appearance_Data_Response"),
	Client_Set_User_Appearance_Data_Response = 13 UMETA(DisplayName = "Client_Set_User_Appearance_Data_Response"),
	Client_Create_Match_Response = 14 UMETA(DisplayName = "Client_Create_Match_Response"),
	Client_Join_Match_Response = 15 UMETA(DisplayName = "Client_Join_Match_Response"),
	Server_Match_Server_Verify_Response = 16 UMETA(DisplayName = "Server_Match_Server_Verify_Response"),
	Server_Match_Server_Ready_Response = 17 UMETA(DisplayName = "Server_Match_Server_Ready_Response"),
	Server_Stop_Match_Response = 18 UMETA(DisplayName = "Server_Stop_Match_Response"),
	Server_Match_Allow_User_Response = 19 UMETA(DisplayName = "Server_Match_Allow_User_Response"),
};

UENUM(BlueprintType)
enum class JSON_Common_Social_Types : uint8
{
	Invalid_Social = 0 UMETA(DisplayName = "Invalid_Social"),
	Friends = 1 UMETA(DisplayName = "Friends"),
	Followers = 2 UMETA(DisplayName = "Followers"),
	Blocked = 3 UMETA(DisplayName = "Blocked"),
	Following = 4 UMETA(DisplayName = "Following"),
	Blocking = 5 UMETA(DisplayName = "Blocking"),
	Incoming_Friend_Request = 6 UMETA(DisplayName = "Incoming_Friend_Request"),
	Outgoing_Friend_Request = 7 UMETA(DisplayName = "Outgoing_Friend_Request"),
};

UENUM(BlueprintType)
enum class JSON_Common_Match_Types : uint8
{
	Invalid_Match_Type = 0 UMETA(DisplayName = "Invalid_Match_Type"),
	Ongoing_Match = 1 UMETA(DisplayName = "Ongoing_Match"),
};

UENUM(BlueprintType)
enum class JSON_Match_Flag_Types : uint8
{
	Invalid_Flag_Type = 0 UMETA(DisplayName = "Invalid_Flag_Type"),
	Public_Match = 1 UMETA(DisplayName = "Public_Match"),
	Private_Match = 2 UMETA(DisplayName = "Private_Match"),
};

UENUM(BlueprintType)
enum class JSON_Common_Social_Command_Types : uint8
{
	Invalid_Social_Command = 0 UMETA(DisplayName = "Invalid_Social_Command"),
	Friend = 1 UMETA(DisplayName = "Friend"),
	UnFriend = 2 UMETA(DisplayName = "UnFriend"),
	Follow = 3 UMETA(DisplayName = "Follow"),
	UnFollow = 4 UMETA(DisplayName = "UnFollow"),
	Block = 5 UMETA(DisplayName = "Block"),
	UnBlock = 6 UMETA(DisplayName = "UnBlock"),
};

UENUM(BlueprintType)
enum class JSON_Common_Fetch_Specified_Types : uint8
{
	Invalid_Fetch_Specified_Type = 0 UMETA(DisplayName = "Invalid_Fetch_Specified_Type"),
	User_Name = 1 UMETA(DisplayName = "User_Name"),
	Match = 2 UMETA(DisplayName = "Match"),
};

USTRUCT(BlueprintType)
struct FReceived_Socket_JSON
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Data_Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Data;
};

USTRUCT(BlueprintType)
struct FTicket_Info_JSON
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Ticket_ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Ticket_Owner;
};

USTRUCT(BlueprintType)
struct FTicket_Container_JSON
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Container_ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Container_Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray <FString> Ticket_Bag;
};

USTRUCT(BlueprintType)
struct FBalloon_Vote_JSON
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Balloon_ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Votes_Casted;
};

USTRUCT(BlueprintType)
struct FMatch_Details_Struct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Match_ID = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Index_Key = ""; // Also called Session ID Use stoull()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Host_UserName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Password = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Match_Name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Match_Description = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Match_Type = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Ticket_Cost = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString IP = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Port = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Participants; // User_At_Name = Username
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Match_Has_Started = false;
};

USTRUCT(BlueprintType)
struct FUser_Main_Identification_Data
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString User_At_Name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString User_Display_Name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString EMail = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString User_Full_Name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Phone_No = "";
};

USTRUCT(BlueprintType)
struct FUser_Partial_Identification_Data
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString User_At_Name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString User_Display_Name = "";
};

USTRUCT(BlueprintType)
struct FUser_Indentification_And_Player_Data
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUser_Main_Identification_Data Indentification_Data;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Last_Login_Date_Time = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Tickets_Owned = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Character_Customization_Data = "";
};

USTRUCT(BlueprintType)
struct FUser_Ticket_Owned_Data
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString User_At_Name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Tickets_Owned = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Data = "";
};

USTRUCT(BlueprintType)
struct FUser_Character_Customization_Data
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString User_At_Name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Character_Customization_Data = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Data = "";
};

USTRUCT(BlueprintType)
struct FMatch_User_Join_Data
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Is_New_Created_Match = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Match_ID = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString IP = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Port = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Unique_Join_Key = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Data = "";
};

/**
 *
 */
UCLASS()
class WREXA_API UCSSocketSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	bool bEnableSocketDebugMessage = true;

	TSharedPtr<IWebSocket>WebSocket;

	//TODO CHANGE LOCATION OF THESE VARIABLES
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Websocket Messenger")
		bool Is_Websocket_Connected = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wrexa | Character")
		FUser_Indentification_And_Player_Data Current_User_Details;

protected:

	void SocketConnected();
	void SocketConnectionError(const FString& Error);
	void SocketClosed(int32 StatusCode, const FString& Reason, const bool bWasClean);
	UFUNCTION(BlueprintCallable)//TODO REMOVE ONCE FININISED
	void SocketMessageReceived(const FString& MessageString);
	void SocketMessageSent(const FString& MessageString);

	FString SerializeData(const FString& Type, const FString& Action, const TMap<FString, FString>& Data) const;	
	
	UFUNCTION(BlueprintCallable)
		FString SerializeDataForSpecifiedType(JSON_Common_Response_Types DataType, FString Data);//TODO Add Many Types of Send Data
	
	UFUNCTION(BlueprintCallable)
		FString SerializeDataFor_Ticket_Bag_Request(FString Container_ID, FString Container_Level);//For Fetching Ticket Bag/Container

	UFUNCTION(BlueprintCallable)
		FString SerializeDataFor_Ticket_Info_Request(FString Ticket_ID);//For Fetching Ticket Info, Specifically The Owner ID/Name

public:
	FString JsonToString(const TSharedPtr<FJsonObject> JsonObject) const;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
		void Websocket_Connect();

	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
		void Websocket_Disconnect();

	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
		void SendMessage(const FString& MessageString);

	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
		void SendMessageUnpacked(const FString& Type, const FString& Action, const TMap<FString, FString>& Data);
	
	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
	FString JSON_Common_Request_Types_To_String(JSON_Common_Request_Types Data_Type);
	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
	FString JSON_Common_Response_Types_To_String(JSON_Common_Response_Types Data_Type);
	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
	FString JSON_Common_Social_Types_To_String(JSON_Common_Social_Types Data_Type);
	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
	FString JSON_Common_Match_Types_To_String(JSON_Common_Match_Types Data_Type);
	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
	FString JSON_Match_Flag_Types_To_String(JSON_Match_Flag_Types Data_Type);
	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
	FString JSON_Common_Social_Command_Types_To_String(JSON_Common_Social_Command_Types Data_Type);
	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
	FString JSON_Common_Fetch_Specified_Types_To_String(JSON_Common_Fetch_Specified_Types Data_Type);


	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
	JSON_Common_Request_Types String_To_JSON_Common_Request_Types(FString Input_String);
	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
	JSON_Common_Response_Types String_To_JSON_Common_Response_Types(FString Input_String);
	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
	JSON_Common_Social_Types String_To_JSON_Common_Social_Types(FString Input_String);
	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
	JSON_Common_Match_Types String_To_JSON_Common_Match_Types(FString Input_String);
	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
	JSON_Match_Flag_Types String_To_JSON_Match_Flag_Types(FString Input_String);
	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
	JSON_Common_Social_Command_Types String_To_JSON_Common_Social_Command_Types(FString Input_String);
	UFUNCTION(BlueprintCallable, Category = "Websocket Messenger")
	JSON_Common_Fetch_Specified_Types String_To_JSON_Common_Fetch_Specified_Types(FString Input_String);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSocketConnected);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSocketConnectionError, FString, Error);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSocketDataSent, FString, Message);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSocketDataReceived, const FReceived_Socket_JSON&, Message);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSocketClosed, int32, StatusCode, FString, Reason, bool, bWasClean);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRaffleGalaxyMultipurposeEnum_JSONReceive, JSON_Common_Response_Types, TheReceivedType, bool, OptionalBoolean, FString, OptionalReason);//When Ever This is Activated it means the Client Needs to Update Widget or Other Things

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMainIdentificationReceived, FUser_Indentification_And_Player_Data, User_Indentification_And_Player_Data_Struct);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserTicketOwned, FUser_Ticket_Owned_Data, User_Ticket_Owned_Data);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserCharacterCustomizationDataReceived, FUser_Character_Customization_Data, User_Character_Customization_Data);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMatchUserJoinDataReceived, FMatch_User_Join_Data, Match_User_Join_Data);

	UPROPERTY(BlueprintAssignable, Category = "Websocket Messenger")
		FSocketConnected OnSocketConnected;

	UPROPERTY(BlueprintAssignable, Category = "Websocket Messenger")
		FSocketConnectionError OnSocketConnectionError;

	UPROPERTY(BlueprintAssignable, Category = "Websocket Messenger")
		FSocketDataSent OnSocketMessageSent;

	UPROPERTY(BlueprintAssignable, Category = "Websocket Messenger")
		FSocketDataReceived OnSocketMessageReceived;

	UPROPERTY(BlueprintAssignable, Category = "Websocket Messenger")
		FSocketClosed OnSocketClosed;

	UPROPERTY(BlueprintAssignable, Category = "Websocket Messenger")
		FRaffleGalaxyMultipurposeEnum_JSONReceive OnMultipurposeEnum_JSONReceive;

	
	UPROPERTY(BlueprintAssignable, Category = "Websocket Messenger")
		FMainIdentificationReceived User_Indentification_And_Player_Data_JSONResponse;

	UPROPERTY(BlueprintAssignable, Category = "Websocket Messenger")
		FUserTicketOwned UserTicketOwned_JSONResponse;

	UPROPERTY(BlueprintAssignable, Category = "Websocket Messenger")
		FUserCharacterCustomizationDataReceived UserCharacterCustomizationDataReceived_JSONResponse;

	UPROPERTY(BlueprintAssignable, Category = "Websocket Messenger")
		FMatchUserJoinDataReceived MatchUserJoinDataReceived_JSONResponse;
};