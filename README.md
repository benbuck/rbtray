## RBTray

RBTray is a small Windows program that runs in the background and allows almost any window to be minimized to the system tray by:

- Right-clicking its minimize button
- Shift-right-clicking its title bar
- Using the Alt-Shift-Down hotkey

Note that not all of these methods will work for every window, so please use whichever one works for your needs.

RBTray is free, open source, and is distributed under the terms of the [GNU General Public Licence](http://www.gnu.org/copyleft/gpl.html).

Note: We made the following changes:

 - Added customizable hot key feature.
 - Made appropriate installers for both 32 and 64 bit OS.

## Download

- [64 bit binaries](x64)
- [32 bit binaries](x86)
- [Original RBTray](https://sourceforge.net/projects/rbtray/files/)

## Installing

Download either the 32-bit or 64-bit binaries (depending on your OS) to a folder, for example:
```
%PROGRAMFILES%\RBTray
```
Double click RBTray.exe to start it. If you want it to automatically start after you reboot, create a shortcut to RBTray.exe in your Start menu's Startup group.

On Windows 10/11 that is located at
```
%USERNAME%\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup
```

But a **_much_** quicker way to access it is to press "`WIN` + `R`" then type
(or copy & paste) 
```
shell:Startup
```

## Configuration

In the installation directory, there is a file named `rbtray.ini`. You can customize the shortcut in there. Currently the supported shortcuts are:

1. For the modifiers:
   - Alt+Control
   - Alt+Shift
  
2. For the keys:
   - Down
   - Up
   - M

The **default** shortcut is `Alt-Shift-Down`

## Using

To minimize a program to the system tray, you can use any of these methods:

- Right-click with the mouse on the program's minimize button.
- Hold down a Shift key while right-clicking on the program's title bar.
- Pressing Alt-Shift-Down on the keyboard (all at the same time).

This should create an icon for the window in the system tray. To restore the program's window, single-click the program's icon in the tray. Alternatively, you can right-click on the tray icon which will bring up a popup menu, then select **Restore Window**.

Note: For Microsoft Store apps or apps packaged in APPX, APPXBUNDLE, MSIX, MSIXBUNDLE, a generic program icon will be shown.

In some cases, the first two methods cause problems with other software because of they way they integrate into Windows using a hook to intercept mouse events.
In these cases, you can use the `--no-hook` option, which means that only the last method of using the Alt-Shift-Down hotkey will work. Also in this case the RBHook.dll isn't needed since it only exists to support the mouse event hook.

## Warning

If you play games that contains **Anti-Cheat** softwares, it is **RECOMMENDED** to use the **NO HOOK** method. 

## Exiting

Right-click on any tray icon created by RBTray and click **Exit RBTray** in the popup menu, or run RBTray.exe with the `--exit` parameter.

## Authors

Nikolay Redko: <http://rbtray.sourceforge.net/>, <https://github.com/nredko>

J.D. Purcell: <http://www.moitah.net/>, <https://github.com/jdpurcell>

Benbuck Nason: <https://github.com/benbuck>

## Contributors

Dimitri Pappas <https://github.com/fragtion>

Ondrej Petrzilka <https://github.com/OndrejPetrzilka>

Andrzej Włoszczyński <https://github.com/Andrzej-W>

Hugo Locurcio <https://github.com/Calinou>

Thiago V. S. Coelho: <https://github.com/thiagovscoelho>

Sk. Mashrur Sourov: <https://github.com/SMSourov>

## Other

For original forum, bug tracker, etc. see [RBTray SourceForge project page]
(<http://sourceforge.net/projects/rbtray/>).

Copyright &copy; 1998-2011 Nikolay Redko, J.D. Purcell

Copyright &copy; 2015 Benbuck Nason
