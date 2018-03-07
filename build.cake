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
var tests = new [] { }.ToList();
var solution = "./EncWAVtoAC3.sln";
var versionHeaderPath = (FilePath)File("./src/version.h");
var installerScript = MakeAbsolute((FilePath)File("./setup/setup.iss"));
var iscc = @"C:/Program Files (x86)/Inno Setup 5/ISCC.exe";
var artifactsDir = (DirectoryPath)Directory("./artifacts");

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
// SETUP
///////////////////////////////////////////////////////////////////////////////

Setup(context =>
{
    context.Log.Verbosity = Verbosity.Normal;
});

///////////////////////////////////////////////////////////////////////////////
// TEARDOWN
///////////////////////////////////////////////////////////////////////////////

Teardown(context =>
{
    Information("Finished running tasks.");
});

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

var runTestAction = new Action<string,string,string> ((test, configuration, platform) =>
{
    Information("Test: {0}, {1} / {2}", test, configuration, platform);
    var pattern = "./tests/" + test + "/bin/" + configuration + "/" + platform + "/" + test + ".dll";
    VSTest(pattern, new VSTestSettings() {
        PlatformArchitecture = (platform == "Win32" || platform == "x86") ? VSTestPlatform.x86 : VSTestPlatform.x64,
        InIsolation = (platform == "Win32" || platform == "x86") ? false : true,
        Logger = "AppVeyor" });
});

var copyConfigAction = new Action<string> ((output) =>
{
    var outputDir = artifactsDir.Combine(output);
    var langDir = outputDir.Combine("lang");
    Information("Copy config: {0}", output);
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
    Information("Copy engines x86: {0}", output);
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
    Information("Copy engines x64: {0}", output);
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
    Information("Package config: {0}", output);
    CleanDirectory(outputDir);
    copyConfigAction(output);
    Zip(outputDir, outputZip);
});

var packageEnginesX86Action = new Action(() =>
{
    var output = "EncWAVtoAC3-" + version + suffix + "-Engines-x86";
    var outputDir = artifactsDir.Combine(output);
    var outputZip = artifactsDir.CombineWithFilePath(output + ".zip");
    Information("Package engines x86: {0}", output);
    CleanDirectory(outputDir);
    copyEnginesX86Action(output);
    Zip(outputDir, outputZip);
});

var packageEnginesX64Action = new Action(() =>
{
    var output = "EncWAVtoAC3-" + version + suffix + "-Engines-x64";
    var outputDir = artifactsDir.Combine(output);
    var outputZip = artifactsDir.CombineWithFilePath(output + ".zip");
    Information("Package engines x64: {0}", output);
    CleanDirectory(outputDir);
    copyEnginesX64Action(output);
    Zip(outputDir, outputZip);
});

var packageGuiBinariesAction = new Action<string,string> ((configuration, platform) =>
{
    var path = "./src/bin/" + configuration + "/" + platform + "/";
    var output = "EncWAVtoAC3-" + version + suffix + "-" + platform + (configuration == "Release" ? "" : ("-(" + configuration + ")"));
    var outputDir = artifactsDir.Combine(output);
    var outputZip = artifactsDir.CombineWithFilePath(output + ".zip");
    Information("Package binaries: {0}, {1} / {2}", output, configuration, platform);
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
    Information("Package binaries: {0}, {1} / {2}", output, configuration, platform);
    CleanDirectory(outputDir);
    CopyFileToDirectory(File("README.md"), outputDir);
    CopyFileToDirectory(File("COPYING.TXT"), outputDir);
    CopyFileToDirectory(File(path + "EncWAVtoAC3.CLI.exe"), outputDir);
    if (configuration == "Debug") CopyFiles(path + "*.pdb", outputDir);
    CopyFileToDirectory(File("./config/EncWAVtoAC3.presets"), outputDir);
    if (platform == "Win32") copyEnginesX86Action(output);
    if (platform == "x64") copyEnginesX64Action(output);
    Zip(outputDir, outputZip);
});

var packageInstallersAction = new Action<string,string> ((configuration, platform) =>
{
    Information("Package installer: {0} / {1}", configuration, platform);
    StartProcess(iscc, new ProcessSettings { 
        Arguments = 
            "/Q "
            + "\"" + installerScript.FullPath + "\""
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
    CleanDirectories("./**/bin/**");
    CleanDirectories("./**/obj/**");
});

Task("Build")
    .IsDependentOn("Clean")
    .Does(() =>
{
    configurations.ForEach(c => platforms.ForEach(p => buildSolutionAction(solution, c, p)));
});

Task("Run-Unit-Tests")
    .IsDependentOn("Build")
    .Does(() =>
{
    configurations.ForEach(c => platforms.ForEach(p => tests.ForEach(t => runTestAction(t, c, p))));
});

Task("Package-Binaries")
    .IsDependentOn("Run-Unit-Tests")
    .Does(() =>
{
    configurations.ForEach(c => platforms.ForEach(p => packageGuiBinariesAction(c, p)));
    configurations.ForEach(c => platforms.ForEach(p => packageCliBinariesAction(c, p)));
});

Task("Package-Installers")
    .IsDependentOn("Run-Unit-Tests")
    .Does(() =>
{
    configurations.ForEach(c => platforms.ForEach(p => packageInstallersAction(c, p)));
});

Task("Package-Config")
    .IsDependentOn("Build")
    .Does(() =>
{
    packageConfigAction();
});

Task("Package-Engines")
    .IsDependentOn("Build")
    .Does(() =>
{
    packageEnginesX86Action();
    packageEnginesX64Action();
});

///////////////////////////////////////////////////////////////////////////////
// TARGETS
///////////////////////////////////////////////////////////////////////////////

Task("Package")
  .IsDependentOn("Package-Binaries")
  .IsDependentOn("Package-Installers")
  .IsDependentOn("Package-Config")
  .IsDependentOn("Package-Engines");

Task("Default")
  .IsDependentOn("Run-Unit-Tests");

///////////////////////////////////////////////////////////////////////////////
// EXECUTE
///////////////////////////////////////////////////////////////////////////////

RunTarget(target);
