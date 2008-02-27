[Setup]
AppName=NITRO
AppVerName=NITRO 1.5
AppPublisherURL=http://sourceforge.net/projects/nitro-nitf/
AppSupportURL=http://sourceforge.net/projects/nitro-nitf/
AppUpdatesURL=http://sourceforge.net/projects/nitro-nitf/
DefaultDirName={pf}\nitro-nitf
DefaultGroupName=NITRO
AllowNoIcons=yes
LicenseFile=..\..\COPYING.LESSER
OutputBaseFilename=setup
Compression=lzma
SolidCompression=yes
ChangesEnvironment=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Types]
Name: "full"; Description: "Full installation"
Name: "compact"; Description: "Compact installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "main"; Description: "Main Files"; Types: full compact custom; Flags: fixed
Name: "jpeg"; Description: "JPEG Plug-Ins"; Types: full

[Files]
Source: "..\..\c\nitf\lib\win32\nitf-c.lib"; DestDir: "{app}\lib"; Flags: ignoreversion
Source: "..\..\c\nitf\include\*"; DestDir: "{app}\include"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\c\nitf\shared\*.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\c\nitf\tests\*.c"; DestDir: "{app}\tests"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\c\nitf\tests\*.exe"; DestDir: "{app}\tests\bin"; Flags: ignoreversion recursesubdirs createallsubdirs
Components: jpeg; Source: "..\..\external\libjpeg-plugins\lib\win32\*.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist

[Registry]
Root: HKCU; Subkey: "Environment"; ValueType: string; ValueName: "NITF_PLUGIN_PATH"; ValueData: "{app}\plugins"; Flags: uninsdeletekeyifempty

[Icons]
Name: "{group}\{cm:UninstallProgram,NITRO}"; Filename: "{uninstallexe}"

