Payment Manager
========================

A simple, open source, graphical accounting program to manage customer payments in any industry.  Written in C, and uses GTK 3.0

It can log payments to hard disk, present filtered and grand totals, and sort customer payments by various parameters.  The program logs the date, customer, method, and amount of each payment.

Unlike most Excel solutions, the program tries to prevent a user from entering unsortable data.  Also unlike Excel, the default save format is an easy-to-read .csv file, which is also compatible with Excel, LibreOffice Calc etc.

The only dependencies are any modern C standard library which supports C99 and the open source GTK library for the GUI.  Windows compilation is of course more involved than Linux and will require the open source MYS2

Requirements
------------
###Software
tl;dr default gcc on modern systems should work just fine

* A 64-bit, C99 compiler compiler that uses 1 byte for char, 4 bytes for int, and 8 bytes for long long (this is default on modern, 64-bit Linux and Windows systems)
* GTK 3.0 C library

###Hardware
* 64-bit CPU

Compilation tutorials
------------
### Windows 11
Download msys2 building platform from https://www.msys2.org or from the following direct link:

https://github.com/msys2/msys2-installer/releases/download/2023-05-26/msys2-x86_64-20230526.exe

Click on Windows start menu and type in "mingw64"

Click on "MSYS2 MINGW64", a command line will pop up.   Enter the following in this command line:

```bash
wget https://github.com/orchardstreet/fee_adder/archive/refs/heads/master.zip

unzip master.zip

cd fee_adder-master

chmod +x build-windows.sh

./build-windows.sh
```

Open Windows file manager and go to the C drive (C:\), this is where your program now resides

Click on "Fee_Addr" in the C drive

Click on "launch-program"

### Debian-based systems
Type the following into the command line:

```bash
sudo apt install libgtk-3-dev build-essential unzip wget -y

wget https://github.com/orchardstreet/fee_adder/archive/refs/heads/master.zip

unzip master.zip

cd fee_adder-master

chmod +x compile.sh

./compile.sh

./fee_adder
```

