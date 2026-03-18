param(
	[string]$RepoRoot = (Get-Location).Path
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

$RepoRoot = (Resolve-Path $RepoRoot).Path
$targetDir = Join-Path $RepoRoot "UI/Assets/Proposed/ColorBold"
New-Item -ItemType Directory -Force -Path $targetDir | Out-Null
$defaultScale = 1.08

# Current icon -> MDI source icon + color
$map = @(
	@{ Current = "Help.png"; Mdi = "help-circle"; Color = "#1e88e5"; WhiteGlyph = $true; Scale = 1.08 },
	@{ Current = "Warning.png"; Mdi = "alert"; Color = "#f9a825" },
	@{ Current = "Error.png"; Mdi = "alert-circle"; Color = "#e53935" },
	@{ Current = "Record.png"; Mdi = "record-circle"; Color = "#d32f2f"; Scale = 1.08 },
	@{ Current = "MediaPlay.png"; Mdi = "play"; Color = "#2e7d32" },
	@{ Current = "MediaPause.png"; Mdi = "pause"; Color = "#fdd835" },
	@{ Current = "MediaStop.png"; Mdi = "stop"; Color = "#c62828" },
	@{ Current = "Close.png"; Mdi = "close-thick"; Color = "#d32f2f"; Scale = 1.18 },
	@{ Current = "Settings.png"; Mdi = "cog"; Color = "#546e7a" },
	@{ Current = "Find.png"; Mdi = "magnify"; Color = "#00838f" },
	@{ Current = "Refresh.png"; Mdi = "refresh"; Color = "#1565c0" },
	@{ Current = "ResetSettings.png"; Mdi = "backup-restore"; Color = "#ab47bc" },
	@{ Current = "Folder.png"; Mdi = "folder"; Color = "#fbc02d" },
	@{ Current = "Script.png"; Mdi = "script-text"; Color = "#3949ab" },
	@{ Current = "LogWindow.png"; Mdi = "file-document-outline"; Color = "#455a64" },
	@{ Current = "Debugger.png"; Mdi = "bug"; Color = "#2e7d32" },
	@{ Current = "Speed.png"; Mdi = "speedometer"; Color = "#00897b" },
	@{ Current = "HistoryViewer.png"; Mdi = "history"; Color = "#6d4c41" },
	@{ Current = "VideoOptions.png"; Mdi = "video-outline"; Color = "#1976d2" },
	@{ Current = "Repeat.png"; Mdi = "repeat"; Color = "#0277bd" },
	@{ Current = "Add.png"; Mdi = "plus-thick"; Color = "#2e7d32" },
	@{ Current = "Edit.png"; Mdi = "pencil"; Color = "#ef6c00" },
	@{ Current = "Copy.png"; Mdi = "content-copy"; Color = "#3949ab" },
	@{ Current = "Paste.png"; Mdi = "content-paste"; Color = "#6d4c41" },
	@{ Current = "SaveFloppy.png"; Mdi = "content-save"; Color = "#1565c0" },
	@{ Current = "Undo.png"; Mdi = "undo"; Color = "#6d4c41" },
	@{ Current = "Reload.png"; Mdi = "reload"; Color = "#1e88e5" },
	@{ Current = "NextArrow.png"; Mdi = "chevron-right"; Color = "#455a64"; Scale = 1.12 },
	@{ Current = "PreviousArrow.png"; Mdi = "chevron-left"; Color = "#455a64"; Scale = 1.12 },
	@{ Current = "Function.png"; Mdi = "function-variant"; Color = "#8e24aa" },
	@{ Current = "Enum.png"; Mdi = "format-list-numbered"; Color = "#00838f" },
	@{ Current = "Drive.png"; Mdi = "harddisk"; Color = "#546e7a" },
	@{ Current = "CheatCode.png"; Mdi = "code-tags"; Color = "#5e35b1" },
	@{ Current = "EditLabel.png"; Mdi = "label-outline"; Color = "#5e35b1" },
	@{ Current = "NesIcon.png"; Mdi = "gamepad-variant"; Color = "#e53935" },
	@{ Current = "SnesIcon.png"; Mdi = "gamepad-square"; Color = "#8e24aa" },
	@{ Current = "GameboyIcon.png"; Mdi = "nintendo-game-boy"; Color = "#43a047" },
	@{ Current = "GbaIcon.png"; Mdi = "gamepad-round"; Color = "#1e88e5" },
	@{ Current = "PceIcon.png"; Mdi = "chip"; Color = "#00897b" },
	@{ Current = "SmsIcon.png"; Mdi = "controller-classic"; Color = "#fb8c00" },
	@{ Current = "WsIcon.png"; Mdi = "cards-playing-outline"; Color = "#546e7a" },
	@{ Current = "LynxIcon.png"; Mdi = "paw"; Color = "#6d4c41" }
)

$results = New-Object System.Collections.Generic.List[object]

foreach ($entry in $map) {
	$mdiName = [string]$entry.Mdi
	$currentName = [string]$entry.Current
	$color = [string]$entry.Color
	$svgName = [System.IO.Path]::GetFileNameWithoutExtension($currentName) + ".svg"
	$outPath = Join-Path $targetDir $svgName
	$url = "https://raw.githubusercontent.com/Templarian/MaterialDesign/master/svg/$mdiName.svg"
	$scale = if ($entry.ContainsKey("Scale")) { [double]$entry.Scale } else { $defaultScale }
	$scaleText = $scale.ToString([System.Globalization.CultureInfo]::InvariantCulture)

	try {
		$raw = (Invoke-WebRequest -Uri $url -TimeoutSec 30).Content
		if (-not $raw) {
			throw "Empty SVG payload"
		}

		if ($entry.ContainsKey("WhiteGlyph") -and $entry.WhiteGlyph -eq $true) {
			$glyphRaw = (Invoke-WebRequest -Uri "https://raw.githubusercontent.com/Templarian/MaterialDesign/master/svg/help.svg" -TimeoutSec 30).Content
			$glyphPath = [regex]::Match($glyphRaw, '<path[^>]*>').Value
			$glyphPath = $glyphPath -replace 'fill="[^"]*"', ''
			if (-not $glyphPath) {
				throw "Failed to build help glyph"
			}
			$glyphPath = $glyphPath -replace '<path ', '<path fill="#ffffff" '

			$svg = @"
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
	<circle cx="12" cy="12" r="10.4" fill="$color"/>
	<g transform="translate(12 12) scale(0.78) translate(-12 -12)">
		$glyphPath
	</g>
</svg>
"@
		} else {
			# Ensure bold/color appearance for previews.
			$svg = $raw -replace '<svg\s+', ("<svg fill=`"" + $color + "`" ")
			$svg = $svg -replace '<path ', ("<path transform=`"translate(12 12) scale(" + $scaleText + ") translate(-12 -12)`" ")
		}

		Set-Content -Path $outPath -Value $svg -NoNewline

		$results.Add([pscustomobject]@{
			Current = $currentName
			Mdi = $mdiName
			Color = $color
			Output = $outPath
			Status = "ok"
		}) | Out-Null
	} catch {
		$results.Add([pscustomobject]@{
			Current = $currentName
			Mdi = $mdiName
			Color = $color
			Output = $outPath
			Status = "failed"
		}) | Out-Null
	}
}

$licenseText = @"
Material Design Icons
https://github.com/Templarian/MaterialDesign
License: Apache-2.0
"@
Set-Content -Path (Join-Path $targetDir "LICENSE-MDI.txt") -Value $licenseText -NoNewline

$results | Sort-Object Current
