@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"
echo [ASR_LOG] ASR_SELECT_GO.

@REM ----------------------------------------------------
@REM Direct call prebuild_asr.bat
@REM Param[0] ASR_NULL
@REM Param[1] ASR_WS
@REM Param[2] ASR_YJ
@REM set "ASR_SELECT=ASR_NULL"
@REM ----------------------------------------------------

@REM ----------------------------------------------------
@REM Parameter call prebuild_asr.bat
@REM set "ASR_SELECT=%~1"
@REM ----------------------------------------------------

@REM ----------------------------------------------------
@REM Read config.h call prebuild_asr.bat
set "ASR_SELECT="
for /f "tokens=3 delims= " %%a in ('findstr /c:"#define ASR_SELECT" "..\..\config.h"') do (
	echo [ASR_LOG] ASR_SELECT IS:%%a
	set "ASR_SELECT=%%a"
)
@REM ----------------------------------------------------

set "modules=..\..\..\..\platform\modules"

if "%ASR_SELECT%" == "ASR_WS"  (
    @REM echo [ASR_LOG] ASR_SELECT IS ASR_WS
	call :asr_delete true
	xcopy "!modules!\ws\mp3" "res/asr" /e /i /c /y
	xcopy "!modules!\asr_using\voice.bin" "ui/asr" /e /i /c /y
	copy  "!modules!\ws\libasr.a" "!modules!\asr_using\libasr.a" /y /v
	for /f "tokens=3 delims= " %%a in ('findstr /c:"#define ASR_PREFETCH_EN" "..\..\config.h"') do (
		echo [ASR_LOG] ASR_PREFETCH_EN IS:%%a
		if %%a == 1 (
			xcopy "!modules!\ws\weight\weight.bin" "ui/asr" /e /i /c /y
		)
	)
	goto :asr_done
) else if "%ASR_SELECT%" == "ASR_YJ" (
    @REM echo [ASR_LOG] ASR_SELECT IS ASR_YJ
	call :asr_delete true
	xcopy "!modules!\yj\mp3" "res/asr" /e /i /c /y
	xcopy "!modules!\asr_using\voice.bin" "ui/asr" /e /i /c /y
	xcopy "!modules!\yj\weight\weights.bin" "ui/asr" /e /i /c /y
	copy  "!modules!\yj\libetasr.a" "!modules!\asr_using\libasr.a" /y /v
	goto :asr_done
) else if "%ASR_SELECT%" == "ASR_WS_AIR" (
	@REM echo [ASR_LOG] ASR_SELECT IS ASR_WS_AIR
	call :asr_delete true
	xcopy "!modules!\ws_air\mp3" "res/asr" /e /i /c /y
	xcopy "!modules!\asr_using\voice.bin" "ui/asr" /e /i /c /y
	copy  "!modules!\ws_air\libasr.a" "!modules!\asr_using\libasr.a" /y /v
	for /f "tokens=3 delims= " %%a in ('findstr /c:"#define IR_AIR_FUNC" "..\..\..\..\platform\header\config_extra.h"') do (
		echo [ASR_LOG] IR_AIR_FUNC IS:%%a
		if %%a == 1 (
			copy  "!modules!\ws_air\air\libwsir.a" "!modules!\asr_using\libasr_air.a" /y /v
		)
	)
	for /f "tokens=3 delims= " %%a in ('findstr /c:"#define ASR_PREFETCH_EN" "!modules!\ws_air\asr.h"') do (
		echo [ASR_LOG] ASR_PREFETCH_EN IS:%%a
		if %%a == 1 (
			xcopy "!modules!\ws_air\weight\weight.bin" "ui/asr" /e /i /c /y
		)
	)
	goto :asr_done
) else if "%ASR_SELECT%" == "ASR_NULL" (
	@REM echo [ASR_LOG] ASR_SELECT IS ASR_NULL
	call :asr_delete true
	goto :asr_done
) else if "%ASR_SELECT%" == "" (
	echo [ASR_LOG] ASR_PARAM_NULL Please re-enter the parameters
	goto :asr_err
) else (
    echo [ASR_LOG] ASR_PARAM_ERROR Please re-enter the parameters
    goto :asr_err
)

:asr_delete
if "%~1" == "true" (
	set "del_paths[0]=res/asr"
	set "del_paths[1]=ui/asr"
	for /l %%i in (0,1,1) do (
		echo [ASR_LOG] del !del_paths[%%i]! file
		rd /s /q "!del_paths[%%i]!"
		md "!del_paths[%%i]!"
	)

	for /f "delims=" %%a in ('dir /b /a-d "!modules!\asr_using\*.a"') do (
		echo clr file: %%a
		type nul > "!modules!\asr_using\%%a"
	)

	goto :eof
)

:asr_done
echo [ASR_LOG] ASR_SELECT_SUCCEED.
exit /b 0

:asr_err
echo [ASR_LOG] ASR_SELECT_ERROR.
exit /b 1
