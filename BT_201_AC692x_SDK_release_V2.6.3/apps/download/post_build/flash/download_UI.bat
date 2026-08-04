@echo off  
  
@echo *** AC692X Loader *************************************************************
@echo  SDK VER    - 692x_v2.6.3
@echo  SDK PROJ   - BT_201 (NC)
@echo  --CPU--    - AC6925A4
@echo  Build Time - %time%
@echo  Build Date - %date%
@echo *******************************************************************************
PAUSE
cd %~dp0

if exist uboot.boot del uboot.boot
type uboot.bin > uboot.boot

@rem 打包烧录文件对应源代码
@rem 这样如果在找不到源代码的情况下，我们可以帮你们解压出相应的源代码（默认关闭）
rem SourcePack --proj ../../../../ --out AC692x_sdk_zip --extra .sbc .wav .raw .log .fw .res .ini .mp3

cd tone_resource
copy *.* ..\
cd ..

cd ui_resource
copy *.* ..\
cd ..

@rem 文件下载列表

isd_download.exe -tonorflash -dev br21 -boot 0x2000 -div6 -wait 300 -format cfg -format vm -f uboot.boot sdk.app bt_cfg.bin F_ASCII_S.PIX F_GB2312.TAB F_GB2312_S.PIX ui_sty.sty menu.res
@rem 删除临时文件
if exist *.mp3 del *.mp3 
if exist *.PIX del *.PIX
if exist *.TAB del *.TAB
if exist *.res del *.res
if exist *.sty del *.sty
if exist jl_692x.bin del jl_692x.bin

@rem 生成升级文件
rename jl_isd.bin jl_692x.bin
bfumake.exe -fi jl_692x.bin -ld 0x0000 -rd 0x0000 -fo updata.bfu
if exist jl_cfg.bin del jl_cfg.bin
del no_isd_file

@rem 常用命令说明
@rem -format vm        //擦除VM 区域
@rem -format cfg       //擦除BT CFG 区域
@rem -format 0x3f0-2   //表示从第 0x3f0 个 sector 开始连续擦除 2 个 sector(第一个参数为16进制或10进制都可，第二个参数必须是10进制)

ping /n 2 127.1>null
IF EXIST null del null
::pause

