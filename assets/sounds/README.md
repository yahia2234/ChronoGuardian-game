# Sound Files

Place your custom sound files here to replace the procedural sounds.

## Supported Format
- **Extension**: `.wav` (WAV format)
- **Sample Rate**: 44100 Hz recommended
- **Channels**: Mono or Stereo

## Sound File Names
Replace these procedural sounds with your custom WAV files:

1. `movement.wav` - Player footstep/movement sound
2. `wall_collision.wav` - Player hitting a wall
3. `obstacle_hit.wav` - Player hit by obstacle (pendulum/stalactite)
4. `tile_crack.wav` - Crumbling tile breaking / level restart
5. `collectible_pickup.wav` - Picking up coins/gems
6. `level_complete.wav` - Level completion sound
7. `ambient_hum.wav` - Background ambient sound

## Usage
The game will automatically load WAV files from this directory if they exist.
If a file is missing, the game will use procedurally generated sounds as fallback.
