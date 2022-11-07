@echo off

set rootDir=%cd%

pushd shaders
call glslc.exe --target-spv=spv1.3 --target-env=vulkan1.3 -g -O %rootDir%\shaders\forward\forward.vert                -o forward/forward.vert.spv
call glslc.exe --target-spv=spv1.3 --target-env=vulkan1.3 -g -O %rootDir%\shaders\forward\forward.frag                -o forward/deferred.frag.spv
popd

Xcopy shaders bin\shaders\ /y /e