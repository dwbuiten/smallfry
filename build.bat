@echo off
cl /MT /GL /c /I. *.c
link /LTCG /out:smallfry.exe *.obj jpeg-static.lib
link /LTCG /dll /def:smallfry.def /out:smallfry.dll *.obj jpeg-static.lib