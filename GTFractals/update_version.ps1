Write-Host "Running update_version.ps1"

$versionFile = "Version.rc"
$versionPattern = '(?<=#define VERSION_BUILD )\d+'

# Legge la versione corrente
$versionContent = Get-Content $versionFile
$buildNumber = [regex]::Match($versionContent, $versionPattern).Value

# Incrementa il numero di build
$newBuildNumber = [int]$buildNumber + 1
$updatedContent = [regex]::Replace($versionContent, $versionPattern, $newBuildNumber)

# Scrive la nuova versione nel file
Set-Content -Path $versionFile -Value $updatedContent
Write-Output "Updated build number to $newBuildNumber"

# Esporta la versione per GitHub Actions
$versionOutput = "0.1.$newBuildNumber"
if ($env:GITHUB_ENV) {
    "VERSION=$versionOutput" | Out-File -FilePath $env:GITHUB_ENV -Append
} else {
    Write-Output "Running locally, skipping GitHub Actions export."
}
