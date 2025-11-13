@echo off

call cls

@REM Copiando pasta assets

set DELETEGEN=.\src\gen
echo Limpando pasta destino...
if exist "%DELETEGEN%" (
    rmdir /S /Q "%DELETEGEN%"
)

echo Copiando arquivos...
xcopy ".\deep_raycast_3d.gdextension" ".\addons\deep_raycast_3d\" /I /Y /Q

echo "Setando a variável de ambiente para SCons..."
set PATH=%PATH%;C:\Users\saulo\AppData\Roaming\Python\Python314\Scripts
echo "Variável de ambiente atualizada."
echo "Iniciando o SCons com os argumentos fornecidos..."

call scons platform=windows arch=x86_32 target=template_debug
call scons platform=windows arch=x86_32 target=template_debug
call scons platform=windows arch=x86_64 target=template_release
call scons platform=windows arch=x86_64 target=template_release

call scons platform=linux arch=arm64 target=template_debug
call scons platform=linux arch=arm64 target=template_release
call scons platform=linux arch=x86_64 target=template_debug
call scons platform=linux arch=x86_64 target=template_release

call scons platform=macos arch=x86_64 target=template_debug
call scons platform=macos arch=x86_64 target=template_release

call scons platform=web target=template_debug
call scons platform=web target=template_release
