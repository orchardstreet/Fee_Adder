Const strProgramTitle = "Payment Manager"
scriptDir = CreateObject("Scripting.FileSystemObject").GetParentFolderName(WScript.ScriptFullName)
strProgram = scriptDir & "\..\..\windows-build\bin\payment_manager.exe"
Dim objShortcut, objShell
Set objShell = WScript.CreateObject ("Wscript.Shell")
strLPath = objShell.SpecialFolders ("Desktop")
Set objShortcut = objShell.CreateShortcut (strLPath & "\" & strProgramTitle & ".lnk")
objShortcut.TargetPath = strProgram
objShortcut.Description = strProgramTitle
objShortcut.WorkingDirectory = scriptDir & "\..\.."
objShortcut.Save
WScript.Quit
