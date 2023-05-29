#!/usr/bin/env sh
SOURCEDIR="$( cd "$(dirname "$0")" ; pwd -P )"
TARGETDIR="$SOURCEDIR/windows-build"

# delete TARGETDIR if it exists
echo -n "Remove any previous windows build folder..."
if [ -d "$TARGETDIR" ]; then
  echo -n "previous build present...removing..."
  rm -r $TARGETDIR
fi
echo "finished"

#delete program launcher if it exists
echo -n "Remove any previous program launcher..."
if [ -f "$SOURCEDIR/launch-program.vbs" ]; then
  echo -n "previous program launcher present...removing..."
  rm -r $SOURCEDIR/launch-program.vbs
fi
echo "finished"

#delete remote dir if it exists
echo -n "Remove any previous copy of program on C drive..."
if [ -d "/c/Payment_manager" ]; then
  echo -n "previous program copy present...removing..."
  rm -r /c/Payment_Manager
fi
echo "finished"

# create file structure
echo -n "Create windows build file structure..."
mkdir $TARGETDIR
mkdir "${TARGETDIR}/etc/"
mkdir "${TARGETDIR}/bin/"
mkdir "${TARGETDIR}/lib/"
mkdir -p "${TARGETDIR}/share/glib-2.0/schemas"
mkdir "${TARGETDIR}/share/doc"
mkdir "${TARGETDIR}/share/themes"
echo "finished"

# compile program from source and place main executable in TARGETDIR/bin
echo -n "Compile program and place main .exe in bin folder..."
gcc src/payment_manager.c -o $TARGETDIR/bin/payment_manager.exe `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`
echo "finished"

# copy app dependency library to TARGETDIR/bin
echo -n "Copy app dependency library..."
ldd "${TARGETDIR}/bin/payment_manager.exe" | grep "$MSYSTEM_PREFIX\/bin\/.*dll" -o | xargs -I{} cp -f "{}" "${TARGETDIR}/bin/"
ldd $MSYSTEM_PREFIX/bin/libjson-glib-1.0-0.dll | grep "$MSYSTEM_PREFIX\/bin\/.*dll" -o | xargs -I{} cp -f "{}" "${TARGETDIR}/bin/"

cp -f $MSYSTEM_PREFIX/bin/librsvg-2-2.dll "${TARGETDIR}/bin/"
cp -f $MSYSTEM_PREFIX/bin/libgthread-2.0-0.dll "${TARGETDIR}/bin/"
cp -f $MSYSTEM_PREFIX/bin/libgmp-10.dll "${TARGETDIR}/bin/"
echo "finished"

# copy GDBus/GSpawn-helper and dependencies files to TARGETDIR/bin
echo -n "Copy GDBus/Helper and dependencies..."
cp -f $MSYSTEM_PREFIX/bin/gdbus.exe "${TARGETDIR}/bin/"
ldd $MSYSTEM_PREFIX/bin/gdbus.exe | grep "$MSYSTEM_PREFIX\/bin\/.*dll" -o | xargs -I{} cp -f "{}" "${TARGETDIR}/bin/"
cp -f $MSYSTEM_PREFIX/bin/gspawn-win64-helper.exe "${TARGETDIR}/bin/"
ldd $MSYSTEM_PREFIX/bin/gspawn-win64-helper.exe | grep "$MSYSTEM_PREFIX\/bin\/.*dll" -o | xargs -I{} cp -f "{}" "${TARGETDIR}/bin/"
echo "finished"

# copy GTK runtime dependencies to appropriate files in TARGETDIR
echo -n "Copy GTK runtime dependencies..."
cp -rf $MSYSTEM_PREFIX/lib/gdk-pixbuf-2.0 "${TARGETDIR}/lib/"
cp -rf $MSYSTEM_PREFIX/lib/gio "${TARGETDIR}/lib/"
cp -rf $MSYSTEM_PREFIX/etc/gtk-3.0 "${TARGETDIR}/etc/"
cp -rf $MSYSTEM_PREFIX/share/icons "${TARGETDIR}/share/"
cp -rf $MSYSTEM_PREFIX/share/locale "${TARGETDIR}/share/"
glib-compile-schemas $MSYSTEM_PREFIX/share/glib-2.0/schemas
cp -f $MSYSTEM_PREFIX/share/glib-2.0/schemas/gschema* "${TARGETDIR}/share/glib-2.0/schemas"
cp -rf $MSYSTEM_PREFIX/share/themes/Default "${TARGETDIR}/share/themes/"
#cp -rf $MSYSTEM_PREFIX/share/themes/MS-Windows "${TARGETDIR}/share/themes/"
find "${TARGETDIR}/lib" -type f -path '*.dll.a' -exec rm '{}' \;
echo "finished"

# download LGPL-3.0 license and put in /share/doc
echo "Downloading the remote license file..."
if [ ! -f "${TARGETDIR}/share/doc/lgpl-3.0.txt" ]; then
  curl "https://www.gnu.org/licenses/lgpl-3.0.txt" -o "${TARGETDIR}/share/doc/lgpl-3.0.txt"
fi
if [ -f "${TARGETDIR}/share/doc/lgpl-3.0.txt" ]; then
  echo "finished"
else
  echo "failed"
fi

#create launcher script for program in TARGETDIR
echo -n "Creating launcher script for program..."
echo 'Set oShell = CreateObject ("Wscript.Shell")
Dim strArgs
strArgs = "cmd /c windows-build\bin\payment_manager.exe"
oShell.Run strArgs, 0, false' > $SOURCEDIR/launch-program.vbs
echo "finished"

#copy program to C drive (remote dir)
echo -n "Copying compiled program (payment_manager.exe), necessary dependencies, source, and launcher script to C drive..."
mkdir /c/Payment_Manager
cp -r $SOURCEDIR/* /c/Payment_Manager
echo "finished"

echo -n "Creating Desktop shortcut..."
cd c:/
wscript.exe Payment_Manager/src/scripts/create-desktop-shortcut.vbs
echo "finished"

echo -n "Creating folder shortcut..."
cd c:/
wscript.exe Payment_Manager/src/scripts/create-folder-shortcut.vbs
echo "finished"

echo "
Finished compiling and installing Windows build.  You can find the install folder in the C drive, where the save file (purchase_log.csv) also is.  
A shortcut was also placed on the desktop.  A replacement Desktop shortcut can be created anytime by double clicking create-dektop-shortcut.vbs
in the newly created program folder in C:/"


