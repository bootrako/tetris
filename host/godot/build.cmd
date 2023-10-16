@echo off

if "%~1" == "" (
    set args=custom_modules="../"
)
if "%~1" == "editor" (
    set args=custom_modules="../"
)
if "%~1" == "editor_debug" (
    set args=custom_modules="../" dev_build="yes"
)
if "%~1" == "export" (
    set args=target="template_release" profile="../export.py" custom_modules="../"
)
if "%~1" == "export_web" (
    set args=platform="web" target="template_release" profile="../export.py" custom_modules="../" javascript_eval="no"
)

pushd godot
scons %args%
popd