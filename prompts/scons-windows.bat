@echo off

cls

echo =====================================================================
echo Deletando a pasta gen...
echo =====================================================================
set DELETEGEN=.\src\gen
if exist "%DELETEGEN%" (
    rmdir /S /Q "%DELETEGEN%"
)

echo =====================================================================
echo Copiando arquivos...
echo =====================================================================
xcopy ".\deep_raycast_3d.gdextension" ".\addons\deep_raycast_3d\" /I /Y /Q

echo =====================================================================
echo Setando a variável de ambiente para SCons...
echo =====================================================================
set PATH=%PATH%;C:\Users\saulo\AppData\Roaming\Python\Python314\Scripts

echo =====================================================================
echo Compilando para windows debug...
echo =====================================================================
scons platform=windows arch=x86_64 target=template_debug
scons platform=windows arch=x86_32 target=template_debug

echo =====================================================================
echo Compilando para windows release...
echo =====================================================================
scons platform=windows arch=x86_64 target=template_release
scons platform=windows arch=x86_32 target=template_release

echo =====================================================================
echo Compilação windows finalizada!
echo =====================================================================