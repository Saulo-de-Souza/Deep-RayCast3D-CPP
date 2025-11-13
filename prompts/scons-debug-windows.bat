@echo off

@REM Copiando pasta assets
set DEST=.\my_game_here\assets\nodes_icons
echo Limpando pasta destino...
if exist "%DEST%" (
    rmdir /S /Q "%DEST%"
)
echo Copiando arquivos...
xcopy ".\assets\" ".\my_game_here\assets" /E /I /Y /Q

xcopy ".\my_game_here.gdextension" ".\my_game_here\bin\" /I /Y /Q

echo "Setando a variável de ambiente para SCons..."
set PATH=%PATH%;C:\Users\saulo\AppData\Roaming\Python\Python314\Scripts
echo "Variável de ambiente atualizada."
echo "Iniciando o SCons com os argumentos fornecidos..."

scons target=template_debug platform=windows symbols_visibility=visible optimize=debug generate_bindings=yes precision=single arch=x86_64