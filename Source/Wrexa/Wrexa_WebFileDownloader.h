#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Runtime/Online/HTTP/Public/Http.h"
#include "Runtime/Online/HTTP/Public/HttpManager.h"
#include "Wrexa_WebFileDownloader.generated.h"

DECLARE_DELEGATE_OneParam(FDirWebFileRecursiveDownloader, FString);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWebFileDownloader_OnDownloadComplete, FString, FileName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWebFileDownloader_OnDownloadError, FString, FileName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWebFileDownloader_OnDownloadProgress, FString, FileName, int32, progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWebFileDownloader_OnUpdateCheckCompleted, FString, FileName, bool, hasUpdate);

UCLASS(Blueprintable, BlueprintType)
class WREXA_API UWrexa_WebFileDownloader : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "WebFile Downloader", Meta = (DisplayName = "Get A Web File Downloader"))
		static UWrexa_WebFileDownloader* GetWebFileDownloader(FString Downloaded_FileName, FString URL, FString File_Extension, bool& IsValid);

	UFUNCTION(BlueprintPure, Category = "WebFile Downloader", Meta = (DisplayName = "Downloaded Web Files List"))
		static TArray<FString> DownloadedWebFilesList();

	UFUNCTION(BlueprintPure, Category = "WebFile Downloader", Meta = (DisplayName = "Delete Web File File"))
		static void DeleteWebFileFile(FString WebFileName, bool& isDeleted);

	UFUNCTION(BlueprintPure, Category = "WebFile Downloader", Meta = (DisplayName = "Is Web File Already Downloaded"))
		static void IsWebFileDownloaded(FString WebFileName, bool& isDownloaded);

	UPROPERTY(BlueprintAssignable, Category = "WebFile Downloader")
		FWebFileDownloader_OnDownloadComplete OnWebFileDownloadCompleted;

	UPROPERTY(BlueprintAssignable, Category = "WebFile Downloader")
		FWebFileDownloader_OnDownloadError OnWebFileDownloadError;

	UPROPERTY(BlueprintAssignable, Category = "WebFile Downloader")
		FWebFileDownloader_OnDownloadProgress OnWebFileDownloadProgress;

	UPROPERTY(BlueprintAssignable, Category = "WebFile Downloader")
		FWebFileDownloader_OnUpdateCheckCompleted OnUpdateCheckCompleted;

	UFUNCTION(BlueprintCallable, Category = "WebFile Downloader", Meta = (DisplayName = "Does Web File Have Update"))
		void CheckIfWebFileHasUpdate();

	UFUNCTION(BlueprintCallable, Category = "WebFile Downloader", Meta = (DisplayName = "Download Web File URL and Output Name"))
		void SetWebFileNameAndURL(FString Downloaded_FileName, FString URL, FString File_Extension);

	UFUNCTION(BlueprintCallable, Category = "WebFile Downloader", Meta = (DisplayName = "Start Downloading Web File"))
		void DownloadWebFile();

	UFUNCTION(BlueprintCallable, Category = "WebFile Downloader", Meta = (DisplayName = "Cancel Download"))
		void CancelDownload();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WebFile Downloader", Meta = (DisplayName = "Output File Name"))
		FString OutputFileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WebFile Downloader", Meta = (DisplayName = "Server URL"))
		FString OriginalURL;

	static FString WebFileFolder();
	static bool CreateWebFileFolder();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool IsDownloading = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool IsComplete = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool IsFailed = false;

private:

	void HttpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HttpDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HttpRequestProgress(FHttpRequestPtr Request, int32 bytesSent, int32 bytesReceived);

	void UpdateCheckHttpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	int32 RequestSize;
	FString RequestUrl;

	TSharedPtr<IHttpRequest> UpdateRequest;
	TSharedPtr<IHttpRequest> DownloadRequest;
};