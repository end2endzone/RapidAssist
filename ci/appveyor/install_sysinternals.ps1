# Invoke-WebRequest -Uri "https://download.sysinternals.com/files/SysinternalsSuite.zip" -OutFile "SysinternalsSuite.zip"

#function Expand-ZIPFile($file, $destination)
#{
#  $shell = new-object -com shell.application
#  $zip = $shell.NameSpace($file)
#  foreach($item in $zip.items())
#  {
#    $shell.Namespace($destination).copyhere($item)
#  }
#}

# Determine script location for PowerShell
$ScriptDir = Split-Path $script:MyInvocation.MyCommand.Path

# Download SysinternalsSuite.zip
Write-Output "Downloading $ScriptDir\SysinternalsSuite.zip ..."
$WebClient = New-Object System.Net.WebClient
$WebClient.DownloadFile("https://download.sysinternals.com/files/SysinternalsSuite.zip","$ScriptDir\SysinternalsSuite.zip")
Write-Output "done"
Write-Output ""

Write-Output "Creating '$ScriptDir\bin' directory..."
if(!(Test-Path -Path "$ScriptDir\bin" )){
  New-Item -ItemType directory -Path "$ScriptDir\bin" | Out-Null
}
Write-Output "done"
Write-Output ""

Write-Output "Extracting SysinternalsSuite.zip to '$ScriptDir\bin' directory..."
#Expand-ZipFile .\SysinternalsSuite.zip -destination .
Start-Process -FilePath "C:\Program Files\7-Zip\7z.exe" -Wait -WorkingDirectory "." -ArgumentList "x ""$ScriptDir\SysinternalsSuite.zip"" -aoa ""-o$ScriptDir\bin"""
Write-Output "done"
Write-Output ""
