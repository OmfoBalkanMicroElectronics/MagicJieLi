# Local music

The local music task plays files from a USB stick, or from SD storage if it is enabled.

- Shows the file/folder name, track number and play time.
- Supports folder switching, repeat modes and EQ presets.
- FF/FR key-release messages stop the seek icons from getting stuck on screen.
- Deleting a file takes two steps, with an on-screen confirmation.
- After about five seconds of inactivity, the visualizer opens. Any key returns to the normal screen.

Main sources:

- `apps/task_manager/task_music/task_music.c`
- `apps/task_manager/task_music/task_music_key.c`
- `apps/common/music_player/music_player.c`
- `apps/common/music_player/music_decoder.c`
