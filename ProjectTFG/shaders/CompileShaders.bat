@echo off
set GLSL=C:\VulkanSDK\1.4.313.2\Bin\glslangValidator.exe

echo Compilando Shaders de Escena y Skybox...
%GLSL% -V scene_shader.vert -o scene_vert.spv
%GLSL% -V scene_shader.frag -o scene_frag.spv
%GLSL% -V skybox_shader.vert -o skybox_vert.spv
%GLSL% -V skybox_shader.frag -o skybox_frag.spv

echo.
echo Compilando Shaders de Particulas...
%GLSL% -V particles_shader.vert -o particles_vert.spv
%GLSL% -V particles_shader.frag -o particles_frag.spv
%GLSL% -V fire.comp -o fire_comp.spv
%GLSL% -V smoke.comp -o smoke_comp.spv
%GLSL% -V water.comp -o water_comp.spv

echo.
echo === Compilacion finalizada ===
pause