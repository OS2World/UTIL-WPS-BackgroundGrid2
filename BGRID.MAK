# - - - - - - - - - - -
# bgrid --  Background grid for desktop
# - - - - - - - - - - -

CDEBUG= -Zi -Zd -Od
CC  = cl -u -c -Alfu -G2s -W3 -Zpe -B1 d:\c\bin\c1l.exe $(CDEBUG)

# - - - - - - - - - - -
.c.obj:
    $(CC) $*.c
# - - - - - - - - - - -

bgrid.exe:  bgrid.obj   \
               bgrid.def   \
               bgrid.mak
     link bgrid,,,os2,bgrid

bgrid.obj:  bgrid.c
