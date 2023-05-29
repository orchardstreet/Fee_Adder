Const strProgramTitle = "Payment Manager"
scriptDir = CreateObject("Scripting.FileSystemObject").GetParentFolderName(WScript.ScriptFullName)
strProgram = scriptDir & "\..\..\windows-build\bin\payment_manager.exe"
Dim objShortcut, objShell
Set objShell = WScript.CreateObject ("Wscript.Shell")
strLPath = scriptDir & "\..\.."
Set objShortcut = objShell.CreateShortcut (strLPath & "\" & strProgramTitle & ".lnk")
objShortcut.TargetPath = strProgram
objShortcut.Description = strProgramTitle
objShortcut.WorkingDirectory = strLPath
objShortcut.Save
WScript.Quit
