setlocal

pushd %~dp0

set BASEDIR=%~dp0..\

set PYDIR=c:\sdk\python-3.9.10
set path=%PYDIR%;%PYDIR%Scripts;%PATH%
set PYTHONPATH=%PYDIR%Lib\site-packages

rem it dies without this
mkdir pygame_sdl2_windeps

python setup.py build_ext

popd