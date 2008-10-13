[Setup]
AppName=NITRO
AppVersion=2.0-RC2
AppVerName=NITRO 2.0-RC2
AppPublisherURL=http://nitro-nitf.sourceforge.net/
AppSupportURL=http://nitro-nitf.sourceforge.net/
AppUpdatesURL=http://sourceforge.net/projects/nitro-nitf/
DefaultDirName={pf}\nitro-nitf\
DefaultGroupName=NITRO
AllowNoIcons=yes
LicenseFile=..\..\COPYING.LESSER
Compression=lzma
SolidCompression=yes
ChangesEnvironment=yes
OutputBaseFilename=nitro-2.0-rc2-setup
UninstallDisplayName=NITRO 2.0-RC2
SetupIconFile=NITRO.ico
WizardSmallImageFile=NITRO_small.bmp
WizardImageFile=NITRO.bmp
WizardImageStretch=no
Uninstallable=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Types]
Name: "full"; Description: "Full Installation"
Name: "compact"; Description: "Compact Installation"
Name: "custom"; Description: "Custom Installation"; Flags: iscustom

[Components]
Name: "core"; Description: "Core Library and Header Files"; Types: full compact custom; Flags: fixed
Name: "java"; Description: "Java Bindings"; Types: full compact
Name: "python"; Description: "Python Bindings"; Types: full compact
Name: "plugins"; Description: "Plugins"; Types: full compact
Name: "plugins\tres"; Description: "TREs"; Types: full compact
Name: "plugins\jpeg"; Description: "JPEG"; Types: full compact
Name: "plugins\jasper"; Description: "JasPer JPEG 2000"; Types: full compact
Name: "plugins\xmltre"; Description: "XMLTRE Example"; Types: full compact
Name: "docs"; Description: "Documentation"; Types: full
Name: "docs\c"; Description: "C API"; Types: full
Name: "docs\java"; Description: "Java API"; Types: full
Name: "tests"; Description: "Test Applications"; Types: full
Name: "samples"; Description: "Code Samples"; Types: full
Name: "samples\c"; Description: "C"; Types: full
Name: "samples\java"; Description: "Java"; Types: full
Name: "samples\python"; Description: "Python"; Types: full


[Files]
Source: "..\..\COPYING.LESSER"; DestDir: "{app}\share\doc\nitf"; Flags: ignoreversion; Components: core;
Source: "..\..\c\nitf\lib\win32\nitf-c.lib"; DestDir: "{app}\lib"; Flags: ignoreversion; Components: core;
Source: "..\..\c\nitf\include\*"; DestDir: "{app}\include"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: core;
Source: "..\..\c\nitf\plugins\win32\*.dll"; DestDir: "{app}\share\nitf"; Flags: ignoreversion recursesubdirs; Components: plugins\tres;
Source: "..\..\external\libjpeg-plugins\lib\win32\*.dll"; DestDir: "{app}\share\nitf"; Flags: ignoreversion recursesubdirs skipifsourcedoesntexist; Components: plugins\jpeg;
Source: "..\..\external\jasper-plugins\lib\win32\*.dll"; DestDir: "{app}\share\nitf"; Flags: ignoreversion recursesubdirs skipifsourcedoesntexist; Components: plugins\jasper;
Source: "..\..\external\example-plugins\lib\win32\*.dll"; DestDir: "{app}\share\nitf"; Flags: ignoreversion recursesubdirs skipifsourcedoesntexist; Components: plugins\xmltre;
Source: "..\..\c\nitf\tests\*.c"; DestDir: "{app}\samples\c\"; Flags: ignoreversion recursesubdirs; Components: samples\c;
Source: "..\..\c\nitf\tests\*.exe"; DestDir: "{app}\tests"; Flags: ignoreversion recursesubdirs; Components: tests;
Source: "..\..\java\nitf\lib\win32\nitf.jni-c.dll"; DestDir: "{app}\lib"; Flags: ignoreversion; Components: java;
Source: "..\..\java\nitf\target\*.jar"; DestDir: "{app}\lib"; Flags: ignoreversion; Components: java;
Source: "..\..\java\nitf\src\test\*"; DestDir: "{app}\samples\java"; Flags: ignoreversion skipifsourcedoesntexist recursesubdirs; Components: samples\java;
Source: "..\..\python\nitf\src\python\*"; DestDir: "{app}\python"; Flags: ignoreversion recursesubdirs; Components: python;
Source: "..\..\python\nitf\src\test\python\*"; DestDir: "{app}\samples\python"; Flags: ignoreversion skipifsourcedoesntexist; Components: samples\python;
Source: "..\..\python\nitf\dist\nitro-python-2.0-rc2.win32.exe"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs; Components: python;
Source: "..\..\java\nitf\apidocs\*"; DestDir: "{app}\share\doc\nitf\api\java"; Flags: ignoreversion recursesubdirs skipifsourcedoesntexist; Components: docs\java;
Source: "..\..\c\nitf\doc\html\*"; DestDir: "{app}\share\doc\nitf\api\c"; Flags: ignoreversion recursesubdirs skipifsourcedoesntexist; Components: docs\c;

[Registry]
Root: HKCU; Subkey: "Environment"; ValueType: expandsz; ValueName: "PATH_TEST"; ValueData: "{app}\lib;{olddata}"; Flags: uninsdeletekeyifempty; Components: core;
Root: HKCU; Subkey: "Environment"; ValueType: string; ValueName: "NITF_PLUGIN_PATH"; ValueData: "{app}\share\nitf"; Flags: uninsdeletekeyifempty; Components: plugins;
; In the future, possibly give an option to install for all users or just for them
;Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "PATH_TEST"; ValueData: "{app}\lib;{olddata}"; Flags: uninsdeletekeyifempty; Components: core;
;Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "NITF_PLUGIN_PATH"; ValueData: "{app}\share\nitf"; Flags: uninsdeletekeyifempty; Components: plugins;

[Icons]
Name: "{group}\{cm:UninstallProgram,NITRO}"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\nitro-python-2.0-rc2.win32.exe"; Description: "Install Python Bindings"; Flags: skipifsilent skipifdoesntexist


