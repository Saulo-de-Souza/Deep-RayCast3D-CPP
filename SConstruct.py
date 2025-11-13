#!/usr/bin/env python
import os
import sys

# Carrega o ambiente do godot-cpp
env = SConscript("godot-cpp/SConstruct")  # type: ignore

# Adiciona os caminhos das fontes
env.Append(CPPPATH=[
    "src/", 
    "src/DeepRayCast3D"
])

# Habilita o padrão C++17
platform = env["platform"]
target = env.get("target", "template_debug")  # Usa template_debug como padrão
arch = env.get("arch", "wasm32" if platform == "web" else "x86_64")  # Ajusta para wasm32 em web


if platform == "windows":
    env.Append(CXXFLAGS=["/std:c++17", "/O2" if target == "template_release" else "/Od"])
    shared_lib_suffix = ".dll"
    arch_name = "x86_64" if arch == "x86_64" else "x86_32"
elif platform == "linux":
    env.Append(CXXFLAGS=["-std=c++17", "-O3" if target == "template_release" else "-O0"])
    shared_lib_suffix = ".so"
    arch_name = arch
elif platform == "macos":
    env.Append(CXXFLAGS=["-std=c++17", "-O3" if target == "template_release" else "-O0"])
    shared_lib_suffix = ".dylib"
    arch_name = "universal" if arch == "x86_64" else arch
elif platform == "web":
    # Não usar as duas flags abaixo para exportar no-tread
    # env.Append(CXXFLAGS=["-std=c++17", "-sUSE_PTHREADS=1"])
    # env.Append(LINKFLAGS=["-sUSE_PTHREADS=1"])
    env.Append(CXXFLAGS=["-O3" if target == "template_release" else "-O0"])
    shared_lib_suffix = ".wasm"
    arch_name = "wasm32"
else:
    print(f"Plataforma desconhecida: {platform}")
    sys.exit(1)

# Coleta todos os arquivos .cpp de forma recursiva
sources = []
for root, _, files in os.walk("src"):
    for file in files:
        if file.endswith(".cpp"):
            sources.append(os.path.join(root, file))

# Incluir documentação
if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml")) # type: ignore
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")
        
# Define o nome base da biblioteca
library_name = "deep_raycast_3d"

# Monta o nome completo da biblioteca no formato compatível com gdextension
library_name_full = f"{library_name}.{platform}.{target}.{arch_name}"

# Define o caminho de saída
output_dir = "addons/deep_raycast_3d"
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

# Configura o nome do arquivo de biblioteca compartilhada
library_path = os.path.join(output_dir, library_name_full + shared_lib_suffix)

# Gera a biblioteca compartilhada
library = env.SharedLibrary(
    target=library_path,
    source=sources,
)

# Adiciona a biblioteca como padrão
Default(library)  # type: ignore




