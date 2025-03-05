// Developed By Wrexa Technologies 2023

#pragma once
//#pragma optimize("", off)
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

// For Multi Threading
#include "Async/AsyncWork.h"

#include "Wrexa_Blueprint_Functions.h"

#ifndef ENABLE_SCREEN_CAPTURE
#define ENABLE_SCREEN_CAPTURE
#endif

THIRD_PARTY_INCLUDES_START
// ScreenCapture Requirments
#ifdef _WIN32

// Windows Start
#ifndef USE_DX_SCREEN_CAP
#define USE_DX_SCREEN_CAP
#endif
#ifdef USE_DX_SCREEN_CAP
#define OS_NAME "Windows"
#include <dxgi1_2.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
// Windows End
#else
#ifdef __linux__
#error "LINUX BUILD DETECTED"
// Linux code
#define OS_NAME "Linux"
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
// Linux End
#else
#ifdef __ANDROID__
// Android code
#define OS_NAME "Android"
// Android End
#ifdef __APPLE__
// macOS code
#define OS_NAME "macOS"
// macOS End
#else
#error "Unknown platform"
#endif
#endif
#endif
#endif
#endif
THIRD_PARTY_INCLUDES_END


///Forward Declare
//class AWrexa_Player_Controller;
class UWrexa_Instance;


// For Required Structs
#include "Wrexa_Blueprint_Functions.h"

#include "Wrexa_Screen_Capture.generated.h"

#ifdef _WIN32
#ifdef USE_DX_SCREEN_CAP
struct Dev
{
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_9_1;
    Dev();
    ~Dev();
};

struct OutputDuplication
{
    IDXGIDevice* dxgiDevice = nullptr;
    IDXGIAdapter* dxgiAdapter = nullptr;
    IDXGIOutput* dxgiOutput = nullptr;
    std::vector<IDXGIOutput*> outputs;
    IDXGIOutput1* dxgiOutput1 = nullptr;
    IDXGIOutputDuplication* outputDuplication = nullptr;

    OutputDuplication();

    OutputDuplication(ID3D11Device* device, int monitorIndex);

    OutputDuplication& OutputDuplication::operator=(OutputDuplication&& other);

    ~OutputDuplication();
};

struct AcquiredDesktopImage
{
    IDXGIResource* desktopResource = nullptr;
    ID3D11Texture2D* acquiredDesktopImage = nullptr;

    AcquiredDesktopImage(IDXGIOutputDuplication* outputDuplication);

    ~AcquiredDesktopImage();
};

struct Image
{
    std::vector<byte> bytes;
    int width = 0;
    int height = 0;
    int rowPitch = 0;
};

class Singleton_DirectX_Screen_Capture
{
public:
    int Largest_Monitor_Index = 0;
    //std::unordered_map <int, D3D11_TEXTURE2D_DESC> Current_Description_Map;
    std::unordered_map <int, OutputDuplication*> DX_Duplicated_Output_Map;

    static Singleton_DirectX_Screen_Capture& GetInstance();

    static bool Get_Duplicated_Output(int Monitor_Index, OutputDuplication** Return_Val);

private:
    void Add_New_Duplicated_Output(int Monitor_Index, OutputDuplication* To_Add_Val);

    void Remove_Duplicated_Output(int Monitor_Index);

    static void Add_Duplicated_Output(int Monitor_Index, OutputDuplication* To_Add_Val);

public:
    static OutputDuplication* Return_Duplicated_Output_Add_If_Does_Not_Exist(int Monitor_Index, ID3D11Device* device);

    static void Remove_Duplicated_Output_If_Exists(int Monitor_Index);

    ~Singleton_DirectX_Screen_Capture();

private:   

    Singleton_DirectX_Screen_Capture() {};
    Singleton_DirectX_Screen_Capture(Singleton_DirectX_Screen_Capture const&) = delete;
    Singleton_DirectX_Screen_Capture& operator=(Singleton_DirectX_Screen_Capture const&) = delete;
};

struct DX_Screen_Cap_Struct
{
public:
    int Capturing_Monitor_Index = 0;
    Dev DX_Device;
    OutputDuplication* DX_Duplicated_Output_ptr = nullptr;
    IDXGIResource* desktopResource = nullptr;
    ID3D11Texture2D* tex2dStaging = nullptr;
    D3D11_MAPPED_SUBRESOURCE res;

    DX_Screen_Cap_Struct();
    bool Initialize(int Monitor_Index);
    DX_Screen_Cap_Struct(int Monitor_Index, bool &Is_Successful);

    DX_Screen_Cap_Struct& DX_Screen_Cap_Struct::operator=(DX_Screen_Cap_Struct&& other);
    
    bool Get_Frame(int& Return_Width, int& Return_Height, int Resized_Width, int Resized_Height, unsigned char** RGBA_Buffer);
    bool Get_Frame_Not_Working(int& Return_Width, int& Return_Height, int Resized_Width, int Resized_Height, unsigned char** RGBA_Buffer);
    void Free_Frame();
    
    ~DX_Screen_Cap_Struct();
};

class Singleton_DirectX_Screen_Capture_Direct_Access_Buffer
{
public:
    int Largest_Monitor_Index = 0;
    std::unordered_map <int, DX_Screen_Cap_Struct*> Screen_Capture_Context_Map;

    static Singleton_DirectX_Screen_Capture_Direct_Access_Buffer& GetInstance();
        
    static bool Get_Frame(int Monitor_Index, int& Return_Width, int& Return_Height, int Resized_Width, int Resized_Height, unsigned char** RGBA_Buffer);
    static void Remove_Screen_Capture_Context(int Monitor_Index);

    ~Singleton_DirectX_Screen_Capture_Direct_Access_Buffer();

    static void Stop_Externally();

private:

    Singleton_DirectX_Screen_Capture_Direct_Access_Buffer() {};
    Singleton_DirectX_Screen_Capture_Direct_Access_Buffer(Singleton_DirectX_Screen_Capture_Direct_Access_Buffer const&) = delete;
    Singleton_DirectX_Screen_Capture_Direct_Access_Buffer& operator=(Singleton_DirectX_Screen_Capture_Direct_Access_Buffer const&) = delete;
};

#endif
#endif

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class WREXA_API UWrexa_Screen_Capture : public UObject
{
	GENERATED_BODY()

public:
#ifdef _WIN32
    // Windows Start
    bool Can_Capture_Screen = true;
#else
#ifdef __linux__
    // Linux code
    bool Can_Capture_Screen = true;
    // Linux End
#else
#ifdef __ANDROID__
    bool Can_Capture_Screen = false;
    // Android End
#else
#ifdef __APPLE__
    // macOS code
    bool Can_Capture_Screen = false;
    // macOS End
#else
    bool Can_Capture_Screen = false;
#endif
#endif
#endif
#endif

    bool Request_Screen_Capture_API()
    {
#ifdef _WIN32
        // Windows Start
        Can_Capture_Screen = true;
        return true;
#else
#ifdef __linux__
        // Linux code
        Can_Capture_Screen = false;
        return false;
        // Linux End
#else
#ifdef __ANDROID__
        // Android code
        return false;
        // Android End
#else
#ifdef __APPLE__
        // macOS code
        return false;
        // macOS End
#else
        return false;
#endif
#endif
#endif
#endif
    }

    static int Get_Total_Number_Of_Monitors_API()
    {
#ifndef ENABLE_SCREEN_CAPTURE
        return 0;
#else
#ifdef _WIN32
        return GetSystemMetrics(SM_CMONITORS);
#elif __linux__
        FILE* fp;
        char* line = NULL;
        size_t len = 0;
        ssize_t read;
        int num_monitors = 0;

        fp = popen("xrandr --query | grep ' connected' | wc -l", "r");
        if (fp == NULL)
        {
            perror("popen failed");
            exit(EXIT_FAILURE);
        }

        if ((read = getline(&line, &len, fp)) != -1)
        {
            num_monitors = atoi(line);
        }

        pclose(fp);
        free(line);

        return num_monitors;
#else
        return 0;
#endif
#endif
    }
#ifdef _WIN32
    static int CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
    {
#ifndef ENABLE_SCREEN_CAPTURE
        return 0;
#else
        std::vector<HMONITOR>* monitors = reinterpret_cast<std::vector<HMONITOR> *>(dwData);
        monitors->push_back(hMonitor);
        return 1;
#endif
    }
#endif

#ifdef _WIN32
#ifdef USE_DX_SCREEN_CAP
    D3D11_TEXTURE2D_DESC Current_Description;
    //OutputDuplication DX_Duplicated_Output;
    static bool captureDesktop_DX(int Monitor_To_Capture, int& width, int& height, unsigned char** RGBA_Buffer);
    bool captureDesktop_DX_REUSE(int Monitor_To_Capture, int& width, int& height, unsigned char** RGBA_Buffer);
    static bool captureDesktop_DX_REUSE_Unsafe(int Monitor_To_Capture, int& Return_Width, int& Return_Height, int Resized_Width, int Resized_Height, unsigned char** RGBA_Buffer);
#endif
#endif // _WIN32    

public:	
	void BeginDestory();

    uint8* Image_Raw_Buffer = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        int Screen_Capture_Width = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        int Screen_Capture_Height = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int Current_Monitor_Index = 0;

    // Don't Forget to Free Data Using delete Keyword
    static bool Capture_Screen_API(int Monitor_To_Capture, int& width, int& height, unsigned char** RGBA_Buffer);
    bool Capture_Screen_API_REUSE(int Monitor_To_Capture, int& width, int& height, unsigned char** RGBA_Buffer);

	static bool Is_Screen_Capture_Allowed_By_The_Operating_System_API();
	static bool Request_To_Screen_Capture_Operating_System_API(bool Bypass_Check);
	bool Start_Screen_Capture_Internal_API(const int& Monitor_Index, const int& CallBack_Frequency_Milliseconds, const int& Offset_Width, const int& Offset_Height);
	bool Stop_Screen_Capture_API();

    UFUNCTION(BlueprintCallable, Category = "Wrexa | Screen Capture")
    static int Get_Total_Number_Of_Monitors();

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Screen Capture")
		static bool Is_Screen_Capture_Allowed_By_The_Operating_System();

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Screen Capture")
		static bool Request_To_Screen_Capture_Operating_System(bool Bypass_Check);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Screen Capture")
		 bool Start_Screen_Capture(const int& Monitor_Index, const int& CallBack_Frequency_Milliseconds, const int& Offset_Width, const int& Offset_Height);//static bool Start_Screen_Capture(FScreen_Capture_Ptr &Return_Keep_Alive_ptr);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Screen Capture")
		bool Stop_Screen_Capture();

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Screen Capture")
		UTexture2D* TextureFrom_Screen_Capture(bool Swap_Colour_Channel, bool& IsTextureAvailable);

	UFUNCTION(BlueprintCallable, Category = "Wrexa | Screen Capture")
		void Image_From_Screen_Capture(bool& IsTextureAvailable, int& Width, int& Height, TArray<uint8>& Image_Buffer, bool& True_For_RGBA_False_For_RGB);

    void Image_From_Screen_Capture_Pointer(bool& IsTextureAvailable, int& Width, int& Height, uint8** Image_Buffer, bool& True_For_RGBA_False_For_RGB);
    void Image_From_Screen_Capture_Pointer_Unsafe(bool& IsTextureAvailable, int& Return_Width, int& Return_Height, int Resized_Width, int Resized_Height, uint8** Image_Buffer);

    UFUNCTION(BlueprintCallable, Category = "Wrexa | Screen Capture")
        bool Request_Get_Frame(UWrexa_Instance* Reference_Of_Main_Game_Instance, const bool& Is_GPU_Capture);

    UFUNCTION(BlueprintCallable, Category = "Wrexa | Screen Capture")
        bool Request_Get_Frame_With_Config(UWrexa_Instance* Reference_Of_Main_Game_Instance, const bool& Is_GPU_Capture, const bool& Should_Resize_And_JPEG, const int& Arg_JPEG_Quality_Percent, const EResize_Alogrithm_Type& Arg_Resize_Algorithm_Type, const int& Arg_Resize_Width, const int& Arg_Resize_Height);
};


class FScreen_Share_Thread : public FRunnable
{
    FScreen_Share_Thread* Runnable = nullptr;

    /** Thread to run the worker FRunnable on */
    FRunnableThread* Thread = nullptr;

    /** Stop this thread? Uses Thread Safe Counter */
    FThreadSafeCounter StopTaskCounter;

public:
    /** The PC */
    //AWrexa_Player_Controller* Target_Player_Controller = nullptr;
    UWrexa_Instance* Main_Game_Instance = nullptr;
    TArray <uint8> RGBA_Buffer;
    FThreadSafeCounter Current_Monitor_Index = 0;
    FThreadSafeBool IsAccesingArray = false;

    // Multi Threaded GPU Screen Capture
    FThreadSafeBool Is_Currently_GPU_Capture = true;
    //std::atomic<bool> Is_Requesting_GPU_Capture = true;
    FThreadSafeBool Is_Screen_Capture_Thread_Running = false;
    //FThreadSafeBool Is_Screen_Capture_Thread_Stop_Requested = false;
    FThreadSafeBool Is_Capture_Current_Frame = false;

    FThreadSafeBool Is_Resize_And_JPEG = true;
    FThreadSafeCounter JPEG_Quality_Percent = 25;

    FThreadSafeCounter Resize_Algorithm_Type = 1;// By Default It is Nearest Neighbour
    
    FThreadSafeCounter Resize_Width = 0;
    FThreadSafeCounter Resize_Height = 0;

    // Received Original
    int Width = 0;
    int Height = 0;

    bool IsFinished = true;

public:
    // Constructor
    //FScreen_Share_Thread(AWrexa_Player_Controller* Reference_Of_Target_Player_Controller);
    FScreen_Share_Thread(UWrexa_Instance* Reference_Of_Main_Game_Instance);
    
    // Destructor
    virtual ~FScreen_Share_Thread();

    void Safely_Destroy();

    // Begin FRunnable interface.
    virtual bool Init();
    virtual uint32 Run();
    virtual void Stop();
    // End FRunnable interface

    // Construction Helper
    //static FScreen_Share_Thread* Create_Screen_Capture_Thread(AWrexa_Player_Controller* Reference_Of_Target_Player_Controller);
    static FScreen_Share_Thread* Create_Screen_Capture_Thread(UWrexa_Instance* Reference_Of_Main_Game_Instance);

    //~~~ Thread Core Functions ~~~
    void Change_Screen_Capture_Type(bool Is_GPU_Capture);
    void Should_Resize_And_Convert_To_JPEG(bool Should_Resize_And_JPEG, int Arg_JPEG_Quality_Percent, EResize_Alogrithm_Type Arg_Resize_Algorithm_Type, int Arg_Resize_Width, int Arg_Resize_Height);
    void Request_Frame(int Capture_Monitor_Index);
    void Request_Frame_And_Set_Screen_Capture_Type(int Capture_Monitor_Index, bool Is_GPU_Capture);
    void Request_Frame_And_Set_Config(int Capture_Monitor_Index, bool Is_GPU_Capture, bool Should_Resize_And_JPEG, int Arg_JPEG_Quality_Percent, EResize_Alogrithm_Type Arg_Resize_Algorithm_Type, int Arg_Resize_Width, int Arg_Resize_Height);

    /** Makes sure this thread has stopped properly */
    void EnsureCompletion();    

    /** Shuts down the thread.*/
    void Shutdown();

    bool IsThreadFinished();
};

class Singleton_Screen_Capture_Thread
{
public:

    Singleton_Screen_Capture_Thread();
    ~Singleton_Screen_Capture_Thread();

    FThreadSafeBool Is_Game_Thread_Available_For_Access = false;
    static void Stop_Externally();

    FScreen_Share_Thread* Screen_Capture_Thread_Ptr = nullptr;

    static Singleton_Screen_Capture_Thread* GetInstance();

    //static bool Get_Frame(AWrexa_Player_Controller* Reference_Of_Target_Player_Controller, int Monitor_Index, const bool& Is_GPU_Capture);
    static bool Get_Frame(UWrexa_Instance* Reference_Of_Main_Game_Instance, int Monitor_Index, const bool& Is_GPU_Capture);
    static bool Get_Frame_With_Config(UWrexa_Instance* Reference_Of_Main_Game_Instance, int Capture_Monitor_Index, const bool& Is_GPU_Capture, const bool& Should_Resize_And_JPEG, const int& Arg_JPEG_Quality_Percent, const EResize_Alogrithm_Type& Arg_Resize_Algorithm_Type, const int& Arg_Resize_Width, const int& Arg_Resize_Height);

private:
    Singleton_Screen_Capture_Thread(Singleton_Screen_Capture_Thread const&) = delete;
    Singleton_Screen_Capture_Thread& operator=(Singleton_Screen_Capture_Thread const&) = delete;
};