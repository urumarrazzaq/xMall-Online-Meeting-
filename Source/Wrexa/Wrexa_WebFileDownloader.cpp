#include "Wrexa_WebFileDownloader.h"

#include "GenericPlatform/GenericPlatformMisc.h"

UWrexa_WebFileDownloader* UWrexa_WebFileDownloader::GetWebFileDownloader(FString Downloaded_FileName, FString URL, FString File_Extension, bool& IsValid)
{
	IsValid = false;

	UWrexa_WebFileDownloader *Object = NewObject<UWrexa_WebFileDownloader>();

	if (!Object) {
		UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::GetWebFileDownloader] Could not be created"));
		return NULL;
	}
	if (!Object->IsValidLowLevel()) {
		UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::GetWebFileDownloader] Created object is not valid"));
		return NULL;
	}

	UE_LOG(LogTemp, Warning, TEXT("[UWrexa_WebFileDownloader::GetWebFileDownloader] An instance is created successfully"));

	Object->SetWebFileNameAndURL(Downloaded_FileName, URL, File_Extension);
	IsValid = true;
	return Object;
}

FString UWrexa_WebFileDownloader::WebFileFolder()
{	
	FString FileDir(FGenericPlatformMisc::GamePersistentDownloadDir());
	FileDir += "/";// "/ Downloads/";
	FPaths::NormalizeDirectoryName(FileDir);
	return FString(FPaths::ConvertRelativePathToFull(FileDir));
}

bool UWrexa_WebFileDownloader::CreateWebFileFolder()
{
	FDirWebFileRecursiveDownloader RFolder = FDirWebFileRecursiveDownloader::CreateLambda([=](FString Folder)
	{
		const int32 MAX_LOOP_ITR = 3000;
		FPaths::NormalizeDirectoryName(Folder);
		Folder += "/";

		FString Base;
		FString Left;
		FString Remaining;

		Folder.Split(TEXT("/"), &Base, &Remaining);
		Base += "/";

		int32 LoopItr = 0;
		while (Remaining != "" && LoopItr < MAX_LOOP_ITR)
		{
			Remaining.Split(TEXT("/"), &Left, &Remaining);
			Base += Left + FString("/");
			FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*Base);

			UE_LOG(LogTemp, Warning, TEXT("[UWrexa_WebFileDownloader::CreateWebFileFolder] Creating %s"), *Base);
			LoopItr++;
		}
	});

	FString FileDir = WebFileFolder();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (!PlatformFile.DirectoryExists(*FileDir)) {
		RFolder.Execute(FileDir);

		if (!PlatformFile.DirectoryExists(*FileDir)) {
			UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::CreateWebFileFolder] Cannot create folder %s"), *FileDir);
			return false;
		}

		UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::CreateWebFileFolder] Created folder %s"), *FileDir);
	}

	return true;
}

TArray<FString> UWrexa_WebFileDownloader::DownloadedWebFilesList()
{
	FString folder = WebFileFolder() + "/*.*";
	IFileManager& FileManager = IFileManager::Get();

	TArray<FString>files;
	FileManager.FindFiles(files, *folder, true, false);
	//for (int i = 0; i < files.Num(); i++) {
	//	FString str = files[i];
	//	files[i] = str.Replace(TEXT(".jpg"), TEXT(""), ESearchCase::IgnoreCase);
	//}
	return files;
}

void UWrexa_WebFileDownloader::IsWebFileDownloaded(FString WebFileName, bool& isDownloaded)
{
	if (!CreateWebFileFolder())
	{
		isDownloaded = false;
		return;
	}

	FString dataFile = WebFileFolder() + "/" + WebFileName;// +".jpg";
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	isDownloaded = PlatformFile.FileExists(*dataFile);
}

void UWrexa_WebFileDownloader::DeleteWebFileFile(FString WebFileName, bool &isDeleted)
{
	FString dataFile = WebFileFolder() + "/" + WebFileName;// +".jpg";
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.FileExists(*dataFile))
	{
		isDeleted = true;
		return;
	}

	isDeleted = PlatformFile.DeleteFile(*dataFile);
}

void UWrexa_WebFileDownloader::CheckIfWebFileHasUpdate()
{
	UE_LOG(LogTemp, Warning, TEXT("[UWrexa_WebFileDownloader::DoesWebFileHaveUpdate] Checking for update %s"), *OutputFileName);

	FString dataFile = WebFileFolder() + "/" + OutputFileName;// +".jpg";
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.FileExists(*dataFile))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UWrexa_WebFileDownloader::DoesWebFileHaveUpdate] WebFile not downloaded yet %s"), *OutputFileName);
		OnUpdateCheckCompleted.Broadcast(OutputFileName, true);
		return;
	}

	RequestSize = -1;
	RequestUrl = OriginalURL;// +"/" + OutputFileName;// +".jpg";
	UE_LOG(LogTemp, Warning, TEXT("[UWrexa_WebFileDownloader::DoesWebFileHaveUpdate] Requesting headers for %s from %s"), *OutputFileName, *RequestUrl);

	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest> HttpRequest = Http->CreateRequest();
	UpdateRequest = HttpRequest;

	HttpRequest->SetVerb(TEXT("HEAD"));
	HttpRequest->SetURL(RequestUrl);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UWrexa_WebFileDownloader::UpdateCheckHttpRequestComplete);
	if (!HttpRequest->ProcessRequest())
	{
		OnUpdateCheckCompleted.Broadcast(OutputFileName, false);
	}
}

void UWrexa_WebFileDownloader::UpdateCheckHttpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	UpdateRequest.Reset();

	if (!Response.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::UpdateCheckHttpRequestComplete] Could not connect to %s"), *RequestUrl);
		OnUpdateCheckCompleted.Broadcast(OutputFileName, false);
		return;
	}

	if (!bWasSuccessful) {
		UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::UpdateCheckHttpRequestComplete] Could not connect to %s"), *RequestUrl);
		OnUpdateCheckCompleted.Broadcast(OutputFileName, false);
		return;
	}

	FString dataFile = WebFileFolder() + "/" + OutputFileName;// +".jpg";
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FFileStatData statData = PlatformFile.GetStatData(*dataFile);

	bool isSizeDifferent = false;
	bool isModDateDifferent = false;

	int fileSize = 0;
	int modDate = 0;

	{
		int32 StatusCode = Response->GetResponseCode();
		if (StatusCode / 100 != 2)
		{
			UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::UpdateCheckHttpRequestComplete] %s HTTP response %d, for %s"), *OutputFileName, StatusCode, *RequestUrl);
			OnUpdateCheckCompleted.Broadcast(OutputFileName, false);
			return;
		}

		TArray<FString> headers = Response->GetAllHeaders();
		for (FString h : headers) {
			UE_LOG(LogTemp, Warning, TEXT("[UWrexa_WebFileDownloader::UpdateCheckHttpRequestComplete] %s Header: %s"), *OutputFileName, *h);
		}

		for (FString h : headers) {
			if (h.Contains("x-file-size", ESearchCase::IgnoreCase) || h.Contains("Content-Length", ESearchCase::IgnoreCase)) {
				FString left;
				FString right;
				h.Split(":", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);
				if (right.Len())
				{
					fileSize = FCString::Atoi(*right);
				}
			}

			if (h.Contains("x-file-mod", ESearchCase::IgnoreCase)) {
				FString left;
				FString right;
				h.Split(":", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);
				if (right.Len())
				{
					modDate = FCString::Atoi(*right);
				}
			}

			if (h.Contains("Last-Modified", ESearchCase::IgnoreCase)) {
				FString left;
				FString right;
				h.Split(":", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);
				if (right.Len())
				{
					right = right.TrimEnd();
					FDateTime date;
					FDateTime::ParseHttpDate(right, date);
					modDate = date.ToUnixTimestamp();
				}
			}
		}

		Request.Reset();
	}

	UE_LOG(LogTemp, Warning, TEXT("[UWrexa_WebFileDownloader::UpdateCheckHttpRequestComplete] %s - REMOTE: File size %i Mod date %i"), *OutputFileName, fileSize, modDate);
	UE_LOG(LogTemp, Warning, TEXT("[UWrexa_WebFileDownloader::UpdateCheckHttpRequestComplete] %s - LOCAL:  File size %i Mod date %i"), *OutputFileName, statData.FileSize, statData.ModificationTime.ToUnixTimestamp());

	isSizeDifferent = fileSize > 0 && statData.FileSize != fileSize;
	isModDateDifferent = modDate > 0 && modDate > statData.ModificationTime.ToUnixTimestamp();
	OnUpdateCheckCompleted.Broadcast(OutputFileName, isSizeDifferent || isModDateDifferent);
}

void UWrexa_WebFileDownloader::SetWebFileNameAndURL(FString Downloaded_FileName, FString URL, FString File_Extension)
{
	if (!IsDownloading)
	{
		OutputFileName = Downloaded_FileName + "." + File_Extension;
		OriginalURL = URL;
		
		IsDownloading = true;
		IsComplete = false;
		IsFailed = false;
	}
}

void UWrexa_WebFileDownloader::DownloadWebFile()
{
	CreateWebFileFolder();

	RequestSize = -1;
	RequestUrl = OriginalURL;// +"/" + OutputFileName;// +".jpg";
	UE_LOG(LogTemp, Warning, TEXT("[UWrexa_WebFileDownloader::DownloadWebFile] %s Requesting headers for %s"), *OutputFileName, *RequestUrl);

	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest> HttpRequest = Http->CreateRequest();
	DownloadRequest = HttpRequest;

	HttpRequest->SetVerb(TEXT("HEAD"));
	HttpRequest->SetURL(RequestUrl);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UWrexa_WebFileDownloader::HttpRequestComplete);
	if (!HttpRequest->ProcessRequest())
	{
		IsDownloading = false;
		IsComplete = false;
		IsFailed = true;
		OnWebFileDownloadError.Broadcast(OutputFileName);
	}
}

void UWrexa_WebFileDownloader::HttpRequestProgress(FHttpRequestPtr Request, int32 bytesSent, int32 bytesReceived)
{
	if (RequestSize <= 0) return;

	float percent = (float)bytesReceived / (float)RequestSize;
	OnWebFileDownloadProgress.Broadcast(OutputFileName, (int32)(percent * 100));
}

void UWrexa_WebFileDownloader::HttpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	DownloadRequest.Reset();

	if (!Response.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::HttpRequestComplete] Could not connect to %s"), *RequestUrl);
		IsDownloading = false;
		IsComplete = false;
		IsFailed = true;
		OnWebFileDownloadError.Broadcast(OutputFileName);
		return;
	}

	if (!bWasSuccessful) {
		UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::HttpRequestComplete] Could not connect to %s"), *RequestUrl);
		IsDownloading = false;
		IsComplete = false;
		IsFailed = true;
		OnWebFileDownloadError.Broadcast(OutputFileName);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[UWrexa_WebFileDownloader::HttpRequestComplete] %s Starting download of %s"), *OutputFileName, *RequestUrl);

	// Finding size of the requested file
	{
		int32 StatusCode = Response->GetResponseCode();
		if (StatusCode / 100 != 2)
		{
			UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::HttpRequestComplete] %s HTTP response %d, for %s"), *OutputFileName, StatusCode, *RequestUrl);
			IsDownloading = false;
			IsComplete = false;
			IsFailed = true;
			OnWebFileDownloadError.Broadcast(OutputFileName);
			return;
		}

		TArray<FString> headers = Response->GetAllHeaders();
		for (FString h : headers) {
			UE_LOG(LogTemp, Warning, TEXT("[UWrexa_WebFileDownloader::HttpRequestComplete] %s Header: %s"), *OutputFileName, *h);
		}

		for (FString h : headers) {
			if (h.Contains("x-file-size", ESearchCase::IgnoreCase) || h.Contains("Content-Length", ESearchCase::IgnoreCase)) {
				FString left;
				FString right;
				h.Split(":", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);
				if (right.Len())
				{
					RequestSize = FCString::Atoi(*right);
				}

				break;
			}
		}

		Request.Reset();
	}

	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest> HttpRequest = Http->CreateRequest();
	DownloadRequest = HttpRequest;

	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(RequestUrl);
	HttpRequest->OnRequestProgress().BindUObject(this, &UWrexa_WebFileDownloader::HttpRequestProgress);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UWrexa_WebFileDownloader::HttpDownloadComplete);
	if (!HttpRequest->ProcessRequest())
	{
		IsDownloading = false;
		IsComplete = false;
		IsFailed = true;
		OnWebFileDownloadError.Broadcast(OutputFileName);
	}
}

void UWrexa_WebFileDownloader::HttpDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	DownloadRequest.Reset();

	if (!Response.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::HttpDownloadComplete] Could not connect to %s"), *RequestUrl);
		IsDownloading = false;
		IsComplete = false;
		IsFailed = true;
		OnWebFileDownloadError.Broadcast(OutputFileName);
		return;
	}

	if (!bWasSuccessful) {
		UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::HttpDownloadComplete] Could not connect to %s"), *RequestUrl);
		IsDownloading = false;
		IsComplete = false;
		IsFailed = true;
		OnWebFileDownloadError.Broadcast(OutputFileName);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[UWrexa_WebFileDownloader::HttpDownloadComplete] Download completed for %s from %s"), *OutputFileName, *RequestUrl);

	int32 StatusCode = Response->GetResponseCode();
	if (StatusCode / 100 != 2)
	{
		UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::HttpDownloadComplete] %s HTTP response %d, for %s"), *OutputFileName, StatusCode, *RequestUrl);
		IsDownloading = false;
		IsComplete = false;
		IsFailed = true;
		OnWebFileDownloadError.Broadcast(OutputFileName);
		return;
	}

	TArray<FString> headers = Response->GetAllHeaders();
	for (FString h : headers) {
		UE_LOG(LogTemp, Warning, TEXT("UExpoSocket::HttpDownloadComplete] %s Header: %s"), *OutputFileName, *h);
	}

	const TArray<uint8>& Content = Response->GetContent();

	FString Filename = WebFileFolder() + "/" + OutputFileName;// +".jpg";
	if (FFileHelper::SaveArrayToFile(Content, *Filename))
	{
		IsDownloading = false;
		IsComplete = true;
		IsFailed = false;
		OnWebFileDownloadCompleted.Broadcast(OutputFileName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::HttpDownloadComplete] %s Could not write %s to disk "), *OutputFileName, *Filename);
		IsDownloading = false;
		IsComplete = false;
		IsFailed = true;
		OnWebFileDownloadError.Broadcast(OutputFileName);
	}
}

void UWrexa_WebFileDownloader::CancelDownload()
{
	UE_LOG(LogTemp, Error, TEXT("[UWrexa_WebFileDownloader::CancelDownload] Cancelling request for %s"), *RequestUrl);

	if (UpdateRequest.IsValid()) {
		if (UpdateRequest->OnProcessRequestComplete().IsBound())
			UpdateRequest->OnProcessRequestComplete().Unbind();

		UpdateRequest->CancelRequest();
		UpdateRequest.Reset();
	}

	if (DownloadRequest.IsValid()) {
		if (DownloadRequest->OnProcessRequestComplete().IsBound())
			DownloadRequest->OnProcessRequestComplete().Unbind();

		DownloadRequest->CancelRequest();
		DownloadRequest.Reset();
	}

	IsDownloading = false;
	IsComplete = false;
	IsFailed = false;
}