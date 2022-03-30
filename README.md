# RBTray

RBTray is a small Windows program that runs in the background and allows almost
any window to be minimized to the system tray by:

- Right-Clicking its minimize button
- Shift-Right-Clicking on its title bar
- Using the Alt-Control-Down hotkey

Note that not all of these methods will work for every window, so please use
whichever one works for your needs.

Pressing Alt-Control-Up will restore all windows hidden by the RBTray.

RBTray is free, open source, and is distributed under the terms of the [GNU
General Public Licence](http://www.gnu.org/copyleft/gpl.html).

## Download

- [64 bit binaries](x64)
- [32 bit binaries](x86)
- [Original RBTray](http://sourceforge.net/projects/rbtray/files/)

## Installing

Download either the 32-bit or 64-bit binaries (depending on your OS) to a folder,
for example "`C:\Program Files\RBTray`".  Double click RBTray.exe to start it. If
you want it to automatically start after you reboot, create a shortcut to
RBTray.exe in your Start menu's Startup group.

On Windows 10 that is located at
`"C:\Users\YOUR_USERNAME_HERE\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup"`

But a ***much*** quicker way to access it is to press "`WIN` + `R`" then type
(or copy & paste) `shell:Startup`

## Using

To minimize a program to the system tray, you can use any of these methods:

- Right-click with the mouse on the program's minimize button.
- Hold the Shift key while Right-clicking on the program's title bar.
- Pressing Alt-Control-Down on the keyboard (all at the same time).

This should create an icon for the window in the system tray. To restore the
program's window, single-click the program's icon in the tray. Alternatively,
you can Right-click on the tray icon which should bring up a popup menu, then
select Restore Window. To restore all windows, press Alt-Control-Up, or click
menu item "Restore All".

In some cases the first two methods cause problems with other software because
of they way they integrate into Windows using a hook to intercept mouse events.
In this case, you can use the `--no-hook` option, which means that only the
last method of using the Alt-Control-Down hotkey will work. Also in
this case the RBHook.dll isn't needed since it only exists to support the mouse
event hook.

## Exiting

Right click on any tray icon created by RBTray and click Exit RBTray in the
popup menu, or run RBTray.exe with the `--exit` parameter.

## Authors

Nikolay Redko: http://rbtray.sourceforge.net/, https://github.com/nredko

J.D. Purcell: http://www.moitah.net/, https://github.com/jdpurcell

Benbuck Nason: https://github.com/benbuck

## Other

For original forum, bug tracker, etc. see [RBTray SourceForge project page]
(http://sourceforge.net/projects/rbtray/).

Copyright &copy; 1998-2011 Nikolay Redko, J.D. Purcell

Copyright &copy; 2015 Benbuck Nason
