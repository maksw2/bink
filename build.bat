@echo off
ml64 /c /nologo /Brepro helpers.asm
cl main.cpp /c /Brepro /GS- /GR- /Oi /EHs-c- /nologo /Zc:threadSafeInit- /I include || pause && exit /b
cl take_care_of_bink.cpp /c /Brepro /GS- /GR- /Oi /EHs-c- /nologo /Zc:threadSafeInit- /I include || pause && exit /b
cl user32.cpp /c /Brepro /GS- /GR- /Oi /EHs-c- /nologo /Zc:threadSafeInit- /I include || pause && exit /b
cl ole32.cpp /c /Brepro /GS- /GR- /Oi /EHs-c- /nologo /Zc:threadSafeInit- /I include || pause && exit /b
cl kernel32.cpp /Brepro /c /GS- /GR- /Oi /EHs-c- /nologo /Zc:threadSafeInit- /I include || pause && exit /b
cl winmm.cpp /c /Brepro /GS- /GR- /Oi /EHs-c- /nologo /Zc:threadSafeInit- /I include || pause && exit /b
link helpers.obj main.obj take_care_of_bink.obj user32.obj ole32.obj kernel32.obj winmm.obj /Brepro /STACK:0x400000,0x100000 /NODEFAULTLIB /ENTRY:efi_main /NOLOGO /SUBSYSTEM:EFI_APPLICATION /OUT:BootX64.efi efilib.lib /MACHINE:X64 || pause && exit /b
copy /B /Y BootX64.efi fatroot\EFI\Boot\
