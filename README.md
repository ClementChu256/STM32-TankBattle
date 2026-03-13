# IERG3810 Mini Project: Tank Battle Game

## Overview

This project is a simplified version of the classic Battle City game developed for the **IERG3810 course** at The Chinese University of Hong Kong. It runs on an **STM32F10x embedded microcontroller** with a **TFT LCD display** and joystick input interface. The game features player-controlled tanks engaging in combat with AI-controlled enemy tanks across multiple maps with destructible obstacles.


### It is intended for educational and non-commercial use only.

## Project Structure

### Core Game Files

- **`miniProject.c`**: Main entry point containing the state machine for menu navigation and game flow
- **`Board/game.c` / `Board/game.h`**: Core game logic including entity movement, collision detection, firing mechanics, and game state management
- **`Board/entity.h`**: Defines the `entity` structure for all in-game objects (tanks, bullets, explosions)
- **`Board/map.h`**: Contains 3 pre-designed game maps with obstacle layouts

### Hardware Interface Files

- **`Board/IERG3810_LCD.c / IERG3810_LCD.h`**: TFT LCD display driver for rendering graphics
- **`Board/IERG3810_clock.c / IERG3810_clock.h`**: System clock configuration and timing functions
- **`Board/IERG3810_Interrupt.c / IERG3810_Interrupt.h`**: Interrupt handling for key presses and timers
- **`Board/IERG3810_KEY.c / IERG3810_KEY.h`**: GPIO key input configuration
- **`Board/getInput.c / IERG3810_clock.c`**: Input handling via joystick and buttons
- **`Board/FONT.H`**: Font data for text rendering on LCD

### Asset Files

- **`Board/entity.h`**: Sprite data for:
  - Tank designs (6 tank variants with animation states)
  - Bullet graphics
  - Explosion effects
  - Obstacle graphics (brick, steel, bushes)

## Game Mechanics

### Obstacle System

Obstacles form 8×8 tile units and are destructible on a per-tile basis:

| Type    | Description                  | Properties              |
|---------|------------------------------|-------------------------|
| Brick   | Destructible obstacles       | Can be destroyed by all bullets |
| Steel   | Indestructible obstacles     | Cannot be destroyed    |
| Bushes  | Concealment (16×16 groups)   | Hide tanks visually; 4 bushes per group |

### Direction System

```
0: +Y direction (upward)
1: +X direction (rightward)
2: -Y direction (downward)
3: -X direction (leftward)
```

### Entity Properties

The `entity` structure tracks all in-game objects with properties including:
- **Position**: `posX`, `posY` (current), `lastposX`, `lastposY` (previous)
- **Movement**: `dir` (direction), `speed` (movement speed), `sizeX`, `sizeY`
- **State**: `alive`, `health`, `explosion`, `state` (animation state)
- **Combat**: `shot` (firing status), `firecooldown`, `type`
- **Special**: `ignoreCollision` (spawn invulnerability), `explosionState`

### Tank Types

#### Player Tank
- **Speed**: 2 pixels/frame
- **Fire Rate**: 30 frame cooldown
- **Health**: 1 life per tank
- **Starting Position**: (64, 0) facing +Y direction
- **Initial Lives**: 3

#### AI Tanks (4 simultaneous on arena)

| Type | Speed | Fire Rate | Bullet Speed | Health |
|------|-------|-----------|--------------|--------|
| Type 0 | 2 | Normal | Normal | 1 |
| Type 1 | 4 | Low | High | 1 |
| Type 2 | 2 | High | High | 1 |
| Type 3 | 1 | Normal | Normal | 4 |

### Game Flow

1. **Main Menu**: Select "Start Game" or "Help and Tips"
2. **Map Selection**: Choose from 3 different map layouts
3. **Gameplay**: 
   - Control player tank with joystick
   - Fire bullets to destroy enemy tanks and obstacles
   - Avoid enemy fire
   - Survive wave of enemies to win
4. **Game Over/Win**: Return to main menu

### Controls

| Control | Action |
|---------|--------|
| **Joystick Up/Down/Left/Right** | Move tank in all 4 directions |
| **SELECT Button** | Confirm menu selection / Fire bullet |
| **START Button** | (Menu/UI navigation) |
| **A/B Buttons** | Help/HTP page navigation |

## Technical Implementation

### Graphics Rendering
- **Resolution**: TFT LCD display (320×240 typical)
- **Tank Size**: 16×16 pixels
- **Obstacle Size**: 8×8 pixels each
- **Bullet Size**: Smaller projectiles
- **Double Buffering**: Efficient screen updates by covering last positions

### Game Loop Architecture
- **Timer-based Updates**: TIM3 interrupt drives game state updates
- **Event-driven Input**: Joystick input via external interrupts
- **Collision Detection**: Per-pixel and AABB collision checking
- **Priority-based Rendering**: Obstacles, tanks, then effects

### Optimization Techniques
- **Partial Screen Clearing**: Only redraw changed regions instead of full screen
- **Cooldown Counters**: Track firing and explosion timers per entity
- **Collision Cache**: Temporary collision cooldown to prevent rapid re-collision

## Game Maps

The project includes 3 distinct maps (`mapdata[3][30][30]`) with varying obstacle layouts:
- **Map 0**: Symmetric design with central open area
- **Map 1**: Asymmetric layout with varied obstacle clusters
- **Map 2**: Dense obstacle pattern for complex gameplay

Each map is 30×30 grid units (240×240 pixels in-game).

## Compilation and Deployment

This project targets **STM32F10x** microcontroller family. Compilation requires:
- **STM32F10x standard peripheral library**
- **ARM Compiler Toolchain** or equivalent
- Hardware: STM32F103 evaluation board with TFT LCD shield and joystick input

## Game Features

- Multi-entity collision and physics  
- Destructible environment with tile-based obstacles  
- AI tank variety with distinct behaviors  
- Animation states for tanks and explosions  
- Multiple game maps with strategic layouts  
- Fire rate and health management system  
- Real-time input response  
- Menu system with help/tips pages  


