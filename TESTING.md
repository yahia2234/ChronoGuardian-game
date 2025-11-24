# Chrono Guardian - Testing Checklist

## Current Issues to Verify:

### Physics & Collision
- [ ] Player spawns at correct height (should be y=1.0, currently y=2.0)
- [ ] Player cannot walk through walls
- [ ] Player stays on ground (gravity working)
- [ ] Collision pushes player out of walls

### Controls
- [ ] Mouse movement rotates camera
- [ ] WASD keys move player
- [ ] Right-click toggles camera mode
- [ ] R key restarts level
- [ ] ESC quits game

### Visibility
- [ ] Can see checkered floor
- [ ] Can see grey walls
- [ ] Can see player (blue sphere with fragments)
- [ ] Scene is well-lit
- [ ] Background is visible (blue-grey)

### Audio
- [ ] Movement sound plays when moving
- [ ] Wall collision sound plays when hitting walls
- [ ] Sounds are clear and not too loud

## Debug Output to Check:

When running the game, you should see:
```
Player pos: (x, y, z)  - every second
Move input: (x, y, z)  - shows if WASD is working
```

## Expected Behavior:

1. **Spawn**: Player at (0, 1, 0) after gravity settles
2. **Movement**: WASD moves player, position changes in debug output
3. **Walls**: Cannot move past x=±10 or z=±10
4. **Camera**: Mouse movement changes view direction
5. **Visibility**: See grey walls, checkered floor, blue player

## If Issues Persist:

The most likely problems are:
1. **Mouse not captured** - cursor should be hidden
2. **Collision not resolving** - walls should physically block
3. **Camera angle** - might be looking at wrong direction
4. **Lighting too dark** - scene not visible

Please test and report which items work and which don't!
