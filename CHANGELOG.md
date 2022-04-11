# Change Log

All notable changes to "RBTray" will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## Change history

## [4.14] fork (2022-Mar-20) - Benbuck Nason

- Change hotkey to Control-Alt-Down for simplicity and compatibility.
- Minor markdown cleanup/improvements.

## [4.13] fork (2021-Oct-09) - Benbuck Nason

- Change hotkey to Windows-Alt-Control-Down for Windows 10 and 11 compatibility.

## [4.12] fork (2021-Oct-07) - Benbuck Nason

- Change hotkey to Windows-Control-Down for Windows 11 compatibility (thanks to [Dimitri Pappas](https://github.com/fragtion)).
- Update project files to vstudio 2019

## [4.11] fork (2020-Mar-23) - Benbuck Nason

- Formatting improvements to RBTray.rc.
- Fix incorrect versions in RBTray.rc.
- Minor formatting improvements to README.md.
- Add --no-hook documentation to README.md.
- Added --no-hook command line option (thanks to [Ondrej Petrzilka](https://github.com/OndrejPetrzilka)).

## [4.10] fork (2019-Oct-03) - Benbuck Nason

- Fix missing tray icon issue (thanks to [Andrzej Włoszczyński](https://github.com/Andrzej-W)).

## [4.9] fork (2019-Oct-02) - Benbuck Nason

- Improved documentation.
- Clearer license/copyright notices (thanks to [Hugo Locurcio](https://github.com/Calinou)).
- Better error checking.
- Better command line parsing.

## [4.8] fork (2018-Jul-27) - Benbuck Nason

- Don't minimize desktop/taskbar/etc.
- Return correct value from WinMain.
- Allow processing of quit message.
- Clean up handling of WM_TASKBAR_CREATED.

## [4.7] fork (2018-Jul-21) - Benbuck Nason

- Support minimizing with Win-Alt-Down hotkey.

## [4.6] fork (2017-Aug-29) - Benbuck Nason

- Fix compatibility issue with some Windows 10 64 bit environments.

## [4.5] fork (2017-May-12) - Benbuck Nason

- Add some error handling.
- Fix bug in shift key check.
- Update project files to VS2015.

## [4.4] fork (2015-Dec-05) - Benbuck Nason

- Support minimizing by shift-right-clicking on window title bar.

## 4.3 (2011-Oct-30)

- Hide windows instead of minimizing and hiding.
- Hide windows even if they're already in the tray.
- Fix "Close Window" for Explorer windows.

## 4.2 (2011-Jun-24)

- Improve ability to minimize certain programs.

## 4.1 (2010-May-08)

- Unified mouse hook procedure improves reliability and fixes small minimize target with XP theming.
- Fixed tooltip overflow for long window captions.

## 4.0 (2010-May-06)

- Alternative mouse hook procedure for Windows Vista/7 Aero compatibility.
- Fixed 64-bit compatibility and created 64-bit compiles.
- Removed window menu additions (e.g. Minimize in tray, Always on top), hotkey, and keyboard indicator.
- Dropped Windows 9x support.
- Tray icon tooltip can display Unicode characters.
- Removes tray icon automatically if a program shows itself or exits.
- Fixed problem when minimizing Office 2007 windows.
- Won't minimize MDI child windows.
- Other small fixes and reorganized code.

[unreleased]: https://github.com/benbuck/rbtray/compare/v4.14...HEAD
[4.14]: https://github.com/benbuck/rbtray/compare/v4.13...v4.14
[4.13]: https://github.com/benbuck/rbtray/compare/v4.12...v4.13
[4.12]: https://github.com/benbuck/rbtray/compare/v4.11...v4.12
[4.11]: https://github.com/benbuck/rbtray/compare/v4.10...v4.11
[4.10]: https://github.com/benbuck/rbtray/compare/v4.9...v4.10
[4.9]: https://github.com/benbuck/rbtray/compare/v4.8...v4.9
[4.8]: https://github.com/benbuck/rbtray/compare/v4.7...v4.8
[4.7]: https://github.com/benbuck/rbtray/compare/v4.6...v4.7
[4.6]: https://github.com/benbuck/rbtray/compare/v4.5...v4.6
[4.5]: https://github.com/benbuck/rbtray/compare/v4.4...v4.5
[4.4]: https://github.com/benbuck/rbtray/releases/tag/v4.4
