@echo off
echo Tworzenie skryptu tmp10.bat do tworzenia pomocy, czekaj...
echo ^@echo off > tmp10.bat
echo setlocal disabledelayedexpansion enableextensions >> tmp10.bat
echo.  >> tmp10.bat
rem mo¿na u¿yæ bata który wygeneruje change_log z ustawionymi zmiennymi
rem dla nowej wersji
rem filever /X intakt.exe | sed "s/.* \([0-9]\..*\) shp .* \(.*\) .*/\1, \2/"
rem filever /X intakt.exe | sed "s/.* \([0-9]\..*\) shp .*/\1/"
rem dla starej wersji
rem filever /X prev_intakt.exe | sed "s/.* \([0-9]\..*\) shp .*/\1/"

c:\sh\filever /X output\mingw\intakt.exe | c:\sh\sed "s/.* \([0-9]\..*\) shp .* \(.*\) .*/SET VERN\=\"\1, \2\"/"  >> tmp10.bat
if exist "\\filesrv\wspolne\Laser aplikacje\Bastion + Positive MS SQL\!! Pakiety !!\Hospitality\Komponenty\intakt.exe\intakt.exe" (
   c:\sh\filever /X "\\filesrv\wspolne\Laser aplikacje\Bastion + Positive MS SQL\!! Pakiety !!\Hospitality\Komponenty\intakt\intakt.exe" | c:\sh\sed "s/.* \([0-9]\..*\) shp .* \(.*\) .*/SET VERO\=\"\1, \2\"/"  >> tmp10.bat
) else (
  if exist "c:\sh\intakt.exe" (
     c:\sh\filever /X "c:\sh\intakt.exe" | c:\sh\sed "s/.* \([0-9]\..*\) shp .* \(.*\) .*/SET VERO\=\"\1, \2\"/"  >> tmp10.bat
  ) else (
  if exist "d:\sh\intakt.exe" (
     c:\sh\filever /X "d:\sh\intakt.exe" | c:\sh\sed "s/.* \([0-9]\..*\) shp .* \(.*\) .*/SET VERO\=\"\1, \2\"/"  >> tmp10.bat
  ) 
  )
)
rem filever /X prev_intakt.exe | sed "s/.* \([0-9]\..*\) shp .* \(.*\) .*/echo SET VERO\=\"\1, \2\"/"

rem echo SET VERN^="1.1.4.716, 2010-05-21 08:00:00"  >> tmp10.bat
rem echo SET VERO^="1.1.5.680, 2010-04-07 08:00:00"  >> tmp10.bat
echo SET NTAB^=10  >> tmp10.bat
echo.  >> tmp10.bat
echo ^@echo off  >> tmp10.bat
echo set CUR_DT^=2010  >> tmp10.bat
echo rem %%%%DATE:^~0,0%%%%  >> tmp10.bat
echo echo wyci¹g z daty^: "%%CUR_DT%%"  >> tmp10.bat
echo.  >> tmp10.bat
echo c:\sh\changelog.exe --dbdel  >> tmp10.bat
echo.  >> tmp10.bat
echo FOR /R %%%%I in (*.cpp *.rc *.c *.h) do (  >> tmp10.bat
echo echo %%%%I  >> tmp10.bat
echo c:\sh\changelog.exe --subdate^=%%CUR_DT%% --tab^=%%NTAB%% "%%%%I"  >> tmp10.bat
echo )  >> tmp10.bat
rem echo echo. ^>^> changelog%%CUR_DT%% >> tmp10.bat
echo echo ^^^<pre^^^> ^>^> changelog%%CUR_DT%% >> tmp10.bat
echo echo. ^>^> changelog%%CUR_DT%% >> tmp10.bat
echo c:\sh\changelog.exe --print --vern^=%%VERN%% --vero^=%%VERO%% ^>^> changelog%%CUR_DT%% >> tmp10.bat
echo echo ^^^</pre^^^> ^>^> changelog%%CUR_DT%% >> tmp10.bat
echo copy changelog%%CUR_DT%% doc\changelog >> tmp10.bat
echo copy changelog%%CUR_DT%% doc\version.txt >> tmp10.bat
echo del /q changelog%%CUR_DT%% >> tmp10.bat
echo c:\sh\changelog.exe --dbdel >> tmp10.bat
echo SET VERN^= >> tmp10.bat
echo SET VERO^= >> tmp10.bat
echo SET NTAB^= >> tmp10.bat
echo pause >> tmp10.bat
echo Koniec.
