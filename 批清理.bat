@echo off
echo �������obj pch idb pdb ncb opt plg res sbr ilk suo�ļ������Ե�......
pause
del /f /s /q .\*.obj
del /f /s /q .\*.pch
del /f /s /q .\*.idb
del /f /s /q .\*.pdb
del /f /s /q .\*.ncb 
del /f /s /q .\*.opt 
del /f /s /q .\*.plg
del /f /s /q .\*.res
del /f /s /q .\*.sbr
del /f /s /q .\*.ilk
del /f /s /q .\*.aps
del /f /s /q .\*.log
for /r . %%a in (Debug) do (  
  if exist %%a (
  echo "delete" %%a
  rd /s /q "%%a" 
 )
)

for /r . %%a in (Release) do (  
  if exist %%a (
  echo "delete" %%a
  rd /s /q "%%a" 
 )
)

for /r . %%a in (ipch) do (  
  if exist %%a (
  echo "delete" %%a
  rd /s /q "%%a" 
 )
)

for /r . %%a in (*.sdf) do (  
  if exist %%a (
  echo "delete" %%a
  del "%%a" 
 )
)
for /r . %%a in (*.obj) do (  
  if exist %%a (
  echo "delete" %%a
  del "%%a" 
 )
)
for /r . %%a in (*.tlog) do (  
  if exist %%a (
  echo "delete" %%a
  del "%%a" 
 )
)

echo ����ļ���ɣ�
echo. & pause