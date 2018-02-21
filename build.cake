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
var configurations = new [] { "Release" }.ToList();
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

var copyConfigAction = new Action<string> ((output) => 
{
    var outputDir = artifactsDir.Combine(output);
    var langDir = outputDir.Combine("lang");
    CopyFileToDirectory(File("./config/EncWAVtoAC3.portable"), outputDir);
    CopyFileToDirectory(File("./config/EncWAVtoAC3.presets"), outputDir);
    CleanDirectory(langDir);
    CopyFiles("./config/lang/*.txt", langDir);
});

var copyEnginesX86Action = new Action<string> ((output) => 
{
    var outputDir = artifactsDir.Combine(output);
    var aftenBinDir = (DirectoryPath)Directory("./src/aften/windows/output");
    var dirs = new [] { "libaftendll_x86", "libaftendll_x86_SSE", "libaftendll_x86_SSE2", "libaftendll_x86_SSE3" };
    foreach (var dir in dirs)
    {
        CleanDirectory(outputDir.Combine(dir));
        CopyFileToDirectory(aftenBinDir.Combine(dir).CombineWithFilePath("libaften.dll"), outputDir.Combine(dir));
    }
});

var copyEnginesX64Action = new Action<string> ((output) => 
{
    var outputDir = artifactsDir.Combine(output);
    var aftenBinDir = (DirectoryPath)Directory("./src/aften/windows/output");
    var dirs = new [] { "libaftendll_AMD64", "libaftendll_AMD64_SSE2", "libaftendll_AMD64_SSE3" };
    foreach (var dir in dirs)
    {
        CleanDirectory(outputDir.Combine(dir));
        CopyFileToDirectory(aftenBinDir.Combine(dir).CombineWithFilePath("libaften.dll"), outputDir.Combine(dir));
    }
});

var packageConfigAction = new Action(() => 
{
    var output = "EncWAVtoAC3-" + version + suffix + "-Config";
    var outputDir = artifactsDir.Combine(output);
    var outputZip = artifactsDir.CombineWithFilePath(output + ".zip");
    CleanDirectory(outputDir);
    copyConfigAction(output);
    Zip(outputDir, outputZip);
});

var packageEnginesX86Action = new Action(() => 
{
    var output = "EncWAVtoAC3-" + version + suffix + "-Engines-x86";
    var outputDir = artifactsDir.Combine(output);
    var outputZip = artifactsDir.CombineWithFilePath(output + ".zip");
    CleanDirectory(outputDir);
    copyEnginesX86Action(output);
    Zip(outputDir, outputZip);
});

var packageEnginesX64Action = new Action(() => 
{
    var output = "EncWAVtoAC3-" + version + suffix + "-Engines-x64";
    var outputDir = artifactsDir.Combine(output);
    var outputZip = artifactsDir.CombineWithFilePath(output + ".zip");
    CleanDirectory(outputDir);
    copyEnginesX64Action(output);
    Zip(outputDir, outputZip);
});

var packageBinariesAction = new Action<string,string> ((configuration, platform) => 
{
    var path = "./src/bin/" + configuration + "/" + platform + "/";
    var output = "EncWAVtoAC3-" + version + suffix + "-" + platform + (configuration == "Release" ? "" : ("-(" + configuration + ")"));
    var outputDir = artifactsDir.Combine(output);
    var outputZip = artifactsDir.CombineWithFilePath(output + ".zip");
    CleanDirectory(outputDir);
    CopyFileToDirectory(File("README.md"), outputDir);
    CopyFileToDirectory(File("COPYING.TXT"), outputDir);
    CopyFileToDirectory(File(path + "EncWAVtoAC3.exe"), outputDir);
    if (configuration == "Debug") CopyFiles(path + "*.pdb", outputDir);
    copyConfigAction(output);
    if (platform == "Win32") copyEnginesX86Action(output);
    if (platform == "x64") copyEnginesX64Action(output);
    Zip(outputDir, outputZip);
});

var packageCliBinariesAction = new Action<string,string> ((configuration, platform) => 
{
    var path = "./src/cli/bin/" + configuration + "/" + platform + "/";
    var output = "EncWAVtoAC3.CLI-" + version + suffix + "-" + platform + (configuration == "Release" ? "" : ("-(" + configuration + ")"));
    var outputDir = artifactsDir.Combine(output);
    var outputZip = artifactsDir.CombineWithFilePath(output + ".zip");
    CleanDirectory(outputDir);
    CopyFileToDirectory(File("README.md"), outputDir);
    CopyFileToDirectory(File("COPYING.TXT"), outputDir);
    CopyFileToDirectory(File(path + "EncWAVtoAC3.CLI.exe"), outputDir);
    if (configuration == "Debug") CopyFiles(path + "*.pdb", outputDir);
    copyConfigAction(output);
    if (platform == "Win32") copyEnginesX86Action(output);
    if (platform == "x64") copyEnginesX64Action(output);
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
    configurations.ForEach(configuration => platforms.ForEach(platform => packageCliBinariesAction(configuration, platform))); 
});

Task("Package-Installers")
    .IsDependentOn("Build")
    .Does(() =>
{
    configurations.ForEach(configuration => platforms.ForEach(platform => packageInstallersAction(configuration, platform)));
});

Task("Package-Config")
    .IsDependentOn("Build")
    .Does(() =>
{
    packageConfigAction();
});

Task("Package-Engines-X86")
    .IsDependentOn("Build")
    .Does(() =>
{
    packageEnginesX86Action();
});

Task("Package-Engines-X64")
    .IsDependentOn("Build")
    .Does(() =>
{
    packageEnginesX64Action();
});

///////////////////////////////////////////////////////////////////////////////
// TARGETS
///////////////////////////////////////////////////////////////////////////////

Task("Package")
  .IsDependentOn("Package-Binaries")
  .IsDependentOn("Package-Installers")
  .IsDependentOn("Package-Config")
  .IsDependentOn("Package-Engines-X86")
  .IsDependentOn("Package-Engines-X64");

Task("Default")
  .IsDependentOn("Build");

///////////////////////////////////////////////////////////////////////////////
// EXECUTE
///////////////////////////////////////////////////////////////////////////////

RunTarget(target);
