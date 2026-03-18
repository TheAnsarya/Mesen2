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
	@{ Current = "NesIcon.png"; Mdi = "custom-nes"; Color = "#e53935" },
	@{ Current = "SnesIcon.png"; Mdi = "custom-snes"; Color = "#8e24aa" },
	@{ Current = "GameboyIcon.png"; Mdi = "custom-gameboy"; Color = "#43a047" },
	@{ Current = "GbaIcon.png"; Mdi = "custom-gba"; Color = "#1e88e5" },
	@{ Current = "PceIcon.png"; Mdi = "custom-pce"; Color = "#00897b" },
	@{ Current = "SmsIcon.png"; Mdi = "custom-sms"; Color = "#fb8c00" },
	@{ Current = "WsIcon.png"; Mdi = "custom-ws"; Color = "#546e7a" },
	@{ Current = "LynxIcon.png"; Mdi = "paw"; Color = "#6d4c41" }
)

function GetCustomPlatformSvg([string]$currentName) {
	switch ($currentName) {
		"NesIcon.png" {
			return @"
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
	<rect x="2" y="6" width="20" height="12" rx="2.4" fill="#455a64"/>
	<rect x="3.2" y="7.2" width="17.6" height="9.6" rx="1.6" fill="#607d8b"/>
	<rect x="10.8" y="7.2" width="2.4" height="9.6" fill="#37474f"/>
	<rect x="5.2" y="10" width="3.6" height="1.2" rx="0.4" fill="#eceff1"/>
	<rect x="6.4" y="8.8" width="1.2" height="3.6" rx="0.4" fill="#eceff1"/>
	<circle cx="16.4" cy="10.4" r="1.1" fill="#ef5350"/>
	<circle cx="18.6" cy="12.6" r="1.1" fill="#ef5350"/>
	<rect x="15.6" y="14.4" width="4" height="1" rx="0.5" fill="#cfd8dc"/>
</svg>
"@
		}
		"SnesIcon.png" {
			return @"
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
	<path d="M5,9.5C6.2,8.2 8,7.5 12,7.5C16,7.5 17.8,8.2 19,9.5C20.2,10.8 20.4,13.2 19.4,15.4C18.7,16.8 17.6,17.8 16.1,17.8C14.9,17.8 13.8,17.2 13,16.2H11C10.2,17.2 9.1,17.8 7.9,17.8C6.4,17.8 5.3,16.8 4.6,15.4C3.6,13.2 3.8,10.8 5,9.5Z" fill="#d7dde2"/>
	<rect x="6.2" y="11.2" width="3.4" height="1.2" rx="0.4" fill="#546e7a"/>
	<rect x="7.3" y="10.1" width="1.2" height="3.4" rx="0.4" fill="#546e7a"/>
	<circle cx="15.5" cy="10.8" r="1.1" fill="#e53935"/>
	<circle cx="17.6" cy="12.3" r="1.1" fill="#43a047"/>
	<circle cx="15.4" cy="13.8" r="1.1" fill="#1e88e5"/>
	<circle cx="13.3" cy="12.3" r="1.1" fill="#fdd835"/>
</svg>
"@
		}
		"GameboyIcon.png" {
			return @"
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
	<rect x="5" y="2.2" width="14" height="19.6" rx="2.4" fill="#607d8b"/>
	<rect x="7" y="4.4" width="10" height="7.2" rx="0.9" fill="#9ccc65"/>
	<rect x="8" y="14.6" width="3" height="1" rx="0.35" fill="#eceff1"/>
	<rect x="9" y="13.6" width="1" height="3" rx="0.35" fill="#eceff1"/>
	<circle cx="14.3" cy="15.2" r="0.9" fill="#ab47bc"/>
	<circle cx="16.1" cy="16.7" r="0.9" fill="#ab47bc"/>
	<circle cx="12" cy="19.2" r="0.7" fill="#37474f"/>
</svg>
"@
		}
		"GbaIcon.png" {
			return @"
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
	<rect x="2" y="7" width="20" height="10" rx="5" fill="#1e88e5"/>
	<rect x="7" y="8.7" width="10" height="5" rx="1" fill="#0d47a1"/>
	<rect x="4.5" y="10.8" width="2.4" height="0.9" rx="0.3" fill="#e3f2fd"/>
	<rect x="5.25" y="10.05" width="0.9" height="2.4" rx="0.3" fill="#e3f2fd"/>
	<circle cx="18" cy="11" r="0.8" fill="#90caf9"/>
	<circle cx="19.5" cy="12.5" r="0.8" fill="#90caf9"/>
</svg>
"@
		}
		"PceIcon.png" {
			return @"
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
	<rect x="3" y="4" width="18" height="16" rx="2" fill="#00897b"/>
	<rect x="7" y="8" width="10" height="8" rx="1.2" fill="#004d40"/>
	<path d="M3 9H6M3 12H6M3 15H6M18 9H21M18 12H21M18 15H21" stroke="#80cbc4" stroke-width="1" stroke-linecap="round"/>
	<circle cx="10" cy="11" r="0.8" fill="#80cbc4"/>
	<circle cx="14" cy="13" r="0.8" fill="#80cbc4"/>
</svg>
"@
		}
		"SmsIcon.png" {
			return @"
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
	<rect x="2" y="8" width="20" height="8" rx="3" fill="#fb8c00"/>
	<rect x="5" y="11" width="3" height="1" rx="0.35" fill="#3e2723"/>
	<rect x="6" y="10" width="1" height="3" rx="0.35" fill="#3e2723"/>
	<circle cx="16" cy="12" r="1.1" fill="#3e2723"/>
	<circle cx="19" cy="12" r="1.1" fill="#3e2723"/>
</svg>
"@
		}
		"WsIcon.png" {
			return @"
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
	<rect x="2" y="4" width="20" height="16" rx="3" fill="#546e7a"/>
	<path d="M4.5 8.5L6.5 15.5L9 10.5L12 15.5L15 10.5L17.5 15.5L19.5 8.5" fill="none" stroke="#eceff1" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round"/>
	<circle cx="12" cy="18" r="1" fill="#90caf9"/>
</svg>
"@
		}
		default {
			return $null
		}
	}
}

$results = New-Object System.Collections.Generic.List[object]

foreach ($entry in $map) {
	$mdiName = [string]$entry.Mdi
	$currentName = [string]$entry.Current
	$color = [string]$entry.Color
	$svgName = [System.IO.Path]::GetFileNameWithoutExtension($currentName) + ".svg"
	$outPath = Join-Path $targetDir $svgName
	$customSvg = GetCustomPlatformSvg $currentName
	if (-not [string]::IsNullOrWhiteSpace($customSvg)) {
		Set-Content -Path $outPath -Value $customSvg -NoNewline

		$results.Add([pscustomobject]@{
			Current = $currentName
			Mdi = $mdiName
			Color = $color
			Output = $outPath
			Status = "ok"
		}) | Out-Null
		continue
	}

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
