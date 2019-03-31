setlocal

pushd %~dp0

set BASEDIR=%~dp0..\

set PYDIR=%BASEDIR%python27-for-tools\
set path=%PYDIR%;%PYDIR%Scripts;%PATH%

rem sigh..... @#&*(@#ing python.
mklink /D c:\python27 %PYDIR%

python setup.py build_ext

popd