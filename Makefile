##############################################################################
##
##  请以`x64本机工具命令行`切换工作目录到本文件路径, 然後执行`nmake`编译
##  请自行配置 gcc 环境变量, 仅适配 gcc/g++ 编译器, 注意!!!
##
##  Copyright (c) Yanglan Network.  All rights reserved.

all: libdualmath.dll
	del *.obj

libdualmath.dll: dualmath.obj
	link /DLL /DEF:libdualmath.def /MACHINE:X64 /SUBSYSTEM:WINDOWS /NOENTRY /NODEFAULTLIB $? /OUT:$@

dualmath.obj: dualmath.c
	gcc -m64 -c -mcmodel=small -O3 -DNDEBUG -march=x86-64-v3 dualmath.c -o dualmath.obj
