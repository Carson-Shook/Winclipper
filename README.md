# Winclipper

Winclipper is a lightweight Win32 application that saves everything that you copy so that you can paste it later when you really need it. It's designed with Unicode in mind so that you can copy any character from any language or a string of emojis and know that it will paste correctly.

A built in preview ensures that you know exactly what you are pasting when a single-line preview isn't enough. Other clipboard applications have been useful to me for software development, but in my day-to-day work I regularly have to copy multiple lines of code that all look similar from the first line (lots of xml); that is the problem that Winclipper is built to solve.

The menu is simple and can be summoned with a hotkey of your choice (default: Ctrl + Shift + V), or accessed through its taskbar tray icon. It can be configured to show as much or as little information as you like.

## Building Winclipper

You will need Visual Studio 2017 to build Winclipper. The solution file should contain everything that you need to build. If you want to build the setup projects, then you will need to install [WiX](http://wixtoolset.org).

## Changelog

**1.4.2** - 2021-12-15

This release of Winclipper includes the following bug fixes:
- Fixed an issue with checkbox labels getting cut off at the end in Settings.
- Added checks to make sure the control key has its state properly restored if the user is holding it down while selecting a clip (thanks [micjoh](https://github.com/micjoh) for giving me a way to reproduce the "phtantom V issue"!). 

**1.4.1** - 2021-11-21

Quick fix for updater continuously popping up on startup.

**1.4** - 2021-11-21

This release of Winclipper includes the following features and bug fixes:
- Dark Mode! Winclipper now follows your window color settings in Windows 10 and up in the Settings menu. I might expand this in the future.
- Adds an option for "Paste immediately on click" that can be disabled so that Winclipper only adds items to the clipboard, but still lets the user do the pasting.
- Added one-time startup message to alert users if they have Windows Clipboard History turned on as it can interfere with Winclipper.
- Lots of safety refactoring to try and clean up any remaining memory issues and crashes. If you get an alert that a fatal error has occurred, please report it at https://github.com/Carson-Shook/Winclipper/issues

**1.3.4** - 2019-09-24

This release of Winclipper includes the following improvements and bug fixes:
- Bitmaps will now be rendered using nearest neighbor on displays at native resolution to produce a crisper image.
- The 16x16px icon looks a little cleaner and rounder. I adjusted a few pixels that make it look better when rendered as an icon.
- Text padding will no longer be incorrectly applied to the preview window size when displaying images.
- Removed a sizing handle that was added to the settings window by accident.
- Fixed a rare crash that could occur when using the Exit menu option in the settings window.

**1.3.3** - 2019-07-23

This release of Winclipper includes the following improvements and bug fixes:
- Winclipper will now check to see whether or not it is already running, and will not open again if it's already running.
- If Windows Explorer crashes or is restarted, Winclipper will now add it's notification icon back to the system tray.
- Fixed a nasty crash that occurred in some situations when Direct2D didn't clean itself up properly.
- Added additional safety when attempting to write a file.

**1.3.2** - 2019-06-17

This release of Winclipper includes the following bug fixes:
- Fixed a race condition that caused a build-up of orphaned bitmaps on disk. If you want to clean up any existing orphaned bitmaps, I recommend unchecking "Save clips to disk" in the Settings window, waiting at least 5 seconds (or a little longer for non-SSDs), and then checking the box again. Those who did not use this setting in the first place were not affected. Unfortunately this fix might lead to a slight loss in performance when copying large amounts of data quickly.
- Fixed a counter that was not correctly implemented that would cause a resource leak when a large bitmap failed to preview in 5 seconds.
- Added a check that prevents copying images larger than 16384 pixels in either direction because they break the Windows Imaging Component, thus breaking the preview window and causing a resource leak.
- Added a fix for non-device independent bitmaps that do not correctly handle alpha channel information. Now your bitmaps copied from Excel should actually appear in the preview!
    - Technical explanation for those who need it: if a 32-bit bitmap is of type CF_BITMAP rather than CF_DIB, then all alpha channel bytes are set to 255 since Winclipper handles all bitmaps internally as CF_DIB and otherwise cannot make the distinction about whether or not to use transparency.

**1.3.1** - 2019-05-01

This release of Winclipper includes the following bug fixes:
- Adds a de-duplication comparison to bitmaps. Unfortunately the issue did not occur on any of my test machines, only the machines of my coworkers, so I missed it. Thanks guys for helping me work out the bugs!
- It now prioritizes previewing bitmaps over text.
- If a clip contains both a bitmap and text, then the bitmap thumbnail will display alongside the text, but without the image size.

**1.3** - 2019-05-01

This release of Winclipper includes the biggest addition since its original release: Bitmap support! Now you can copy images and paste them at a later time, and of course, if you don't want to do that (or you want to reduce or increase the amount of memory to cache images) you can change that in the settings.
- Bitmaps can now be copied and pasted just like Unicode text. They appear in the menu with a tiny thumbnail and the dimensions of the image, and the pop-up preview shows the image with a max height or width of 500px.
- Added the long-awaited Delete feature. Now you can right click on a clip and click "Delete" to remove it from the list.
- The Clip format has been reworked to support multiple types of data. Your clips file will be upgraded to the new format the first time that you run this update.
- Text previews are now cached after they draw so that viewing large text previews is much faster.
- You'll now be warned when clicking "Clear Clips", giving you an opportunity to cancel in case you misclicked it.

It also includes the following bug fixes:
- Fixes a handful of tiny memory leaks.
- Fixed an issue with the Microsoft edge hack. It isn't actually needed in Windows 10 1809, and it should work a tiny bit better in previous versions.
- Clips should copy more reliably now.
- Preview offset is now more accurate, and handles differently sized menu items.
- Several security issues were fixed regarding buffer overflows and typecasting.

**1.2** - 2018-09-15

This release of Winclipper includes yet another rewritten text-preview, this time using DirectWrite, and do you know what that means? Color emojis and improved Unicode support!
- The entire underlying structure of Winclipper has been reorganized to better support future development.
- In addition to color emoji on Windows 8.1+ this release also supports the black and white emoji font on previous versions of Windows.
- Portions of the preview are now cached for improved performance after you've looked at them.
- You can now use keyboard navigation to open the Clips Menu from the menubar icon.
- Improved error handling and language safety in most files.

It also includes the following bug fixes:
- The Clips Menu invoked from the menubar icon should now correctly paste again.
- Winclipper should now more reliably paste into Microsoft Edge browser content (Note, this is a hack because Edge's window layout is truely bizarre).

**1.1.1** - 2018-06-17

This release of Winclipper includes the following improvements:
- The last active window now regains focus after dismissing Winclipper without making a selection.
- Winclipper now works better with multiple monitors, specifically, the preview should appear on the correct side of the menu, and this should help ensure that the preview is the correct size when viewed across monitors with different DPIs (hopefully)

It also includes the following bug fixes:
- Winclipper now correctly launches StupidSimpleUpdater (SSUP) so that CheckFrequency is respected.
- Upgraded SSUP to version 1.1 which includes TLS 1.2 support (as a temporary measure, the upgrades will be hosted on my GitHub Pages site so that existing users will be able to receive it without interruption).

**1.1** - 2018-03-18

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
