[Setup]
AppName=NITRO
AppVerName=NITRO 1.5-RC2
AppPublisherURL=http://sourceforge.net/projects/nitro-nitf/
AppSupportURL=http://sourceforge.net/projects/nitro-nitf/
AppUpdatesURL=http://sourceforge.net/projects/nitro-nitf/
DefaultDirName={pf}\nitro-nitf
DefaultGroupName=NITRO
AllowNoIcons=yes
LicenseFile=..\..\COPYING.LESSER
Compression=lzma
SolidCompression=yes
ChangesEnvironment=yes
OutputBaseFilename=NITRO-1.5-RC2-setup

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Types]
Name: "full"; Description: "Full Installation"
Name: "compact"; Description: "Compact Installation"
Name: "custom"; Description: "Custom Installation"; Flags: iscustom

[Components]
Name: "core"; Description: "Core Library and Header Files"; Types: full compact custom; Flags: fixed
Name: "tres"; Description: "TRE Plug-Ins"; Types: full compact
Name: "jpeg"; Description: "JPEG Plug-Ins"; Types: full compact
Name: "tests"; Description: "Tests"; Types: full



[Files]
Source: "..\..\c\nitf\lib\win32\nitf-c.lib"; DestDir: "{app}\lib"; Flags: ignoreversion; Components: core;
Source: "..\..\c\nitf\include\*"; DestDir: "{app}\include"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: core;
Source: "..\..\c\nitf\shared\*.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: tres;
Source: "..\..\c\nitf\tests\*.c"; DestDir: "{app}\tests"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: tests;
Source: "..\..\c\nitf\tests\*.exe"; DestDir: "{app}\tests\bin"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: tests;
Source: "..\..\external\libjpeg-plugins\lib\win32\*.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: jpeg;

[Registry]
Root: HKCU; Subkey: "Environment"; ValueType: string; ValueName: "NITF_PLUGIN_PATH"; ValueData: "{app}\plugins"; Flags: uninsdeletekeyifempty; Components: tres jpeg;

[Icons]
Name: "{group}\{cm:UninstallProgram,NITRO}"; Filename: "{uninstallexe}"

