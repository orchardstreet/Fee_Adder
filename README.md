Payment Manager
========================

Accounting program

Compilation
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

