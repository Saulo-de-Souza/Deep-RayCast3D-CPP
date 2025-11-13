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

scons

