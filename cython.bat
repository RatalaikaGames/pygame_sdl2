setlocal

pushd %~dp0

set BASEDIR=%~dp0..\

set PYDIR=%BASEDIR%python27\
set path=%PYDIR%;%PATH%

rem sigh..... @#&*(@#ing python.
mklink /D c:\sdk\python27 %PYDIR%

python setup.py build_ext

popd