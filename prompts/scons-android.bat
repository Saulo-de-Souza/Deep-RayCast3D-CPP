@echo off
cls

echo ========================================
echo 🔧 Compilando Deep RayCast3D GDExtension
echo ========================================

REM ==============================
REM Limpar pasta gerada (src/gen)
REM ==============================
set DELETEGEN=.\src\gen
echo Limpando pasta de geração...
if exist "%DELETEGEN%" (
    rmdir /S /Q "%DELETEGEN%"
)

REM ==============================
REM Copiar arquivo .gdextension
REM ==============================
echo Copiando arquivo deep_raycast_3d.gdextension...
xcopy ".\deep_raycast_3d.gdextension" ".\addons\deep_raycast_3d\" /I /Y /Q

REM ==============================
REM Configurar PATH para o SCons
REM ==============================
echo Setando variáveis de ambiente...
set PATH=%PATH%;C:\Users\saulo\AppData\Roaming\Python\Python314\Scripts
echo PATH atualizado com sucesso.

echo ========================================
echo 🚀 Iniciando builds com SCons...
echo ========================================

REM ------------------------------
REM 🤖 ANDROID
REM ------------------------------
call scons platform=android arch=arm64v8 target=template_debug
call scons platform=android arch=arm64v8 target=template_release
call scons platform=android arch=arm32v7 target=template_debug
call scons platform=android arch=arm32v7 target=template_release

echo =====================================================================
echo Copiando pasta addons para o projeto example...
echo =====================================================================
xcopy ".\addons" ".\example\addons" /E /I /Y /Q

echo ========================================
echo ✅ Compilação finalizada com sucesso!
echo ========================================



@REM scons target=template_release platform=windows symbols_visibility=hidden optimize=speed generate_bindings=yes precision=double arch=x86_64
@REM scons platform=web target=template_debug dlink_enabled=yes threads=no