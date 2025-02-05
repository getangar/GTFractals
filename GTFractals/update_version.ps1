Write-Host "Running update_version.ps1"

# Path to the correct resource file
$versionFile = "resource.rc"

# Regular expression to find the build number
$versionPattern = '(?<=#define VERSION_BUILD )\d+'

# Read the content of the resource file
$versionContent = Get-Content $versionFile -Raw

# Extract the current build number using regex
$buildNumber = [regex]::Match($versionContent, $versionPattern).Value

# Increment the build number
$newBuildNumber = [int]$buildNumber + 1

# Update the build number
$updatedContent = [regex]::Replace($versionContent, $versionPattern, $newBuildNumber)

# Define the new version values
$major = 0
$minor = 1
$build = $newBuildNumber
$revision = 0  # Can remain constant

# Update FileVersion and ProductVersion while preserving indentation
$updatedContent = $updatedContent -replace 'VALUE "FileVersion", ".*?"', "VALUE `"FileVersion`", `"$major.$minor.$build.$revision`"" `
                                      -replace 'VALUE "ProductVersion", ".*?"', "VALUE `"ProductVersion`", `"$major.$minor.$build.$revision`"" 

# Ensure proper encoding for the resource file
$updatedContent | Set-Content -Path $versionFile -Encoding Unicode

Write-Output "Updated resource file with version: $major.$minor.$build.$revision"

# Export version for GitHub Actions (only if running in GitHub environment)
$versionOutput = "$major.$minor.$build.$revision"
if ($env:GITHUB_ENV) {
    "VERSION=$versionOutput" | Out-File -FilePath $env:GITHUB_ENV -Append
} else {
    Write-Output "Running locally, skipping GitHub Actions export."
}
