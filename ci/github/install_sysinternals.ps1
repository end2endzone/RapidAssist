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

# Download SysinternalsSuite.zip
Write-Output "Downloading SysinternalsSuite.zip..."
$WebClient = New-Object System.Net.WebClient
$WebClient.DownloadFile("https://download.sysinternals.com/files/SysinternalsSuite.zip","SysinternalsSuite.zip")
Write-Output "Done"
Write-Output ""

Write-Output "Creating bin directory..."
if(!(Test-Path -Path "bin" )){
  New-Item -ItemType directory -Path "bin" | Out-Null
}
Write-Output "Done"
Write-Output ""

Write-Output "Extracting SysinternalsSuite.zip to bin directory..."
#New-Item -Path "." -Name "SysinternalsSuite" -ItemType "directory" | Out-Null
#Expand-ZipFile .\SysinternalsSuite.zip -destination .
Start-Process -FilePath "C:\Program Files\7-Zip\7z.exe" -Wait -WorkingDirectory "." -ArgumentList "x","SysinternalsSuite.zip","-obin"
Write-Output "Done"
Write-Output ""
