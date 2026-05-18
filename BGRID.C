/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
//   HWND_DESKTOP background grid
//
//    This program displays a grid on the PM desktop background,
//    and shows the current mouse pointer position when the pointer
//    is over the BGRID background window.  This is helpful in
//    estimating or measuring sizes for windows, graphics, etc.
//
//    Double-click on the backround window to end it.
//
//   Gunnar P. Seaburg
//   IBM SID Houston
//   1989, 1990
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define INCL_GPI
#define INCL_WIN
#include <stdlib.h>
#include <stdio.h>
#include <os2.h>
#include "bgrid.h"

#define DEF_MAIN_WND         100

void QueryFontSize (HWND hwnd, LONG * cxChar, LONG * cyChar);
MRESULT EXPENTRY ClientWndProc (HWND, ULONG, MPARAM, MPARAM);

POINTL screen; /* size of screen */
HWND hwndFrame;

int main (void)

{
static CHAR szClientClass [] = "BGrid";

static ULONG flFrameFlags = FCF_SHELLPOSITION;
HAB  hab;
HMQ  hmq;
HWND hwndClient;
QMSG qmsg;

/* - - - - */

hab = WinInitialize (0);
hmq = WinCreateMsgQueue (hab, 0);

WinRegisterClass (hab, (PCCH) szClientClass, (PFNWP) ClientWndProc, CS_SIZEREDRAW, sizeof(PVOID));

screen.x = WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN);
screen.y = WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN);

hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE, &flFrameFlags,
                                (PCSZ) szClientClass, NULL, 0L, 0,
                                DEF_MAIN_WND, &hwndClient);

WinSetWindowPos (hwndFrame, HWND_BOTTOM, 0, 0,   /* x, y */
                 (LONG)screen.x, (LONG)screen.y,    /* cx, cy */
                 SWP_SIZE | SWP_MOVE | SWP_ZORDER | SWP_SHOW);

while (WinGetMsg(hab, &qmsg, 0, 0, 0)) WinDispatchMsg (hab, &qmsg);

WinDestroyWindow (hwndFrame);
WinDestroyMsgQueue (hmq);
WinTerminate (hab);
return 0;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

void QueryFontSize (HWND hwnd, LONG * cxChar, LONG * cyChar)

{
FONTMETRICS  fm;
HPS  hps;

hps = WinGetPS (hwnd);
GpiQueryFontMetrics ( hps, (LONG) sizeof fm, &fm ) ;
*cxChar = (LONG) fm.lAveCharWidth;
*cyChar = (LONG) fm.lMaxBaselineExt;
WinReleasePS (hps) ;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

MRESULT EXPENTRY ClientWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)

{
HPS       hps;
static LONG cxChar;     // size of average char
static LONG cyChar;
static RECTL     rcl;
  POINTL pt, ptlButton;
static CHAR   szString [20];
  int     i;
static HWND hwndCoord;

/* - - - - */

switch (msg) {
     case WM_PAINT:

          WinSetWindowPos (hwndFrame, HWND_BOTTOM, 0, 0, 0, 0, SWP_ZORDER);

          hps = WinBeginPaint (hwnd, 0, NULL);
          WinQueryWindowRect (hwnd, &rcl);
          WinFillRect (hps, &rcl, SYSCLR_BACKGROUND);

          // draw grid

          GpiSetColor( hps, SYSCLR_WINDOWTEXT );
          GpiSetBackColor( hps, SYSCLR_WINDOW );
          GpiSetBackMix( hps, BM_OVERPAINT );

          for (i=0;i<max((LONG)screen.x, (LONG)screen.y) ;i=i+10) {

             pt.x = i;                                         /* vertical */
             pt.y = 0;
             GpiSetCurrentPosition (hps, &pt);
             pt.y = screen.y;

             if ((i % 100) == 0) {
                 GpiSetLineType (hps, LINETYPE_SOLID    );
                 GpiSetColor(hps, CLR_DEFAULT);
             } else {
                 GpiSetLineType (hps, LINETYPE_DOT      );
                 GpiSetColor(hps, CLR_DARKGRAY);
             } /* endif */

             GpiLine (hps, &pt);

             pt.y = i;                                         /* horizontal */
             pt.x = 0;
             GpiSetCurrentPosition (hps, &pt);
             pt.x = screen.x;

             if ((i % 100) == 0) {
                 GpiSetLineType (hps, LINETYPE_SOLID    );
                 GpiSetColor(hps, CLR_DEFAULT);
             } else {
                 GpiSetLineType (hps, LINETYPE_DOT      );
                 GpiSetColor(hps, CLR_DARKGRAY);
             } /* endif */

             GpiLine (hps, &pt);

          } /* endfor */

          WinEndPaint (hps);
          return (MRESULT) FALSE;

     case WM_BUTTON1DOWN:
     case WM_BUTTON1UP:
     case WM_BUTTON2DOWN:
     case WM_BUTTON2UP:
          WinSetWindowPos (hwndFrame, HWND_BOTTOM, 0, 0, 0, 0, SWP_ZORDER);
          return (MRESULT) TRUE;

     case WM_MOUSEMOVE:
       // WinSetWindowPos (hwndFrame, HWND_BOTTOM, 0, 0, 0, 0, SWP_ZORDER);
          ptlButton.x = MOUSEMSG (&msg)-> x;
          ptlButton.y = MOUSEMSG (&msg)-> y;
          sprintf (szString, "(%3ld, %3ld)", ptlButton.x, ptlButton.y);
          WinSetWindowText (hwndCoord, (PCSZ) szString);
          break;

     case WM_BUTTON1DBLCLK:                       /* exit */
     case WM_BUTTON2DBLCLK:                       /* exit */
          WinPostMsg(hwnd, WM_QUIT, 0L, 0L);
          break;

     case WM_CREATE:
          QueryFontSize (hwnd, &cxChar, &cyChar);
          hwndCoord = WinCreateWindow (hwnd, WC_STATIC, (PCSZ) "Hello",  SS_TEXT | WS_VISIBLE,
                     0, (LONG)(screen.y-cyChar), 12*cxChar, cyChar, hwnd, HWND_TOP, 1, NULL, NULL);
          break;

     } /* switch */

return WinDefWindowProc (hwnd, msg, mp1, mp2);
}
