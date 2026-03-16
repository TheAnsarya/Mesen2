# Movie System Guide

## Overview

Nexen movies capture deterministic input history and related metadata for playback, verification, and TAS editing.

## Core Workflows

### Record a Movie

1. Start a game and open movie controls.
2. Begin recording.
3. Play normally or with frame stepping.
4. Stop and save the movie file.

### Playback and Review

1. Open an existing movie.
2. Start playback.
3. Pause, rewind, and inspect key moments.
4. Open TAS editor for frame-level adjustments.

### Import and Export

Nexen supports import and export for common TAS/movie formats. Use this when sharing with external tools or migrating old projects.

## GUI Tips

- Use branches in TAS editor to compare strategies.
- Use save states before major reroutes.
- Use movie playback to verify deterministic outcomes.

## Panel Walkthrough (Screenshot Anchors)

### Walkthrough A: Record and Save a Movie

1. Open movie controls from the main UI while a ROM is loaded.
2. Start recording.
3. Perform gameplay input sequence.
4. Stop recording and save the movie file.

| Step | Panel | Screenshot Anchor ID | Capture Target |
|---|---|---|---|
| 1 | Main Window (Movie Controls Open) | `movie-a-01-controls-open` | `docs/screenshots/movie-system/a-01-controls-open.png` |
| 2 | Movie Controls (Recording Active) | `movie-a-02-recording-active` | `docs/screenshots/movie-system/a-02-recording-active.png` |
| 3 | Save Movie Dialog | `movie-a-03-save-dialog` | `docs/screenshots/movie-system/a-03-save-dialog.png` |

### Walkthrough B: Playback and TAS Handoff

1. Open an existing movie.
2. Start playback and pause at target frame.
3. Open TAS editor and inspect input timeline.
4. Create a branch and continue testing.

| Step | Panel | Screenshot Anchor ID | Capture Target |
|---|---|---|---|
| 1 | Open Movie Dialog | `movie-b-01-open-dialog` | `docs/screenshots/movie-system/b-01-open-dialog.png` |
| 2 | Playback View (Paused at Target Frame) | `movie-b-02-paused-target` | `docs/screenshots/movie-system/b-02-paused-target.png` |
| 3 | TAS Editor (Piano Roll) | `movie-b-03-tas-piano-roll` | `docs/screenshots/movie-system/b-03-tas-piano-roll.png` |

## File Format

- Nexen native movie format: `.nexen-movie`
- Technical specification: [NEXEN_MOVIE_FORMAT.md](NEXEN_MOVIE_FORMAT.md)

## Related Links

- [TAS Editor Manual](TAS-Editor-Manual.md)
- [TAS Developer Guide](TAS-Developer-Guide.md)
- [Save States Guide](Save-States.md)
