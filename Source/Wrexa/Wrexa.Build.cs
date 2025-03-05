// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using UnrealBuildTool;

public class Wrexa : ModuleRules
{
	public Wrexa(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Slate", "RenderCore", "UMG", "Networking", "Sockets", "WebSockets", "Json", "JsonUtilities", "OnlineSubsystem", "OnlineSubsystemNull", "OnlineSubsystemUtils", "HairStrandsCore", "RHI", "Niagara", "LiveLinkAnimationCore", "WebBrowser", "WebBrowserWidget" });
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Slate", "RenderCore", "UMG", "Networking", "Sockets", "WebSockets", "Json", "JsonUtilities", "OnlineSubsystem", "OnlineSubsystemNull", "OnlineSubsystemUtils", "RHI", "Niagara", "LiveLinkAnimationCore", "WebBrowser", "WebBrowserWidget", "ImageWrapper", "ImageCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { "ReadyPlayerMe", "HTTP" });

        if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            Console.WriteLine("Building For Linux");
            //PublicAdditionalCompileOptions.Add("-isystem C:/UnrealToolchains/v20_clang-13.0.1-centos7/x86_64-unknown-linux-gnu/usr/include");
        }

        //Load_Screen_capture_lite_static(Target);//Replaced With Header Only Version Built By Punal Manalan
        //Load_jpge_Compressor_static(Target); Not Needed Use Unreal Engine's Built it Function
    }

    public int Start_CMD_Process(string CMD_Path, string Option_And_Commands, string Main_Working_Directory)
    {
        var processInfo = new ProcessStartInfo(CMD_Path, Option_And_Commands)
        {
            CreateNoWindow = true,
            UseShellExecute = false,
            RedirectStandardError = true,
            RedirectStandardOutput = true,
            WorkingDirectory = Main_Working_Directory
        };

        StringBuilder outputString = new StringBuilder();
        Process p = Process.Start(processInfo);

        p.OutputDataReceived += (sender, args) => { outputString.Append(args.Data); Console.WriteLine(args.Data); };
        p.ErrorDataReceived += (sender, args) => { outputString.Append(args.Data); Console.WriteLine(args.Data); };
        p.BeginOutputReadLine();
        p.BeginErrorReadLine();
        p.WaitForExit();

        //if(p.ExitCode != 0)
        //{
        //     Console.WriteLine(outputString);
        //}
        Console.WriteLine(outputString);
        return p.ExitCode;
    }

    public bool Load_Screen_capture_lite_static(ReadOnlyTargetRules Target)
    {
        bool IsSuccessful = false;
        bool isLibrarySupported = false;

        string Main_Working_Directory = Path.GetFullPath(Path.Combine(this.ModuleDirectory, "../../"));
        Main_Working_Directory = Main_Working_Directory.Replace("\\", "/");

        string cmd = "";
        string options = "";

        string CMake_Source_Path = "";
        string CMake_Build_Path = "";
        string CMake_ToolChain_Path = "";// Optional

        string Generator = "\"Visual Studio 17 2022\"";// "\"Visual Studio 17 2022\"";//"Visual Studio 17 2022" Quotes are important
        string Architecture = "x64";// ARM ARM64

        string All_Arguments = "";// This Should Be Filled Manually

        string Fullcommand = "";

        if ((BuildHostPlatform.Current.Platform == UnrealTargetPlatform.Win64)
#if !UE_5_0_OR_LATER
            || (BuildHostPlatform.Current.Platform == UnrealTargetPlatform.Win32)
#endif//!UE_5_0_OR_LATER
            )
        {
            cmd = "cmd.exe";
            options = "/c ";
        }
        else if (BuildHostPlatform.Current.Platform == UnrealTargetPlatform.Linux)
        {
            cmd = "bash";
            options = "-c ";
        }

        CMake_Source_Path = Path.GetFullPath(Path.Combine(this.ModuleDirectory, "../Deps/screen_capture_lite"));
        CMake_Build_Path = Path.GetFullPath(Path.Combine(this.ModuleDirectory, "../Deps/Build/screen_capture_lite"));
        //CMake_ToolChain_Path = Path.GetFullPath(Path.Combine(this.ModuleDirectory, "../CMake_Linux_CrossCompile_ToolChain.cmake"));

        //if (!System.IO.Directory.Exists(CMake_Build_Path))
        //{
        //    System.IO.Directory.CreateDirectory(CMake_Build_Path);
        //}        

        switch (Target.Configuration)
        {
            case UnrealTargetConfiguration.Shipping:
                {
                    // Dynamic Library (Only -DCMAKE_BUILD_TYPE=Release) Works
                    //CMakeTarget.add(Target, this, "screen_capture_lite_shared", Path.Combine(this.ModuleDirectory, "../Deps/screen_capture_lite"), "-DBUILD_EXAMPLE=OFF -DBUILD_CSHARP=ON -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release", true);
                    //string Destination = Path.Combine("$(BinaryOutputDir)", "screen_capture_lite_shared.dll");
                    //string Source = Path.Combine("$(ProjectDir)", "Intermediate/CMakeTarget/screen_capture_lite_shared/build/Release", "screen_capture_lite_shared.dll");
                    //RuntimeDependencies.Add(Destination, Source);

                    // Static Library (Only -DCMAKE_BUILD_TYPE=Release) Works
                                                            
                    //"Q:\\Wrexa_Repo\\Wrexa_Template/Intermediate/CMakeTarget/screen_capture_lite_static/build/screen_capture_lite_static/src_cpp/Release"
                    PublicIncludePaths.Add(Path.Combine(this.ModuleDirectory, "../Deps/screen_capture_lite/include"));//Is This Helpful I Do Not Know
                    PrivateIncludePaths.Add(Path.Combine(this.ModuleDirectory, "../Deps/screen_capture_lite/src_cpp"));
                    PrivateIncludePaths.Add(Path.Combine(this.ModuleDirectory, "../Deps/screen_capture_lite/src_csharp"));

                    if ((Target.Platform == UnrealTargetPlatform.Win64))
                    {
                        Console.WriteLine("CMake Added Target \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Shipping Build");
                        //CMakeTarget.add(Target, this, "screen_capture_lite_static", Path.Combine(this.ModuleDirectory, "../Deps/screen_capture_lite"), "-DBUILD_EXAMPLE=OFF -DBUILD_CSHARP=OFF -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release", false);

                        CMake_Build_Path = CMake_Build_Path + "/Win64";
                        if (!System.IO.Directory.Exists(CMake_Build_Path))
                        {
                            System.IO.Directory.CreateDirectory(CMake_Build_Path);
                        }

                        Fullcommand = "cmake -S " + CMake_Source_Path + " -B " + CMake_Build_Path;
                        Fullcommand = Fullcommand.Replace("\\", "/");

                        Fullcommand = Fullcommand + " -G " + Generator + " -A " + Architecture;
                        All_Arguments = "-DBUILD_EXAMPLE=OFF -DBUILD_CSHARP=OFF -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release";
                        Fullcommand = Fullcommand + " " + All_Arguments;

                        if (BuildHostPlatform.Current.Platform == UnrealTargetPlatform.Linux)
                        {
                            Fullcommand = " \"" + Fullcommand.Replace("\"", "\\\"") + "\"";
                        }
                        else
                        {
                            Fullcommand = " \"" + Fullcommand + " \"";
                        }
                        Console.WriteLine("Building CMake Project With These Command: " + cmd + " " + options + Fullcommand);

                        if (Start_CMD_Process(cmd, options + Fullcommand, Main_Working_Directory) == 0)
                        {
                            IsSuccessful = true;
                        }
                        else
                        {
                            Console.WriteLine("CMake Build Failed \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Shipping Build");
                        }

                        if (IsSuccessful)
                        {
                            IsSuccessful = false;
                            Console.WriteLine("CMake Build Successful For \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Shipping Build");
                            Console.WriteLine("Running Make File Build For \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Shipping Build");

                            //Fullcommand = "devenv \"" + CMake_Build_Path + "/screen_capture_lite_build.sln\" /Rebuild \"Release|Win32\"";
                            Fullcommand = "devenv \"" + CMake_Build_Path + "/screen_capture_lite_build.sln\"";
                            Fullcommand = Fullcommand.Replace("/", "\\");
                            if (BuildHostPlatform.Current.Platform == UnrealTargetPlatform.Linux)
                            {
                                Fullcommand = " \"" + Fullcommand.Replace("\"", "\\\"") + "\"";
                            }
                            else
                            {
                                Fullcommand = " \"" + Fullcommand + " \"";
                            }
                            Console.WriteLine("Building Visual Studio Project With These Command: " + cmd + " " + options + Fullcommand);

                            if (Start_CMD_Process(cmd, options + Fullcommand, Main_Working_Directory) == 0)
                            {
                                IsSuccessful = true;
                                Console.WriteLine("Visual Studio Build Successful \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Shipping Build");
                            }
                            else
                            {
                                Console.WriteLine("Visual Studio Build Failed \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Shipping Build");
                            }
                        }

                        if (IsSuccessful)
                        {
                            Fullcommand = CMake_Build_Path + "/src_cpp/Release/screen_capture_lite_static.lib";
                            Fullcommand = Fullcommand.Replace("\\", "/");

                            Console.WriteLine("Adding \"" + Fullcommand + "\" To PublicAdditonalLibraries \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Shipping Build");

                            PublicAdditionalLibraries.Add(Fullcommand);
                        }
                    }
                    else
                    {
                        Console.WriteLine("CMake Added Target \"screen_capture_lite_static\" For UnrealTargetPlatform.Linux Shipping Build");
                        //CMakeTarget.add(Target, this, "screen_capture_lite_shared", Path.Combine(this.ModuleDirectory, "../Deps/screen_capture_lite"), "-DBUILD_EXAMPLE=OFF -DBUILD_CSHARP=OFF -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release");
                        //CMakeTarget.add(Target, this, "screen_capture_lite_static", Path.Combine(this.ModuleDirectory, "../Deps/screen_capture_lite"), "-DBUILD_EXAMPLE=OFF -DBUILD_CSHARP=OFF -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Linux", false);

                        Console.WriteLine("CMake Added Target \"screen_capture_lite_static\" For UnrealTargetPlatform.Linux Shipping Build");

                        Generator = "\"Unix Makefiles\"";

                        CMake_Build_Path = CMake_Build_Path + "/Linux";
                        if (!System.IO.Directory.Exists(CMake_Build_Path))
                        {
                            System.IO.Directory.CreateDirectory(CMake_Build_Path);
                        }

                        Fullcommand = "cmake -S " + CMake_Source_Path + " -B " + CMake_Build_Path;
                        Fullcommand = Fullcommand.Replace("\\", "/");

                        CMake_ToolChain_Path = Path.GetFullPath(Path.Combine(this.ModuleDirectory, "../CMake_Linux_CrossCompile_ToolChain.cmake"));
                        CMake_ToolChain_Path = CMake_ToolChain_Path.Replace("\\", "/");
                        string contents = File.ReadAllText(CMake_ToolChain_Path);

                        Console.WriteLine("CMake Cross-Compile ToolChain Path \"" + CMake_ToolChain_Path + "\" \"screen_capture_lite_static\" For UnrealTargetPlatform.Linux Shipping Build");
                        Console.WriteLine("CMake Cross-Compile ToolChain Contents \"\n" + contents);

                        Fullcommand = Fullcommand + " -G " + Generator;// + " -A " + Architecture;
                        All_Arguments = "-DBUILD_EXAMPLE=OFF -DBUILD_CSHARP=OFF -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_TOOLCHAIN_FILE=" + CMake_ToolChain_Path;
                        Fullcommand = Fullcommand + " " + All_Arguments;

                        if (BuildHostPlatform.Current.Platform == UnrealTargetPlatform.Linux)
                        {
                            Fullcommand = " \"" + Fullcommand.Replace("\"", "\\\"") + "\"";
                        }
                        else
                        {
                            Fullcommand = " \"" + Fullcommand + " \"";
                        }
                        Console.WriteLine("Building CMake Project With These Command: " + cmd + " " + options + Fullcommand);

                        if (Start_CMD_Process(cmd, options + Fullcommand, Main_Working_Directory) == 0)
                        {
                            IsSuccessful = true;
                        }
                        else
                        {
                            Console.WriteLine("CMake Build Failed \"screen_capture_lite_static\" For UnrealTargetPlatform.Linux Shipping Build");
                        }

                        if (IsSuccessful)
                        {
                            IsSuccessful = false;
                            Console.WriteLine("CMake Build Successful For \"screen_capture_lite_static\" For UnrealTargetPlatform.Linux Shipping Build");
                            Console.WriteLine("Running Visual Studio Build For \"screen_capture_lite_static\" For UnrealTargetPlatform.Linux Shipping Build");

                            //Fullcommand = "devenv \"" + CMake_Build_Path + "/screen_capture_lite_build.sln\" /Rebuild \"Release|Win32\"";
                            Fullcommand = "cmake --build \"" + CMake_Build_Path;
                            Fullcommand = Fullcommand.Replace("/", "\\");
                            if (BuildHostPlatform.Current.Platform == UnrealTargetPlatform.Linux)
                            {
                                Fullcommand = " \"" + Fullcommand.Replace("\"", "\\\"") + "\"";
                            }
                            else
                            {
                                Fullcommand = " \"" + Fullcommand + " \"";
                            }
                            Console.WriteLine("Building Make File With These Command: " + cmd + " " + options + Fullcommand);

                            if (Start_CMD_Process(cmd, options + Fullcommand, Main_Working_Directory) == 0)
                            {
                                IsSuccessful = true;
                                Console.WriteLine("Make File Build Successful \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Shipping Build");
                            }
                            else
                            {
                                Console.WriteLine("Make File Build Failed \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Shipping Build");
                            }
                        }

                        if (IsSuccessful)
                        {
                            Fullcommand = CMake_Build_Path + "/src_cpp/Release/screen_capture_lite_static.so";
                            Fullcommand = Fullcommand.Replace("\\", "/");

                            Console.WriteLine("Adding \"" + Fullcommand + "\" To PublicAdditonalLibraries \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Shipping Build");

                            PublicAdditionalLibraries.Add(Fullcommand);
                        }
                    }


                    //PublicAdditionalLibraries.Add(Path.Combine("$(ProjectDir)", "Intermediate/CMakeTarget/screen_capture_lite_static/build/screen_capture_lite_static/src_cpp/Release", "screen_capture_lite_static.lib"));

                    //string Destination = Path.Combine("$(TargetOutputDir)", "screen_capture_lite_static.lib");
                    //string Source = Path.Combine("$(ProjectDir)", "Intermediate/CMakeTarget/screen_capture_lite_static/build/screen_capture_lite_static/src_cpp/Release", "screen_capture_lite_static.lib");
                    //RuntimeDependencies.Add(Destination, Source, StagedFileType.SystemNonUFS);
                    break;
                }
            default:
                {
                    PublicIncludePaths.Add(Path.Combine(this.ModuleDirectory, "../Deps/screen_capture_lite/include"));//Is This Helpful I Do Not Know
                    PrivateIncludePaths.Add(Path.Combine(this.ModuleDirectory, "../Deps/screen_capture_lite/src_cpp"));
                    PrivateIncludePaths.Add(Path.Combine(this.ModuleDirectory, "../Deps/screen_capture_lite/src_csharp"));
                    //../../Plugins/Wrexa_Screen_Share_Plugin/Source/ThirdParty/Wrexa_Screen_Share_PluginLibrary

                    if ((Target.Platform == UnrealTargetPlatform.Win64))
                    {
                        Console.WriteLine("CMake Added Target \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Development Or Debug Build");
                        //CMakeTarget.add(Target, this, "screen_capture_lite_static", Path.Combine(this.ModuleDirectory, "../Deps/screen_capture_lite"), "-DBUILD_EXAMPLE=OFF -DBUILD_CSHARP=OFF -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release", false);

                        CMake_Build_Path = CMake_Build_Path + "/Win64";
                        if (!System.IO.Directory.Exists(CMake_Build_Path))
                        {
                            System.IO.Directory.CreateDirectory(CMake_Build_Path);
                        }

                        Fullcommand = "cmake -S " + CMake_Source_Path + " -B " + CMake_Build_Path;
                        Fullcommand = Fullcommand.Replace("\\", "/");

                        Fullcommand = Fullcommand + " -G " + Generator + " -A " + Architecture;
                        All_Arguments = "-DBUILD_EXAMPLE=OFF -DBUILD_CSHARP=OFF -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release";
                        Fullcommand = Fullcommand + " " + All_Arguments;
                       
                        if (BuildHostPlatform.Current.Platform == UnrealTargetPlatform.Linux)
                        {
                            Fullcommand = " \"" + Fullcommand.Replace("\"", "\\\"") + "\"";
                        }
                        else
                        {
                            Fullcommand = " \"" + Fullcommand + " \"";
                        }
                        Console.WriteLine("Building CMake Project With These Command: " + cmd + " " + options + Fullcommand);
                        
                        if(Start_CMD_Process(cmd, options + Fullcommand, Main_Working_Directory) == 0)
                        {
                            IsSuccessful = true;                            
                        }
                        else
                        {
                            Console.WriteLine("CMake Build Failed \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Development Or Debug Build");
                        }

                        if(IsSuccessful)
                        {
                            IsSuccessful = false;
                            Console.WriteLine("CMake Build Successful For \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Development Or Debug Build");
                            Console.WriteLine("Running Make File Build For \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Development Or Debug Build");

                            //Fullcommand = "devenv \"" + CMake_Build_Path + "/screen_capture_lite_build.sln\" /Rebuild \"Release|Win32\"";
                            //Fullcommand = "devenv \"" + CMake_Build_Path + "/screen_capture_lite_build.sln\"";
                            Fullcommand = "msbuild \"" + CMake_Build_Path + "/screen_capture_lite_build.sln\"";
                            Fullcommand = Fullcommand.Replace("/", "\\");
                            Fullcommand = Fullcommand + " /p:Configuration=Release /p:Platform=x64 /t:screen_capture_lite_static:Rebuild /p:Project=\"screen_capture_lite/screen_capture_lite_static.vcxproj\"";
                            if (BuildHostPlatform.Current.Platform == UnrealTargetPlatform.Linux)
                            {
                                Fullcommand = " \"" + Fullcommand.Replace("\"", "\\\"") + "\"";
                            }
                            else
                            {
                                Fullcommand = " \"" + Fullcommand + "\"";
                            }
                            Console.WriteLine("Building Visual Studio Project With These Command: " + cmd + " " + options + Fullcommand);

                            if (Start_CMD_Process(cmd, options + Fullcommand, Main_Working_Directory) == 0)
                            {
                                IsSuccessful = true;
                                Console.WriteLine("Visual Studio Build Successful \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Development Or Debug Build");
                            }
                            else
                            {
                                Console.WriteLine("Visual Studio Build Failed \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Development Or Debug Build");
                            }
                        }

                        if(IsSuccessful)
                        {
                            Fullcommand = CMake_Build_Path + "/src_cpp/Release/screen_capture_lite_static.lib";
                            Fullcommand = Fullcommand.Replace("\\", "/");

                            Console.WriteLine("Adding \""+ Fullcommand + "\" To PublicAdditonalLibraries \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Development Or Debug Build");

                            PublicAdditionalLibraries.Add(Fullcommand);
                        }
                    }
                    else
                    {
                        Console.WriteLine("CMake Added Target \"screen_capture_lite_static\" For UnrealTargetPlatform.Linux Development Or Debug Build");

                        Generator = "\"Unix Makefiles\"";

                        CMake_Build_Path = CMake_Build_Path + "/Linux";
                        if (!System.IO.Directory.Exists(CMake_Build_Path))
                        {
                            System.IO.Directory.CreateDirectory(CMake_Build_Path);
                        }

                        Fullcommand = "cmake -S " + CMake_Source_Path + " -B " + CMake_Build_Path;
                        Fullcommand = Fullcommand.Replace("\\", "/");

                        CMake_ToolChain_Path = Path.GetFullPath(Path.Combine(this.ModuleDirectory, "../CMake_Linux_CrossCompile_ToolChain.cmake"));
                        CMake_ToolChain_Path = CMake_ToolChain_Path.Replace("\\", "/");
                        string contents = File.ReadAllText(CMake_ToolChain_Path);

                        Console.WriteLine("CMake Cross-Compile ToolChain Path \""+ CMake_ToolChain_Path + "\" \"screen_capture_lite_static\" For UnrealTargetPlatform.Linux Development Or Debug Build");
                        Console.WriteLine("CMake Cross-Compile ToolChain Contents \"\n" + contents);

                        Fullcommand = Fullcommand + " -G " + Generator;// + " -A " + Architecture;
                        All_Arguments = "-DBUILD_EXAMPLE=OFF -DBUILD_CSHARP=OFF -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_TOOLCHAIN_FILE=" + CMake_ToolChain_Path;
                        Fullcommand = Fullcommand + " " + All_Arguments;

                        if (BuildHostPlatform.Current.Platform == UnrealTargetPlatform.Linux)
                        {
                            Fullcommand = " \"" + Fullcommand.Replace("\"", "\\\"") + "\"";
                        }
                        else
                        {
                            Fullcommand = " \"" + Fullcommand + " \"";
                        }
                        Console.WriteLine("Building CMake Project With These Command: " + cmd + " " + options + Fullcommand);

                        if (Start_CMD_Process(cmd, options + Fullcommand, Main_Working_Directory) == 0)
                        {
                            IsSuccessful = true;
                        }
                        else
                        {
                            Console.WriteLine("CMake Build Failed \"screen_capture_lite_static\" For UnrealTargetPlatform.Linux Development Or Debug Build");
                        }

                        if (IsSuccessful)
                        {
                            IsSuccessful = false;
                            Console.WriteLine("CMake Build Successful For \"screen_capture_lite_static\" For UnrealTargetPlatform.Linux Development Or Debug Build");
                            Console.WriteLine("Running Visual Studio Build For \"screen_capture_lite_static\" For UnrealTargetPlatform.Linux Development Or Debug Build");

                            //Fullcommand = "devenv \"" + CMake_Build_Path + "/screen_capture_lite_build.sln\" /Rebuild \"Release|Win32\"";
                            Fullcommand = "cmake --build \"" + CMake_Build_Path;
                            Fullcommand = Fullcommand.Replace("/", "\\");
                            if (BuildHostPlatform.Current.Platform == UnrealTargetPlatform.Linux)
                            {
                                Fullcommand = " \"" + Fullcommand.Replace("\"", "\\\"") + "\"";
                            }
                            else
                            {
                                Fullcommand = " \"" + Fullcommand + " \"";
                            }
                            Console.WriteLine("Building Make File With These Command: " + cmd + " " + options + Fullcommand);

                            if (Start_CMD_Process(cmd, options + Fullcommand, Main_Working_Directory) == 0)
                            {
                                IsSuccessful = true;
                                Console.WriteLine("Make File Build Successful \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Development Or Debug Build");
                            }
                            else
                            {
                                Console.WriteLine("Make File Build Failed \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Development Or Debug Build");
                            }
                        }

                        if (IsSuccessful)
                        {
                            Fullcommand = CMake_Build_Path + "/src_cpp/Release/screen_capture_lite_static.so";
                            Fullcommand = Fullcommand.Replace("\\", "/");

                            Console.WriteLine("Adding \"" + Fullcommand + "\" To PublicAdditonalLibraries \"screen_capture_lite_static\" For UnrealTargetPlatform.Win64 Development Or Debug Build");

                            PublicAdditionalLibraries.Add(Fullcommand);
                        }

                        //CMakeTarget.add(Target, this, "screen_capture_lite_shared", Path.Combine(this.ModuleDirectory, "../Deps/screen_capture_lite"), "-DBUILD_EXAMPLE=OFF -DBUILD_CSHARP=OFF -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release");
                        //CMakeTarget.add(Target, this, "screen_capture_lite_static", Path.Combine(this.ModuleDirectory, "../Deps/screen_capture_lite"), "-DBUILD_EXAMPLE=OFF -DBUILD_CSHARP=OFF -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Linux", false);
                    }

                    //PublicAdditionalLibraries.Add(Path.Combine("$(ProjectDir)", "Intermediate/CMakeTarget/screen_capture_lite_static/build/screen_capture_lite_static/src_cpp/Release", "screen_capture_lite_static.lib"));

                    //string Destination = Path.Combine("$(TargetOutputDir)", "screen_capture_lite_static.lib");
                    //string Source = Path.Combine("$(ProjectDir)", "Intermediate/CMakeTarget/screen_capture_lite_static/build/screen_capture_lite_static/src_cpp/Release", "screen_capture_lite_static.lib");
                    //RuntimeDependencies.Add(Destination, Source, StagedFileType.SystemNonUFS);
                    //Destination = Path.Combine("$(BinaryOutputDir)", "screen_capture_lite_static.pdb");
                    //Source = Path.Combine("$(ProjectDir)", "Intermediate/CMakeTarget/screen_capture_lite_static/build/screen_capture_lite_static/src_cpp/Release", "screen_capture_lite_static.pdb");
                    //RuntimeDependencies.Add(Destination, Source);


                    break;
                }
        }

        return isLibrarySupported;
    }

    public bool Load_jpge_Compressor_static(ReadOnlyTargetRules Target)
    {
        bool isLibrarySupported = false;

        switch (Target.Configuration)
        {
            case UnrealTargetConfiguration.Shipping:
                {                    
                    //CMakeTarget.add(Target, this, "jpge_compressor_static", Path.Combine(this.ModuleDirectory, "../Deps/jpge_compressor"), "-DCMAKE_BUILD_TYPE=Release -DSTATIC=TRUE", true);
                    
                    PrivateIncludePaths.Add(Path.Combine(this.ModuleDirectory, "../Deps/jpge_compressor"));
                    PrivateIncludePaths.Add(Path.Combine(this.ModuleDirectory, "../Deps/jpge_compressor"));
                    //PublicAdditionalLibraries.Add(Path.Combine("$(ProjectDir)", "Intermediate/CMakeTarget/jpge_compressor_static/build/jpge_compressor_static/Release", "jpge_compressor_static.lib"));

                    //string Destination = Path.Combine("$(TargetOutputDir)", "jpge_compressor_static.lib");
                    //string Source = Path.Combine("$(ProjectDir)", "Intermediate/CMakeTarget/jpge_compressor_static/build/jpge_compressor_static/Release", "jpge_compressor_static.lib");
                    //RuntimeDependencies.Add(Destination, Source, StagedFileType.SystemNonUFS);
                    break;
                }
            default:
                {
                    // Debug Build Does Not Work

                    //CMakeTarget.add(Target, this, "jpge_compressor_static", Path.Combine(this.ModuleDirectory, "../Deps/jpge_compressor"), "-DCMAKE_BUILD_TYPE=Debug -DSTATIC=TRUE", true);

                    //PrivateIncludePaths.Add(Path.Combine(this.ModuleDirectory, "../Deps/jpge_compressor"));
                    //PrivateIncludePaths.Add(Path.Combine(this.ModuleDirectory, "../Deps/jpge_compressor"));
                    //PublicAdditionalLibraries.Add(Path.Combine("$(ProjectDir)", "Intermediate/CMakeTarget/jpge_compressor_static/build/jpge_compressor_static/Debug", "jpge_compressor_static.lib"));

                    //string Destination = Path.Combine("$(TargetOutputDir)", "jpge_compressor_static.lib");
                    //string Source = Path.Combine("$(ProjectDir)", "Intermediate/CMakeTarget/jpge_compressor_static/build/jpge_compressor_static/Debug", "jpge_compressor_static.lib");
                    //RuntimeDependencies.Add(Destination, Source, StagedFileType.SystemNonUFS);

                    //Destination = Path.Combine("$(BinaryOutputDir)", "jpge_compressor_static.pdb");
                    //Source = Path.Combine("$(ProjectDir)", "Intermediate/CMakeTarget/jpge_compressor_static/build/jpge_compressor_static/Debug", "jpge_compressor_static.pdb");
                    //RuntimeDependencies.Add(Destination, Source);

                    //Destination = Path.Combine("$(BinaryOutputDir)", "screen_capture_lite_static.pdb");
                    //Source = Path.Combine("$(ProjectDir)", "Intermediate/CMakeTarget/jpge_compressor_static/build/jpge_compressor_static/Debug", "jpge_compressor_static.pdb");
                    //RuntimeDependencies.Add(Destination, Source);
                    //break;

                    //CMakeTarget.add(Target, this, "jpge_compressor_static", Path.Combine(this.ModuleDirectory, "../Deps/jpge_compressor"), "-DCMAKE_BUILD_TYPE=Release -DSTATIC=TRUE", true);

                    PrivateIncludePaths.Add(Path.Combine(this.ModuleDirectory, "../Deps/jpge_compressor"));
                    PrivateIncludePaths.Add(Path.Combine(this.ModuleDirectory, "../Deps/jpge_compressor"));
                    //PublicAdditionalLibraries.Add(Path.Combine("$(ProjectDir)", "Intermediate/CMakeTarget/jpge_compressor_static/build/jpge_compressor_static/Release", "jpge_compressor_static.lib"));

                    //string Destination = Path.Combine("$(TargetOutputDir)", "jpge_compressor_static.lib");
                    //string Source = Path.Combine("$(ProjectDir)", "Intermediate/CMakeTarget/jpge_compressor_static/build/jpge_compressor_static/Release", "jpge_compressor_static.lib");
                    //RuntimeDependencies.Add(Destination, Source, StagedFileType.SystemNonUFS);
                    break;
                }
        }

        return isLibrarySupported;
    }
}
