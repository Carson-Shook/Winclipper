# Winclipper

Winclipper is a lightweight Win32 application that saves everything that you copy so that you can paste it later when you really need it. It's designed with Unicode in mind so that you can copy any character from any language or a string of emojis and know that it will paste correctly.

A built in preview ensures that you know exactly what you are pasting when a single-line preview isn't enough. Other clipboard applications have been useful to me for software development, but in my day-to-day work I regularly have to copy multiple lines of code that all look similar from the first line (lots of xml); that is the problem that Winclipper is built to solve.

The menu is simple and can be summoned with a hotkey of your choice (default: Ctrl + Shift + V), or accessed through its taskbar tray icon. It can be configured to show as much or as little information as you like.

## Building Winclipper

You will need Visual Studio 2017 to build Winclipper. The solution file should contain everything that you need to build. If you want to build the setup projects, then you will need to install [WiX](http://wixtoolset.org).

## Changelog


This release of Winclipper includes a rewritten text-preview that now uses GDI. You should find it much faster, and as a bonus, it now includes a count of the number of lines not visible in the preview (when text is not larger than 80KB) and also includes the total size of the text in KB.

It also now has a built in updater! Now updates should appear automatically when they are available. Don't worry though, if you don't want to update for some reason, you can skip it.
(Developer Note: I plan on releasing the source for the updater in the near future, just as soon as I get the documentation written up.)

It also fixes a number of bugs, including:
- Substantially improves handling of large amounts of text when displaying the preview. 
- Fixes the preview appearing in the wrong place when scrolling through a long list of clips.
- Fixes clip duplication so that contiguous clips with the same content do not create duplicate entries.
- Fixes a memory leak caused by the text length measurement of the preview.
- Added missing drop-shadow to the preview window.

**1.0**      2018-01-01      Initial Release!