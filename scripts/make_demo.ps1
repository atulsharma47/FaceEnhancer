# make_demo.ps1
# Creates paired before|after images and builds demo.mp4

$ffmpeg = "C:\Users\hp\Downloads\ffmpeg-7.1.1-essentials_build\ffmpeg\bin\ffmpeg.exe"
# If you later move FFmpeg to C:\ffmpeg\bin, update this path.

$project = "C:\FaceEnhancer"
$imagesDir = Join-Path $project "images"
$outputDir = Join-Path $project "output"
$pairedDir = Join-Path $outputDir "paired"

# create paired folder
if (-not (Test-Path $pairedDir)) { New-Item -ItemType Directory -Path $pairedDir | Out-Null }

# gather input image files (case-insensitive jpg/png)
$imgFiles = Get-ChildItem -Path $imagesDir -Include *.jpg,*.JPG,*.png,*.PNG -File

foreach ($img in $imgFiles) {
    $name = $img.Name
    $inPath = $img.FullName
    $outPath = Join-Path $outputDir $name

    if (-not (Test-Path $outPath)) {
        Write-Host "Skipping (no matching output): $name" -ForegroundColor Yellow
        continue
    }

    $pairedName = "paired_$name.png"
    $pairedPath = Join-Path $pairedDir $pairedName

    $args = @("-y", "-i", $inPath, "-i", $outPath, "-filter_complex", "hstack=inputs=2", $pairedPath)
    Write-Host "Pairing: $name -> $pairedName"
    & $ffmpeg @args | Out-Null
}

# build the video from paired images
$glob = Join-Path $pairedDir "paired_*.png"
$framerate = 1
$videoOut = Join-Path $project "demo.mp4"

Write-Host "Building demo video ($videoOut) from paired images..."
$ffmpegArgs = @("-framerate", $framerate.ToString(), "-pattern_type", "glob", "-i", $glob, "-c:v", "libx264", "-r", "30", "-pix_fmt", "yuv420p", $videoOut)
& $ffmpeg @ffmpegArgs

Write-Host "✅ Done! Video created at: $videoOut"
