// Code Done By Punal Manalan 2022


#include "CSSocketSubsystem.h"

#include "IWebSocket.h"
#include "WebSocketsModule.h"
#include "Wrexa_Instance.h"
//#include "Interfaces/ISslCertificateManager.h"
//#include "Interfaces/ISslManager.h"
//#include "Interfaces/ISslContext.h"
//#include "SslModule.h"
//#define UI UI_ST
//THIRD_PARTY_INCLUDES_START
//#include "openssl/evp.h"
//#include <openssl/ssl.h>
//THIRD_PARTY_INCLUDES_END
//#undef UI

/*void AddCertificateToSslContext(const FString& PEMFilePath, ISslContext& SslContext)
{
	TArray<uint8> PemData;
	FFileHelper::LoadFileToArray(PemData, *PEMFilePath);
	ISslCertificateManager& CertManager = FSslModule::Get().GetCertificateManager();
	CertManager.AddCertificatesToSslContext(PemData, SslContext);
}*/

void UCSSocketSubsystem::Websocket_Connect()
{
    if (!Is_Websocket_Connected)
    {
        UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
        if (World != nullptr)
        {
            if ((UWrexa_Instance*)(World->GetGameInstance()) != nullptr)
            {
                WebSocket = FWebSocketsModule::Get().CreateWebSocket(((UWrexa_Instance*)(World->GetGameInstance()))->Main_Server_Address, (((UWrexa_Instance*)(World->GetGameInstance()))->Is_Web_Socket_Https_CLIENT ? "wss" : "ws"));
            }
            else
            {
                WebSocket = FWebSocketsModule::Get().CreateWebSocket("wss://www.Example.org:443", (((UWrexa_Instance*)(World->GetGameInstance()))->Is_Web_Socket_Https_CLIENT ? "wss" : "ws"));
            }
        }
        else
        {
            WebSocket = FWebSocketsModule::Get().CreateWebSocket("wss://www.Example.org:443", (((UWrexa_Instance*)(World->GetGameInstance()))->Is_Web_Socket_Https_CLIENT ? "wss" : "ws"));
        }

        WebSocket->OnConnected().AddUObject(this, &UCSSocketSubsystem::SocketConnected);
        WebSocket->OnConnectionError().AddUObject(this, &UCSSocketSubsystem::SocketConnectionError);
        WebSocket->OnClosed().AddUObject(this, &UCSSocketSubsystem::SocketClosed);
        WebSocket->OnMessage().AddUObject(this, &UCSSocketSubsystem::SocketMessageReceived);
        WebSocket->OnMessageSent().AddUObject(this, &UCSSocketSubsystem::SocketMessageSent);

        WebSocket->Connect();
    }
    else
    {
        if (bEnableSocketDebugMessage)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Websocket Already Connected");
    }
}
void UCSSocketSubsystem::Websocket_Disconnect()
{
    if (WebSocket->IsConnected())
        WebSocket->Close();
}

void UCSSocketSubsystem::SocketConnected()
{
	if (bEnableSocketDebugMessage)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Successfully Connected");
    
    Is_Websocket_Connected = true;

	OnSocketConnected.Broadcast();
}

void UCSSocketSubsystem::SocketConnectionError(const FString& Error)
{
	if (bEnableSocketDebugMessage)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Error);

	OnSocketConnectionError.Broadcast(Error);
}

void UCSSocketSubsystem::SocketClosed(const int32 StatusCode, const FString& Reason, const bool bWasClean)
{
	if (bEnableSocketDebugMessage)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, bWasClean ? FColor::Green : FColor::Red, "Connection Closed " + Reason);

    if (bEnableSocketDebugMessage)
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, "Shutdown");

    OnSocketConnected.Clear();
    OnSocketConnectionError.Clear();
    OnSocketMessageSent.Clear();
    OnSocketMessageReceived.Clear();
    OnSocketClosed.Clear();
    if (WebSocket->IsConnected())
        WebSocket->Close();

    Is_Websocket_Connected = false;

	OnSocketClosed.Broadcast(StatusCode, Reason, bWasClean);
}

void UCSSocketSubsystem::SocketMessageReceived(const FString& MessageString)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(MessageString);
	//FString OutputString;
	//TSharedRef< TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	//FReceived_Socket_JSON TestMessage = {"asdasd","asdasdasd"};

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		FReceived_Socket_JSON OutPut = { "Data_Type","Data" };
		bool IsRequestSuccessful = true;
		const FString Type = JsonObject->GetStringField("Data_Type");
		const TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField("Data");
		TSharedPtr<FJsonObject> TempData = MakeShareable(new FJsonObject());
		OutPut.Data_Type = Type;
		OutPut.Data = JsonToString(Data);

		//TestMessage.Data_Type = JsonObject->GetStringField("Data_Type");
		//FJsonSerializer::Serialize(Data.ToSharedRef(), JsonWriter);
		//TestMessage.Data = OutputString;

		if (bEnableSocketDebugMessage)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "The Received Data Type: " + Type);

		switch (String_To_JSON_Common_Response_Types(Type))
		{
            case JSON_Common_Response_Types::Invaild_Response:
			{
				if (bEnableSocketDebugMessage)
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Invaild_Response Received");
				break;
			}

            case JSON_Common_Response_Types::Client_User_Simple_Login_Response:
            {
                if (bEnableSocketDebugMessage)
                    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Client_User_Simple_Login_Response");
                TempData = Data->GetObjectField("Is_Request_Successful");
                if (TempData->GetStringField("Is_Successful") == "False")
                {
                    IsRequestSuccessful = false;
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Request To Server Is Unsuccessful");
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "The Reason Is: " + TempData->GetStringField("Reason"));
                    OnMultipurposeEnum_JSONReceive.Broadcast(JSON_Common_Response_Types::Client_User_Simple_Login_Response, false, "");
                }
                else
                {
                    TempData = Data->GetObjectField("User_Main_Identification_Data");
                    Current_User_Details.Indentification_Data.User_At_Name = TempData->GetStringField("User_At_Name");
                    Current_User_Details.Indentification_Data.User_Display_Name = TempData->GetStringField("User_Display_Name");
                    Current_User_Details.Indentification_Data.EMail = TempData->GetStringField("EMail");
                    Current_User_Details.Indentification_Data.User_Full_Name = TempData->GetStringField("User_Full_Name");
                    Current_User_Details.Indentification_Data.Phone_No = TempData->GetStringField("Phone_No");

                    FString Extra_Data = Data->GetStringField("Data");

                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "User_At_Name: " + Current_User_Details.Indentification_Data.User_At_Name);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "User_Display_Name: " + Current_User_Details.Indentification_Data.User_Display_Name);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "EMail: " + Current_User_Details.Indentification_Data.EMail);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "User_Full_Name: " + Current_User_Details.Indentification_Data.User_Full_Name);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "User_Full_Name: " + Current_User_Details.Indentification_Data.Phone_No);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Extra_Data: " + Extra_Data);

                    OnMultipurposeEnum_JSONReceive.Broadcast(JSON_Common_Response_Types::Client_User_Simple_Login_Response, true, "");
                }
                break;
            }

            case JSON_Common_Response_Types::Client_User_Simple_Registration_Response:
            {
                if (bEnableSocketDebugMessage)
                    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Client_User_Simple_Registration_Response");
                TempData = Data->GetObjectField("Is_Request_Successful");
                if (TempData->GetStringField("Is_Successful") == "False")
                {
                    IsRequestSuccessful = false;
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Request To Server Is Unsuccessful");
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "The Reason Is: " + TempData->GetStringField("Reason"));
                    OnMultipurposeEnum_JSONReceive.Broadcast(JSON_Common_Response_Types::Client_User_Simple_Registration_Response, false, "");
                }
                else
                {
                    FString Extra_Data = Data->GetStringField("Data");
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Extra_Data: " + Extra_Data);

                    OnMultipurposeEnum_JSONReceive.Broadcast(JSON_Common_Response_Types::Client_User_Simple_Registration_Response, true, "");
                }
                break;
            }

            case JSON_Common_Response_Types::Client_Fetch_User_Main_Identification_Data_Response:
            {
                if (bEnableSocketDebugMessage)
                    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Client_Fetch_User_Main_Identification_Data_Response");
                TempData = Data->GetObjectField("Is_Request_Successful");
                if (TempData->GetStringField("Is_Successful") == "False")
                {
                    IsRequestSuccessful = false;
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Request To Server Is Unsuccessful");
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "The Reason Is: " + TempData->GetStringField("Reason"));
                    OnMultipurposeEnum_JSONReceive.Broadcast(JSON_Common_Response_Types::Client_Fetch_User_Main_Identification_Data_Response, false, "");
                }
                else
                {
                    FUser_Indentification_And_Player_Data Temp_Received_Data_Struct;

                    TempData = Data->GetObjectField("User_Main_Identification_Data");
                    Temp_Received_Data_Struct.Indentification_Data.User_At_Name = TempData->GetStringField("User_At_Name");
                    Temp_Received_Data_Struct.Indentification_Data.User_Display_Name = TempData->GetStringField("User_Display_Name");
                    Temp_Received_Data_Struct.Indentification_Data.EMail = TempData->GetStringField("EMail");
                    Temp_Received_Data_Struct.Indentification_Data.User_Full_Name = TempData->GetStringField("User_Full_Name");
                    Temp_Received_Data_Struct.Indentification_Data.Phone_No = TempData->GetStringField("Phone_No");
                    
                    Temp_Received_Data_Struct.Last_Login_Date_Time = Data->GetStringField("Last_Login_Date_Time");

                    FString Extra_Data = Data->GetStringField("Data");

                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "User_At_Name: " + Temp_Received_Data_Struct.Indentification_Data.User_At_Name);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "User_Display_Name: " + Temp_Received_Data_Struct.Indentification_Data.User_Display_Name);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "EMail: " + Temp_Received_Data_Struct.Indentification_Data.EMail);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "User_Full_Name: " + Temp_Received_Data_Struct.Indentification_Data.User_Full_Name);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "User_Full_Name: " + Temp_Received_Data_Struct.Indentification_Data.Phone_No);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Last_Login_Date_Time: " + Temp_Received_Data_Struct.Last_Login_Date_Time);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Extra_Data: " + Extra_Data);

                    User_Indentification_And_Player_Data_JSONResponse.Broadcast(Temp_Received_Data_Struct);
                }
                break;
            }

            case JSON_Common_Response_Types::Client_Get_Tickets_Owned_Response:
            {
                if (bEnableSocketDebugMessage)
                    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Client_Get_Tickets_Owned_Response");
                TempData = Data->GetObjectField("Is_Request_Successful");
                if (TempData->GetStringField("Is_Successful") == "False")
                {
                    IsRequestSuccessful = false;
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Request To Server Is Unsuccessful");
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "The Reason Is: " + TempData->GetStringField("Reason"));
                    OnMultipurposeEnum_JSONReceive.Broadcast(JSON_Common_Response_Types::Client_Get_Tickets_Owned_Response, false, "");
                }
                else
                {
                    FUser_Ticket_Owned_Data Temp_Received_Data_Struct;

                    Temp_Received_Data_Struct.User_At_Name = Data->GetStringField("User_At_Name");
                    Temp_Received_Data_Struct.Tickets_Owned = Data->GetStringField("Tickets_Owned");
                    Temp_Received_Data_Struct.Data = Data->GetStringField("Data");
                    
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "User_At_Name: " + Temp_Received_Data_Struct.User_At_Name);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Tickets_Owned: " + Temp_Received_Data_Struct.Tickets_Owned);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Extra_Data: " + Temp_Received_Data_Struct.Data);

                    UserTicketOwned_JSONResponse.Broadcast(Temp_Received_Data_Struct);
                }
                break;
            }

            case JSON_Common_Response_Types::Client_Buy_Tickets_Response:
            {
                if (bEnableSocketDebugMessage)
                    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Client_Buy_Tickets_Response");
                TempData = Data->GetObjectField("Is_Request_Successful");
                if (TempData->GetStringField("Is_Successful") == "False")
                {
                    IsRequestSuccessful = false;
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Request To Server Is Unsuccessful");
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "The Reason Is: " + TempData->GetStringField("Reason"));
                    OnMultipurposeEnum_JSONReceive.Broadcast(JSON_Common_Response_Types::Client_Buy_Tickets_Response, false, "");
                }
                else
                {
                    FUser_Ticket_Owned_Data Temp_Received_Data_Struct;

                    Temp_Received_Data_Struct.User_At_Name = Data->GetStringField("User_At_Name");
                    Temp_Received_Data_Struct.Tickets_Owned = Data->GetStringField("Tickets_Owned");
                    Temp_Received_Data_Struct.Data = Data->GetStringField("Data");

                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "User_At_Name: " + Temp_Received_Data_Struct.User_At_Name);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Tickets_Owned: " + Temp_Received_Data_Struct.Tickets_Owned);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Extra_Data: " + Temp_Received_Data_Struct.Data);

                    UserTicketOwned_JSONResponse.Broadcast(Temp_Received_Data_Struct);
                }
                break;
            }

            case JSON_Common_Response_Types::Client_Get_User_Appearance_Data_Response:
            {
                if (bEnableSocketDebugMessage)
                    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Client_Get_User_Appearance_Data_Response");
                TempData = Data->GetObjectField("Is_Request_Successful");
                if (TempData->GetStringField("Is_Successful") == "False")
                {
                    IsRequestSuccessful = false;
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Request To Server Is Unsuccessful");
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "The Reason Is: " + TempData->GetStringField("Reason"));
                    OnMultipurposeEnum_JSONReceive.Broadcast(JSON_Common_Response_Types::Client_Get_User_Appearance_Data_Response, false, "");
                }
                else
                {
                    FUser_Character_Customization_Data Temp_Received_Data_Struct;

                    Temp_Received_Data_Struct.User_At_Name = Data->GetStringField("User_At_Name");
                    Temp_Received_Data_Struct.Character_Customization_Data = Data->GetStringField("User_Apperance_Data_Base_64");
                    Temp_Received_Data_Struct.Data = Data->GetStringField("Data");

                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "User_At_Name: " + Temp_Received_Data_Struct.User_At_Name);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "User_Apperance_Data_Base_64: " + Temp_Received_Data_Struct.Character_Customization_Data);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Extra_Data: " + Temp_Received_Data_Struct.Data);

                    UserCharacterCustomizationDataReceived_JSONResponse.Broadcast(Temp_Received_Data_Struct);
                }
                break;
            }

            case JSON_Common_Response_Types::Client_Set_User_Appearance_Data_Response:
            {
                if (bEnableSocketDebugMessage)
                    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Client_Set_User_Appearance_Data_Response");
                TempData = Data->GetObjectField("Is_Request_Successful");
                if (TempData->GetStringField("Is_Successful") == "False")
                {
                    IsRequestSuccessful = false;
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Request To Server Is Unsuccessful");
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "The Reason Is: " + TempData->GetStringField("Reason"));
                    OnMultipurposeEnum_JSONReceive.Broadcast(JSON_Common_Response_Types::Client_Set_User_Appearance_Data_Response, false, "");
                }
                else
                {
                    FString Extra_Data = Data->GetStringField("Data");
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Extra_Data: " + Extra_Data);

                    OnMultipurposeEnum_JSONReceive.Broadcast(JSON_Common_Response_Types::Client_Set_User_Appearance_Data_Response, true, "");
                }
                break;
            }

            case JSON_Common_Response_Types::Client_Create_Match_Response:
            {
                if (bEnableSocketDebugMessage)
                    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Client_Create_Match_Response");
                TempData = Data->GetObjectField("Is_Request_Successful");
                if (TempData->GetStringField("Is_Successful") == "False")
                {
                    IsRequestSuccessful = false;
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Request To Server Is Unsuccessful");
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "The Reason Is: " + TempData->GetStringField("Reason"));
                    OnMultipurposeEnum_JSONReceive.Broadcast(JSON_Common_Response_Types::Client_Create_Match_Response, false, "");
                }
                else
                {
                    FMatch_User_Join_Data Temp_Received_Data_Struct;

                    Temp_Received_Data_Struct.Is_New_Created_Match = true;
                    Temp_Received_Data_Struct.Match_ID = Data->GetStringField("Match_ID");
                    Temp_Received_Data_Struct.IP = Data->GetStringField("IP");
                    Temp_Received_Data_Struct.Port = Data->GetStringField("Port");
                    Temp_Received_Data_Struct.Unique_Join_Key = "NULL";// For the Host to Join, the Host Must Send Request to Join
                    Temp_Received_Data_Struct.Data = Data->GetStringField("Data");

                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Match_ID: " + Temp_Received_Data_Struct.Match_ID);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "IP: " + Temp_Received_Data_Struct.IP);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Port: " + Temp_Received_Data_Struct.Port);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Unique_Join_Key: " + Temp_Received_Data_Struct.Unique_Join_Key);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Extra_Data: " + Temp_Received_Data_Struct.Data);

                    MatchUserJoinDataReceived_JSONResponse.Broadcast(Temp_Received_Data_Struct);
                }
                break;
            }

            case JSON_Common_Response_Types::Client_Join_Match_Response:
            {
                if (bEnableSocketDebugMessage)
                    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Client_Join_Match_Response");
                TempData = Data->GetObjectField("Is_Request_Successful");
                if (TempData->GetStringField("Is_Successful") == "False")
                {
                    IsRequestSuccessful = false;
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Request To Server Is Unsuccessful");
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "The Reason Is: " + TempData->GetStringField("Reason"));
                    OnMultipurposeEnum_JSONReceive.Broadcast(JSON_Common_Response_Types::Client_Join_Match_Response, false, "");
                }
                else
                {
                    FMatch_User_Join_Data Temp_Received_Data_Struct;

                    Temp_Received_Data_Struct.Is_New_Created_Match = false;
                    Temp_Received_Data_Struct.Match_ID = Data->GetStringField("Match_ID");
                    Temp_Received_Data_Struct.IP = Data->GetStringField("IP");
                    Temp_Received_Data_Struct.Port = Data->GetStringField("Port");
                    Temp_Received_Data_Struct.Unique_Join_Key = Data->GetStringField("Unique_Join_Key");
                    Temp_Received_Data_Struct.Data = Data->GetStringField("Data");

                    UE_LOG(LogTemp, Log, TEXT("Match_ID: %s"), *(Temp_Received_Data_Struct.Match_ID));
                    UE_LOG(LogTemp, Log, TEXT("IP: %s"), *(Temp_Received_Data_Struct.IP));
                    UE_LOG(LogTemp, Log, TEXT("Port: %s"), *(Temp_Received_Data_Struct.Port));
                    UE_LOG(LogTemp, Log, TEXT("Unique_Join_Key %s"), *(Temp_Received_Data_Struct.Unique_Join_Key));
                    UE_LOG(LogTemp, Log, TEXT("Extra_Data %s"), *(Temp_Received_Data_Struct.Data));

                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Match_ID: " + Temp_Received_Data_Struct.Match_ID);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "IP: " + Temp_Received_Data_Struct.IP);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Port: " + Temp_Received_Data_Struct.Port);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Unique_Join_Key: " + Temp_Received_Data_Struct.Unique_Join_Key);
                    if (bEnableSocketDebugMessage)
                        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Extra_Data: " + Temp_Received_Data_Struct.Data);

                    MatchUserJoinDataReceived_JSONResponse.Broadcast(Temp_Received_Data_Struct);
                }
                break;
            }

			default:
			{
				if (bEnableSocketDebugMessage)
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Invaild_Response Received");
				break;
			}
		}
		
		if (bEnableSocketDebugMessage)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, OutPut.Data);

		OnSocketMessageReceived.Broadcast(OutPut);
	}
	else
	{
		if (bEnableSocketDebugMessage)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Failed to deserialize json message");
	}
}

void UCSSocketSubsystem::SocketMessageSent(const FString& MessageString)
{
	if (bEnableSocketDebugMessage)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, MessageString);
}

void UCSSocketSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	bEnableSocketDebugMessage = true;
	if (bEnableSocketDebugMessage)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "RaffleGalaxy Socket Initializing");
#if !UE_SERVER

	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
		FModuleManager::Get().LoadModule("WebSockets");

	if (!FModuleManager::Get().IsModuleLoaded("Json"))
		FModuleManager::Get().LoadModule("Json");

	if (!FModuleManager::Get().IsModuleLoaded("JsonUtilities"))
		FModuleManager::Get().LoadModule("JsonUtilities");

	//if (!FModuleManager::Get().IsModuleLoaded("SSL"))
		//FModuleManager::Get().LoadModule("SSL");

	//AddCertificatesToSslContext;
	//ISslCertificateManager::AddCertificatesToSslContext();
	//FSslModule::GetCertificateManager()
	//SSL_CTX;
	


	/*// Load the .pem file into a TArray<uint8>
	TArray<uint8> PemData;
	//FString PEMFilePath_S = FPaths::ProjectContentDir() + "Certificates/Wrexa.pem";
	//FString PEMFilePath_S = "E:/Wrexa_Repo/Wrexa_Sandbox/Content/Certificates/Wrexa.pem";// Working
	//FString PEMFilePath_S = "Game/Certificates/Wrexa.pem";//
	//FString PEMFilePath_S = "World'/Game/Wrexa/Maps/Test_Map_Punal.Test_Map_Punal'";//Not Working as well
	//FString PEMFilePath_S = FPaths::ProjectContentDir() + "Wrexa.uproject";
	//FString PEMFilePath_S = "E:/Wrexa_Repo/Wrexa_Sandbox/Wrexa.uproject";// Working
	//FString PEMFilePath_S = FPaths::ProjectContentDir() + "../Wrexa.uproject";// Working
	//FString PEMFilePath_S = FPaths::ProjectContentDir() + "Artboard_1_4x-100.uasset";// Working
	FString PEMFilePath_S = FPaths::ProjectContentDir() + "Certificates/Wrexa.pem";
	const TCHAR* PEMFilePath = *PEMFilePath_S;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, PEMFilePath_S);
	//FFileHelper::LoadFileToArray(PemData, PEMFilePath);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FFileHelper::LoadFileToArray(PemData, PEMFilePath)?"File Exists":"FILE DOES NOT EXIST");
	FSslContextCreateOptions SSLCreate_Options;
	SSLCreate_Options.MinimumProtocol = ESslTlsProtocol::TLSv1_2;
	SSLCreate_Options.MaximumProtocol = ESslTlsProtocol::Maximum;
	//Context->AddCertificate(PemData);
	try
	{
		SSL_CTX* ptr = FSslModule::Get().GetSslManager().CreateSslContext(SSLCreate_Options);
		if (ptr == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,"Failed to create SSL context");
			//return;
		}
		else
		{
			FSslModule::Get().GetCertificateManager().AddCertificatesToSslContext(ptr);
		}
	}
	catch (...)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SSL_CTX ERROR");
	}*/
//#ifdef WITH_SSL
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "SSL Exists");
//#else
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SSL_DOES NOT EXIST");
//#endif

	//WebSocket = FWebSocketsModule::Get().CreateWebSocket("ws://www.wrexarepo.org:80", "ws");

    Websocket_Connect();

	//WebSocket = FWebSocketsModule::Get().CreateWebSocket("wss://www.wrexarepo.org");
	//WebSocket = FWebSocketsModule::Get().CreateWebSocket("wss://socketsbay.com/wss/v2/1/demo/", "wss");// This is working	
	
#endif // !UE_SERVER
}

void UCSSocketSubsystem::Deinitialize()
{
#if !UE_SERVER
	OnSocketConnected.Clear();
	OnSocketConnectionError.Clear();
	OnSocketMessageSent.Clear();
	OnSocketMessageReceived.Clear();
	OnSocketClosed.Clear();
	OnMultipurposeEnum_JSONReceive.Clear();
    User_Indentification_And_Player_Data_JSONResponse.Clear();
    UserTicketOwned_JSONResponse.Clear();
    UserCharacterCustomizationDataReceived_JSONResponse.Clear();
    MatchUserJoinDataReceived_JSONResponse.Clear();

	if (bEnableSocketDebugMessage)
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, "Shutdown");

	if (WebSocket->IsConnected())
		WebSocket->Close();
#endif // !UE_SERVER
}

void UCSSocketSubsystem::SendMessage(const FString& MessageString)
{
	if (WebSocket->IsConnected())
		WebSocket->Send(MessageString);
}

void UCSSocketSubsystem::SendMessageUnpacked(const FString& Type, const FString& Action, const TMap<FString, FString>& Data)
{
	SendMessage(SerializeData(Type, Action, Data));
}

FString UCSSocketSubsystem::SerializeData(const FString& Type, const FString& Action,
	const TMap<FString, FString>& Data) const
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	
	JsonObject->SetStringField("type", Type);
	JsonObject->SetStringField("action", Action);

	const TSharedPtr<FJsonObject> DataJsonObject = MakeShareable(new FJsonObject);
	for (const TPair<FString, FString>& Pair : Data)
		DataJsonObject->SetStringField(Pair.Key, Pair.Value);

	JsonObject->SetObjectField("data", DataJsonObject);

	return JsonToString(JsonObject);
}

FString UCSSocketSubsystem::JsonToString(const TSharedPtr<FJsonObject> JsonObject) const
{
	FString JsonString;
	const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter))
		return JsonString;
	return "";
}

//TODO make Serializable Data For Each Type
FString UCSSocketSubsystem::SerializeDataForSpecifiedType(JSON_Common_Response_Types DataType, FString Data)
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> TempJsonObject = MakeShareable(new FJsonObject);
	/*switch (DataType)
	{
		//case Socket_Send_DataType::Client_Fetch_Inventory_Request:
		//{
		//	JsonObject->SetStringField("Data_Type", "Client_Fetch_Inventory_Request");
		//	TempJsonObject->SetStringField("Data", "None");
		//
		//	JsonObject->SetObjectField("Data", TempJsonObject);
		//	break;
		//}
		//
		//case Socket_Send_DataType::Client_Fetch_All_Ongoing_Match_Info_Request:
		//{
		//	JsonObject->SetStringField("Data_Type", "Client_Fetch_All_Ongoing_Match_Info_Request");
		//	TempJsonObject->SetStringField("Data", "None");
		//
		//	JsonObject->SetObjectField("Data", TempJsonObject);
		//	break;
		//}
		//
		//case Socket_Send_DataType::Fetch_All_Quest_Request:
		//{
		//	JsonObject->SetStringField("Data_Type", "Fetch_All_Quest_Request");
		//	TempJsonObject->SetStringField("Data", "None");
		//
		//	JsonObject->SetObjectField("Data", TempJsonObject);
		//	break;
		//}

		default:
		{
			if (bEnableSocketDebugMessage)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, "Unknown Type Send");
			break;
		}
	}*/
	return JsonToString(JsonObject);
}

FString UCSSocketSubsystem::SerializeDataFor_Ticket_Bag_Request(FString Container_ID, FString Container_Level)
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> TempJsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("Data_Type", "Client_Ticket_Bag_Request");
	TempJsonObject->SetStringField("Container_ID", Container_ID);
	TempJsonObject->SetStringField("Container_Level", Container_Level);

	JsonObject->SetObjectField("Data", TempJsonObject);
	return JsonToString(JsonObject);
}

FString UCSSocketSubsystem::SerializeDataFor_Ticket_Info_Request(FString Ticket_ID)
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> TempJsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("Data_Type", "Client_Ticket_Info_Request");
	TempJsonObject->SetStringField("Ticket_ID", Ticket_ID);

	JsonObject->SetObjectField("Data", TempJsonObject);
	return JsonToString(JsonObject);
}

// Common Enum FUNCTIONS
// Start
//****************************************************
///====================================================
//// ~String_To_Enum~ Start~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FString UCSSocketSubsystem::JSON_Common_Request_Types_To_String(JSON_Common_Request_Types Data_Type)
{
    switch (Data_Type)
    {
    case JSON_Common_Request_Types::Client_User_Simple_Login_Request:
    {
        return "Client_User_Simple_Login_Request";
        break;
    }
    case JSON_Common_Request_Types::Client_User_Simple_Registration_Request:
    {
        return "Client_User_Simple_Registration_Request";
        break;
    }
    case JSON_Common_Request_Types::Client_Fetch_User_Full_Identification_Data_Request:
    {
        return "Client_Fetch_User_Full_Identification_Data_Request";
        break;
    }
    case JSON_Common_Request_Types::Client_Fetch_User_Main_Identification_Data_Request:
    {
        return "Client_Fetch_User_Main_Identification_Data_Request";
        break;
    }
    case JSON_Common_Request_Types::Client_Fetch_User_Partial_Identification_Data_Request:
    {
        return "Client_Fetch_User_Partial_Identification_Data_Request";
        break;
    }
    case JSON_Common_Request_Types::Client_Fetch_User_Social_Data_Request:
    {
        return "Client_Fetch_User_Social_Data_Request";
        break;
    }
    case JSON_Common_Request_Types::Client_Fetch_All_Ongoing_Match_Info_Request:
    {
        return "Client_Fetch_All_Ongoing_Match_Info_Request";
        break;
    }
    case JSON_Common_Request_Types::Client_Fetch_All_Specified_Info_Type_Request:
    {
        return "Client_Fetch_All_Specified_Info_Type_Request";
        break;
    }
    case JSON_Common_Request_Types::Client_Social_Command_Request:
    {
        return "Client_Social_Command_Request";
        break;
    }
    case JSON_Common_Request_Types::Client_Get_Tickets_Owned_Request:
    {
        return "Client_Get_Tickets_Owned_Request";
        break;
    }
    case JSON_Common_Request_Types::Client_Buy_Tickets_Request:
    {
        return "Client_Buy_Tickets_Request";
        break;
    }
    case JSON_Common_Request_Types::Client_Get_User_Appearance_Data_Request:
    {
        return "Client_Get_User_Appearance_Data_Request";
        break;
    }
    case JSON_Common_Request_Types::Client_Set_User_Appearance_Data_Request:
    {
        return "Client_Set_User_Appearance_Data_Request";
        break;
    }
    case JSON_Common_Request_Types::Client_Create_Match_Request:
    {
        return "Client_Create_Match_Request";
        break;
    }
    case JSON_Common_Request_Types::Client_Join_Match_Request:
    {
        return "Client_Join_Match_Request";
        break;
    }
    case JSON_Common_Request_Types::Server_Match_Server_Verify_Request:
    {
        return "Server_Match_Server_Verify_Request";
        break;
    }
    case JSON_Common_Request_Types::Server_Match_Server_Ready_Request:
    {
        return "Server_Match_Server_Ready_Request";
        break;
    }
    case JSON_Common_Request_Types::Server_Stop_Match_Request:
    {
        return "Server_Stop_Match_Request";
        break;
    }
    case JSON_Common_Request_Types::Server_Match_Allow_User_Request:
    {
        return "Server_Match_Allow_User_Request";
        break;
    }

    case JSON_Common_Request_Types::Invaild_Request:
    {
        return "Invaild_Request";
        break;
    }
    default:
    {
        return "Invaild_Request";
        break;
    }
    }
}
FString UCSSocketSubsystem::JSON_Common_Response_Types_To_String(JSON_Common_Response_Types Data_Type)
{
    switch (Data_Type)
    {
    case JSON_Common_Response_Types::Client_User_Simple_Login_Response:
    {
        return "Client_User_Simple_Login_Response";
        break;
    }
    case JSON_Common_Response_Types::Client_User_Simple_Registration_Response:
    {
        return "Client_User_Simple_Registration_Response";
        break;
    }
    case JSON_Common_Response_Types::Client_Fetch_User_Full_Identification_Data_Response:
    {
        return "Client_Fetch_User_Full_Identification_Data_Response";
        break;
    }
    case JSON_Common_Response_Types::Client_Fetch_User_Main_Identification_Data_Response:
    {
        return "Client_Fetch_User_Main_Identification_Data_Response";
        break;
    }
    case JSON_Common_Response_Types::Client_Fetch_User_Partial_Identification_Data_Response:
    {
        return "Client_Fetch_User_Partial_Identification_Data_Response";
        break;
    }
    case JSON_Common_Response_Types::Client_Fetch_User_Social_Data_Response:
    {
        return "Client_Fetch_User_Social_Data_Response";
        break;
    }
    case JSON_Common_Response_Types::Client_Fetch_All_Ongoing_Match_Info_Response:
    {
        return "Client_Fetch_All_Ongoing_Match_Info_Response";
        break;
    }
    case JSON_Common_Response_Types::Client_Fetch_All_Specified_Info_Type_Response:
    {
        return "Client_Fetch_All_Specified_Info_Type_Response";
        break;
    }
    case JSON_Common_Response_Types::Client_Social_Command_Response:
    {
        return "Client_Social_Command_Response";
        break;
    }
    case JSON_Common_Response_Types::Client_Get_Tickets_Owned_Response:
    {
        return "Client_Get_Tickets_Owned_Response";
        break;
    }
    case JSON_Common_Response_Types::Client_Buy_Tickets_Response:
    {
        return "Client_Buy_Tickets_Response";
        break;
    }
    case JSON_Common_Response_Types::Client_Get_User_Appearance_Data_Response:
    {
        return "Client_Get_User_Appearance_Data_Response";
        break;
    }
    case JSON_Common_Response_Types::Client_Set_User_Appearance_Data_Response:
    {
        return "Client_Set_User_Appearance_Data_Response";
        break;
    }
    case JSON_Common_Response_Types::Client_Create_Match_Response:
    {
        return "Client_Create_Match_Response";
        break;
    }
    case JSON_Common_Response_Types::Client_Join_Match_Response:
    {
        return "Client_Join_Match_Response";
        break;
    }
    case JSON_Common_Response_Types::Server_Match_Server_Verify_Response:
    {
        return "Server_Match_Server_Verify_Response";
        break;
    }
    case JSON_Common_Response_Types::Server_Match_Server_Ready_Response:
    {
        return "Server_Match_Server_Ready_Response";
        break;
    }
    case JSON_Common_Response_Types::Server_Stop_Match_Response:
    {
        return "Server_Stop_Match_Response";
        break;
    }
    case JSON_Common_Response_Types::Server_Match_Allow_User_Response:
    {
        return "Server_Match_Allow_User_Response";
        break;
    }

    case JSON_Common_Response_Types::Invaild_Response:
    {
        return "Invaild_Response";
        break;
    }
    default:
    {
        return "Invaild_Response";
        break;
    }
    }
}
FString UCSSocketSubsystem::JSON_Common_Social_Types_To_String(JSON_Common_Social_Types Data_Type)
{
    switch (Data_Type)
    {
    case JSON_Common_Social_Types::Friends:
    {
        return "Friends";
        break;
    }
    case JSON_Common_Social_Types::Followers:
    {
        return "Followers";
        break;
    }
    case JSON_Common_Social_Types::Blocked:
    {
        return "Blocked";
        break;
    }
    case JSON_Common_Social_Types::Following:
    {
        return "Following";
        break;
    }
    case JSON_Common_Social_Types::Blocking:
    {
        return "Blocking";
        break;
    }
    case JSON_Common_Social_Types::Incoming_Friend_Request:
    {
        return "Incoming_Friend_Request";
        break;
    }
    case JSON_Common_Social_Types::Outgoing_Friend_Request:
    {
        return "Outgoing_Friend_Request";
        break;
    }

    case JSON_Common_Social_Types::Invalid_Social:
    {
        return "Invalid_Social";
        break;
    }
    default:
    {
        return "Invalid_Social";
        break;
    }
    }
}
FString UCSSocketSubsystem::JSON_Common_Match_Types_To_String(JSON_Common_Match_Types Data_Type)
{
    switch (Data_Type)
    {
    case JSON_Common_Match_Types::Ongoing_Match:
    {
        return "Ongoing_Match";
        break;
    }
    case JSON_Common_Match_Types::Invalid_Match_Type:
    {
        return "Invalid_Match_Type";
        break;
    }
    default:
    {
        return "Invalid_Match_Type";
        break;
    }
    }
}
FString UCSSocketSubsystem::JSON_Match_Flag_Types_To_String(JSON_Match_Flag_Types Data_Type)
{
    switch (Data_Type)
    {
    case JSON_Match_Flag_Types::Public_Match:
    {
        return "Public_Match";
        break;
    }
    case JSON_Match_Flag_Types::Private_Match:
    {
        return "Private_Match";
        break;
    }
    case JSON_Match_Flag_Types::Invalid_Flag_Type:
    {
        return "Invalid_Flag_Type";
        break;
    }
    default:
    {
        return "Invalid_Flag_Type";
        break;
    }
    }
}
FString UCSSocketSubsystem::JSON_Common_Social_Command_Types_To_String(JSON_Common_Social_Command_Types Data_Type)
{
    switch (Data_Type)
    {
    case JSON_Common_Social_Command_Types::Friend:
    {
        return "Friend";
        break;
    }
    case JSON_Common_Social_Command_Types::UnFriend:
    {
        return "UnFriend";
        break;
    }
    case JSON_Common_Social_Command_Types::Follow:
    {
        return "Follow";
        break;
    }
    case JSON_Common_Social_Command_Types::UnFollow:
    {
        return "UnFollow";
        break;
    }
    case JSON_Common_Social_Command_Types::Block:
    {
        return "Block";
        break;
    }
    case JSON_Common_Social_Command_Types::UnBlock:
    {
        return "UnBlock";
        break;
    }
    case JSON_Common_Social_Command_Types::Invalid_Social_Command:
    {
        return "Invalid_Social_Command";
        break;
    }
    default:
    {
        return "Invalid_Social_Command";
        break;
    }
    }
}
FString UCSSocketSubsystem::JSON_Common_Fetch_Specified_Types_To_String(JSON_Common_Fetch_Specified_Types Data_Type)
{
    switch (Data_Type)
    {
    case JSON_Common_Fetch_Specified_Types::User_Name:
    {
        return "User_Name";
        break;
    }
    case JSON_Common_Fetch_Specified_Types::Match:
    {
        return "Match";
        break;
    }
    case JSON_Common_Fetch_Specified_Types::Invalid_Fetch_Specified_Type:
    {
        return "Invalid_Fetch_Specified_Type";
        break;
    }
    default:
    {
        return "Invalid_Fetch_Specified_Type";
        break;
    }
    }
}
//// ~~String_To_Enum~ End~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///====================================================

///====================================================
// ~~Enum_To_String~ Start~~~~~~~~~~~~~~~~~~~~~~~~~~~~

JSON_Common_Request_Types UCSSocketSubsystem::String_To_JSON_Common_Request_Types(FString Input_String)
{
    if (Input_String == "Client_User_Simple_Login_Request")
    {
        return JSON_Common_Request_Types::Client_User_Simple_Login_Request;
    }
    else if (Input_String == "Client_User_Simple_Registration_Request")
    {
        return JSON_Common_Request_Types::Client_User_Simple_Registration_Request;
    }
    else if (Input_String == "Client_Fetch_User_Full_Identification_Data_Request")
    {
        return JSON_Common_Request_Types::Client_Fetch_User_Main_Identification_Data_Request;
    }
    else if (Input_String == "Client_Fetch_User_Main_Identification_Data_Request")
    {
        return JSON_Common_Request_Types::Client_Fetch_User_Full_Identification_Data_Request;
    }
    else if (Input_String == "Client_Fetch_User_Partial_Identification_Data_Request")
    {
        return JSON_Common_Request_Types::Client_Fetch_User_Partial_Identification_Data_Request;
    }
    else if (Input_String == "Client_Fetch_User_Social_Data_Request")
    {
        return JSON_Common_Request_Types::Client_Fetch_User_Social_Data_Request;
    }
    else if (Input_String == "Client_Fetch_All_Ongoing_Match_Info_Request")
    {
        return JSON_Common_Request_Types::Client_Fetch_All_Ongoing_Match_Info_Request;
    }
    else if (Input_String == "Client_Fetch_All_Specified_Info_Type_Request")
    {
        return JSON_Common_Request_Types::Client_Fetch_All_Specified_Info_Type_Request;
    }
    else if (Input_String == "Client_Social_Command_Request")
    {
        return JSON_Common_Request_Types::Client_Social_Command_Request;
    }
    else if (Input_String == "Client_Get_Tickets_Owned_Request")
    {
        return JSON_Common_Request_Types::Client_Get_Tickets_Owned_Request;
    }
    else if (Input_String == "Client_Buy_Tickets_Request")
    {
        return JSON_Common_Request_Types::Client_Buy_Tickets_Request;
    }
    else if (Input_String == "Client_Get_User_Appearance_Data_Request")
    {
        return JSON_Common_Request_Types::Client_Get_User_Appearance_Data_Request;
    }
    else if (Input_String == "Client_Set_User_Appearance_Data_Request")
    {
        return JSON_Common_Request_Types::Client_Set_User_Appearance_Data_Request;
    }
    else if (Input_String == "Client_Create_Match_Request")
    {
        return JSON_Common_Request_Types::Client_Create_Match_Request;
    }
    else if (Input_String == "Client_Join_Match_Request")
    {
        return JSON_Common_Request_Types::Client_Join_Match_Request;
    }
    else if (Input_String == "Server_Match_Server_Verify_Request")
    {
        return JSON_Common_Request_Types::Server_Match_Server_Verify_Request;
    }
    else if (Input_String == "Server_Match_Server_Ready_Request")
    {
        return JSON_Common_Request_Types::Server_Match_Server_Ready_Request;
    }
    else if (Input_String == "Server_Stop_Match_Request")
    {
        return JSON_Common_Request_Types::Server_Stop_Match_Request;
    }
    else if (Input_String == "Server_Match_Allow_User_Request")
    {
        return JSON_Common_Request_Types::Server_Match_Allow_User_Request;
    }
    else
    {
        return JSON_Common_Request_Types::Invaild_Request;
    }
}
JSON_Common_Response_Types UCSSocketSubsystem::String_To_JSON_Common_Response_Types(FString Input_String)
{
    if (Input_String == "Client_User_Simple_Login_Response")
    {
        return JSON_Common_Response_Types::Client_User_Simple_Login_Response;
    }
    else if (Input_String == "Client_User_Simple_Registration_Response")
    {
        return JSON_Common_Response_Types::Client_User_Simple_Registration_Response;
    }
    else if (Input_String == "Client_Fetch_User_Full_Identification_Data_Response")
    {
        return JSON_Common_Response_Types::Client_Fetch_User_Full_Identification_Data_Response;
    }
    else if (Input_String == "Client_Fetch_User_Main_Identification_Data_Response")
    {
        return JSON_Common_Response_Types::Client_Fetch_User_Main_Identification_Data_Response;
    }
    else if (Input_String == "Client_Fetch_User_Partial_Identification_Data_Response")
    {
        return JSON_Common_Response_Types::Client_Fetch_User_Partial_Identification_Data_Response;
    }
    else if (Input_String == "Client_Fetch_User_Social_Data_Response")
    {
        return JSON_Common_Response_Types::Client_Fetch_User_Social_Data_Response;
    }
    else if (Input_String == "Client_Fetch_All_Ongoing_Match_Info_Response")
    {
        return JSON_Common_Response_Types::Client_Fetch_All_Ongoing_Match_Info_Response;
    }
    else if (Input_String == "Client_Fetch_All_Specified_Info_Type_Response")
    {
        return JSON_Common_Response_Types::Client_Fetch_All_Specified_Info_Type_Response;
    }
    else if (Input_String == "Client_Social_Command_Response")
    {
        return JSON_Common_Response_Types::Client_Social_Command_Response;
    }
    else if (Input_String == "Client_Get_Tickets_Owned_Response")
    {
        return JSON_Common_Response_Types::Client_Get_Tickets_Owned_Response;
    }
    else if (Input_String == "Client_Buy_Tickets_Response")
    {
        return JSON_Common_Response_Types::Client_Buy_Tickets_Response;
    }
    else if (Input_String == "Client_Get_User_Appearance_Data_Response")
    {
        return JSON_Common_Response_Types::Client_Get_User_Appearance_Data_Response;
    }
    else if (Input_String == "Client_Set_User_Appearance_Data_Response")
    {
        return JSON_Common_Response_Types::Client_Set_User_Appearance_Data_Response;
    }
    else if (Input_String == "Client_Create_Match_Response")
    {
        return JSON_Common_Response_Types::Client_Create_Match_Response;
    }
    else if (Input_String == "Client_Join_Match_Response")
    {
        return JSON_Common_Response_Types::Client_Join_Match_Response;
    }
    else if (Input_String == "Server_Match_Server_Verify_Response")
    {
        return JSON_Common_Response_Types::Server_Match_Server_Verify_Response;
    }
    else if (Input_String == "Server_Match_Server_Ready_Response")
    {
        return JSON_Common_Response_Types::Server_Match_Server_Ready_Response;
    }
    else if (Input_String == "Server_Stop_Match_Response")
    {
        return JSON_Common_Response_Types::Server_Stop_Match_Response;
    }
    else if (Input_String == "Server_Match_Allow_User_Response")
    {
        return JSON_Common_Response_Types::Server_Match_Allow_User_Response;
    }
    else
    {
        return JSON_Common_Response_Types::Invaild_Response;
    }
}
JSON_Common_Social_Types UCSSocketSubsystem::String_To_JSON_Common_Social_Types(FString Input_String)
{
    if (Input_String == "Friends")
    {
        return JSON_Common_Social_Types::Friends;
    }
    if (Input_String == "Followers")
    {
        return JSON_Common_Social_Types::Followers;
    }
    if (Input_String == "Blocked")
    {
        return JSON_Common_Social_Types::Blocked;
    }
    if (Input_String == "Following")
    {
        return JSON_Common_Social_Types::Following;
    }
    if (Input_String == "Blocking")
    {
        return JSON_Common_Social_Types::Blocking;
    }
    if (Input_String == "Incoming_Friend_Request")
    {
        return JSON_Common_Social_Types::Incoming_Friend_Request;
    }
    if (Input_String == "Outgoing_Friend_Request")
    {
        return JSON_Common_Social_Types::Outgoing_Friend_Request;
    }
    if (Input_String == "Invalid_Social")
    {
        return JSON_Common_Social_Types::Invalid_Social;
    }
    else
    {
        return JSON_Common_Social_Types::Invalid_Social;
    }
}
JSON_Common_Match_Types UCSSocketSubsystem::String_To_JSON_Common_Match_Types(FString Input_String)
{
    if (Input_String == "Ongoing_Match")
    {
        return JSON_Common_Match_Types::Ongoing_Match;
    }
    if (Input_String == "Invalid_Match_Type")
    {
        return JSON_Common_Match_Types::Invalid_Match_Type;
    }
    else
    {
        return JSON_Common_Match_Types::Invalid_Match_Type;
    }
}
JSON_Match_Flag_Types UCSSocketSubsystem::String_To_JSON_Match_Flag_Types(FString Input_String)
{
    if (Input_String == "Public_Match")
    {
        return JSON_Match_Flag_Types::Public_Match;
    }
    if (Input_String == "Private_Match")
    {
        return JSON_Match_Flag_Types::Private_Match;
    }
    if (Input_String == "Invalid_Flag_Type")
    {
        return JSON_Match_Flag_Types::Invalid_Flag_Type;
    }
    else
    {
        return JSON_Match_Flag_Types::Invalid_Flag_Type;
    }
}
JSON_Common_Social_Command_Types UCSSocketSubsystem::String_To_JSON_Common_Social_Command_Types(FString Input_String)
{
    if (Input_String == "Friend")
    {
        return JSON_Common_Social_Command_Types::Friend;
    }
    if (Input_String == "UnFriend")
    {
        return JSON_Common_Social_Command_Types::UnFriend;
    }
    if (Input_String == "Follow")
    {
        return JSON_Common_Social_Command_Types::Follow;
    }
    if (Input_String == "UnFollow")
    {
        return JSON_Common_Social_Command_Types::UnFollow;
    }
    if (Input_String == "Block")
    {
        return JSON_Common_Social_Command_Types::Block;
    }
    if (Input_String == "UnBlock")
    {
        return JSON_Common_Social_Command_Types::UnBlock;
    }
    if (Input_String == "Invalid_Social_Command")
    {
        return JSON_Common_Social_Command_Types::Invalid_Social_Command;
    }
    else
    {
        return JSON_Common_Social_Command_Types::Invalid_Social_Command;
    }
}
JSON_Common_Fetch_Specified_Types UCSSocketSubsystem::String_To_JSON_Common_Fetch_Specified_Types(FString Input_String)
{
    if (Input_String == "User_Name")
    {
        return JSON_Common_Fetch_Specified_Types::User_Name;
    }
    if (Input_String == "Match")
    {
        return JSON_Common_Fetch_Specified_Types::Match;
    }
    if (Input_String == "Invalid_Fetch_Specified_Type")
    {
        return JSON_Common_Fetch_Specified_Types::Invalid_Fetch_Specified_Type;
    }
    else
    {
        return JSON_Common_Fetch_Specified_Types::Invalid_Fetch_Specified_Type;
    }
}
//// ~~Enum_To_String~ End~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///====================================================
//****************************************************
// End
// Common Enum FUNCTIONS