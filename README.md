Payment Manager
========================

A simple, open source, graphical accounting program for tracking customer payments in any industry.  Written in C, and uses the open source GTK 3.0 library for the graphical interface.

This program can log payments to hard disk, present filtered and grand totals, and sort customer payments by various parameters.  The program logs the date, customer, method, and amount of each payment.

Unlike most Excel solutions, the program tries to prevent a user from entering unsortable data.  Also unlike Excel, the default save format is an easy-to-read .csv file, which is also compatible with Excel, LibreOffice Calc etc.

Compilation Requirements
------------
The default gcc and C standard library on modern systems should work just fine

* A 64-bit, C99 compiler
* A modern C standard library
* GTK 3.0 C library

Compilation Tutorials
------------
### Windows 11
Download msys2 building platform from https://www.msys2.org or from the following direct link:

https://github.com/msys2/msys2-installer/releases/download/2023-05-26/msys2-x86_64-20230526.exe

Click on Windows start menu and type in "mingw64"

Click on "MSYS2 MINGW64".  A command line will pop up.   Enter the following in this command line. Answer 'y' to prompts which ask for yes or no. Press "enter" on prompts that ask for a number selection.

```bash
pacman -Syu

pacman -S unzip mingw-w64-x86_64-toolchain nano base-devel mingw-w64-x86_64-gtk3

wget https://github.com/orchardstreet/fee_adder/archive/refs/heads/master.zip

unzip master.zip

cd Payment_Manager-master

chmod +x build-windows.sh

./build-windows.sh
```

The program folder is now completely compiled and installed.  It now resides in the C:\ drive at C:\Payment_Manager.

You can launch the program by clicking on the shortcut in that folder named "Payment Manager" or on the Desktop shortcut of the same name.  It's recommended to click on these shortcuts rather than the main .exe file in /windows-build/bin.   If it works, you can uninstall msys2 entirely if you want, and the program will work without msys2.

### Debian-based systems
Type the following into the command line:

```bash
sudo apt install libgtk-3-dev build-essential unzip wget -y

wget https://github.com/orchardstreet/Payment_Manager/archive/refs/heads/master.zip

unzip master.zip

cd Payment_Manager-master

chmod +x compile.sh

./compile.sh

./payment_manager
```

