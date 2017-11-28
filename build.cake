///////////////////////////////////////////////////////////////////////////////
// USINGS
///////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Linq;

///////////////////////////////////////////////////////////////////////////////
// ARGUMENTS
///////////////////////////////////////////////////////////////////////////////

var target = Argument("target", "Default");

///////////////////////////////////////////////////////////////////////////////
// SETTINGS
///////////////////////////////////////////////////////////////////////////////

var platforms = new [] { "Win32", "x64" }.ToList();
var configurations = new [] { "Debug", "Release" }.ToList();
var solution = "./EncWAVtoAC3.sln";
var versionHeaderPath = (FilePath)File("./src/version.h");
var installerScript = MakeAbsolute((FilePath)File("./setup/setup.iss"));
var iscc = @"C:/Program Files (x86)/Inno Setup 5/ISCC.exe";
var artifactsDir = (DirectoryPath)Directory("./artifacts");
var binDir = (DirectoryPath)Directory("./src/bin");
var objDir = (DirectoryPath)Directory("./src/obj");

///////////////////////////////////////////////////////////////////////////////
// RELEASE
///////////////////////////////////////////////////////////////////////////////

var isAppVeyorBuild = AppVeyor.IsRunningOnAppVeyor;
var isLocalBuild = BuildSystem.IsLocalBuild;
var isPullRequest = BuildSystem.AppVeyor.Environment.PullRequest.IsPullRequest;
var isMainRepo = StringComparer.OrdinalIgnoreCase.Equals("wieslawsoltes/wavtoac3encoder", BuildSystem.AppVeyor.Environment.Repository.Name);
var isMasterBranch = StringComparer.OrdinalIgnoreCase.Equals("master", BuildSystem.AppVeyor.Environment.Repository.Branch);
var isTagged = BuildSystem.AppVeyor.Environment.Repository.Tag.IsTag 
               && !string.IsNullOrWhiteSpace(BuildSystem.AppVeyor.Environment.Repository.Tag.Name);
var isRelease =  !isLocalBuild && !isPullRequest && isMainRepo && isMasterBranch && isTagged;

///////////////////////////////////////////////////////////////////////////////
// VERSION
///////////////////////////////////////////////////////////////////////////////

var text = System.IO.File.ReadAllText(versionHeaderPath.FullPath);
var split = text.Split(new char [] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
var major = split[0].Split(new char [] { ' ' }, StringSplitOptions.RemoveEmptyEntries)[2];
var minor = split[1].Split(new char [] { ' ' }, StringSplitOptions.RemoveEmptyEntries)[2];
var revision = split[2].Split(new char [] { ' ' }, StringSplitOptions.RemoveEmptyEntries)[2];
var build = split[3].Split(new char [] { ' ' }, StringSplitOptions.RemoveEmptyEntries)[2];
var version = major + "." + minor;
var suffix = (isRelease || !isAppVeyorBuild) ? "" : "-build" + EnvironmentVariable("APPVEYOR_BUILD_NUMBER") + "-alpha";

Information("Defined Version: {0}.{1}.{2}.{3}", major, minor, revision, build);
Information("Release Version: {0}", version + suffix);

///////////////////////////////////////////////////////////////////////////////
// ACTIONS
///////////////////////////////////////////////////////////////////////////////

var buildSolutionAction = new Action<string,string,string> ((solution, configuration, platform) => 
{
    Information("Building: {0}, {1} / {2}", solution, configuration, platform);
    MSBuild(solution, settings => {
        settings.SetConfiguration(configuration);
        settings.WithProperty("Platform", "\"" + platform + "\"");
        settings.SetVerbosity(Verbosity.Minimal); });
});

var packageBinariesAction = new Action<string,string> ((configuration, platform) => 
{
    var path = "./src/bin/" + configuration + "/" + platform + "/";
    var output = "EncWAVtoAC3-" + version + suffix + "-" + platform + (configuration == "Release" ? "" : ("-(" + configuration + ")"));
    var outputDir = artifactsDir.Combine(output);
    var outputZip = artifactsDir.CombineWithFilePath(output + ".zip");
    var langDir = outputDir.Combine("Lang");
    var exeFile = File(path + "EncWAVtoAC3.exe");
    var portableFile = File("./setup/EncWAVtoAC3.portable");

    CleanDirectory(outputDir);
    CopyFileToDirectory(File("README.md"), outputDir);
    CopyFileToDirectory(File("CHANGELOG.md"), outputDir);
    CopyFileToDirectory(File("COPYING.TXT"), outputDir);
    CopyFileToDirectory(exeFile, outputDir);
    CopyFileToDirectory(portableFile, outputDir);
    CleanDirectory(langDir);
    CopyFiles("./lang/*.txt", langDir);

    var aftenBinDir = (DirectoryPath)Directory("./src/aften/windows/output");
    var aftenDll = "libaften.dll";
    var aftenDllsWin32 = new [] { "libaftendll_x86", "libaftendll_x86_SSE", "libaftendll_x86_SSE2", "libaftendll_x86_SSE3" };
    var aftenDllsX64 = new [] { "libaftendll_AMD64", "libaftendll_AMD64_SSE2", "libaftendll_AMD64_SSE3" };
    var enginesFileWin32 = File("./engines/Win32/EncWAVtoAC3.engines");
    var enginesFileX64 = File("./engines/x64/EncWAVtoAC3.engines");

    if (platform == "Win32")
    {
        CopyFileToDirectory(enginesFileWin32, outputDir);
        foreach (var dir in aftenDllsWin32)
        {
            CleanDirectory(outputDir.Combine(dir));
            CopyFileToDirectory(aftenBinDir.Combine(dir).CombineWithFilePath(aftenDll), outputDir.Combine(dir));
        }
    }

    if (platform == "x64")
    {
        CopyFileToDirectory(enginesFileX64, outputDir);
        foreach (var dir in aftenDllsX64)
        {
            CleanDirectory(outputDir.Combine(dir));
            CopyFileToDirectory(aftenBinDir.Combine(dir).CombineWithFilePath(aftenDll), outputDir.Combine(dir));
        }
    }

    Zip(outputDir, outputZip);
});

var packageInstallersAction = new Action<string,string> ((configuration, platform) => 
{
    StartProcess(iscc, new ProcessSettings { 
        Arguments = 
            "\"" + installerScript.FullPath + "\""
            + " /DCONFIGURATION=" + configuration
            + " /DBUILD=" + platform
            + " /DVERSION=" + version
            + " /DSUFFIX=" + suffix, 
        WorkingDirectory = MakeAbsolute(artifactsDir) });
});

///////////////////////////////////////////////////////////////////////////////
// TASKS
///////////////////////////////////////////////////////////////////////////////

Task("Clean")
    .Does(() =>
{
    CleanDirectory(artifactsDir);
    CleanDirectory(binDir);
    CleanDirectory(objDir);
});

Task("Build")
    .IsDependentOn("Clean")
    .Does(() =>
{
    configurations.ForEach(configuration => platforms.ForEach(platform => buildSolutionAction(solution, configuration, platform)));
});

Task("Package-Binaries")
    .IsDependentOn("Build")
    .Does(() =>
{
    configurations.ForEach(configuration => platforms.ForEach(platform => packageBinariesAction(configuration, platform)));
});

Task("Package-Installers")
    .IsDependentOn("Build")
    .Does(() =>
{
    configurations.ForEach(configuration => platforms.ForEach(platform => packageInstallersAction(configuration, platform)));
});

///////////////////////////////////////////////////////////////////////////////
// TARGETS
///////////////////////////////////////////////////////////////////////////////

Task("Package")
  .IsDependentOn("Package-Binaries")
  .IsDependentOn("Package-Installers");

Task("Default")
  .IsDependentOn("Build");

///////////////////////////////////////////////////////////////////////////////
// EXECUTE
///////////////////////////////////////////////////////////////////////////////

RunTarget(target);
