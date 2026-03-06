# VEX V5 Robotics Project - AGENTS.md

## Project Overview

This is a **VEX V5 Robotics Competition project** for a ball-shooting robot (likely designed for the "Spin Up" or similar game). The project consists of two main components:

1. **Embedded Robot Code (C++)**: Runs on the VEX V5 brain, controlling motors, sensors, and autonomous routines
2. **Python Visualization Tools**: Flask web app and CLI tool for analyzing robot telemetry data and tuning PID parameters

The robot features:
- 6-wheel tank drive chassis (6 motors: 3 left, 3 right)
- Ball intake and shooting mechanisms
- Color-based ball sorting system (red/blue alliance detection)
- Inertial sensor (gyroscope) for precise heading control
- Distance sensors for wall alignment
- Pneumatic wings and loading mechanisms

---

## Technology Stack

### Embedded/Firmware
- **Platform**: VEX V5 Brain (ARM Cortex-A9)
- **Language**: C++11 with VEX SDK
- **SDK**: VEXcode Pro V5 SDK (clang-based toolchain)
- **Target**: thumbv7-none-eabi

### Data Visualization & Analysis
- **Backend**: Python 3, Flask
- **Frontend**: HTML5, vanilla JavaScript, Plotly.js 2.35.0
- **Data Processing**: pandas, matplotlib
- **Styling**: Catppuccin Mocha dark theme

### Build System
- **Build Tool**: GNU Make
- **Compiler**: clang (cross-compiling for ARM)
- **Linker**: arm-none-eabi-ld

---

## Project Structure

```
.
├── src/                        # C++ source files
│   ├── main.cpp               # Main entry point, competition callbacks
│   └── robot-config.cpp       # Device initialization and configuration
│
├── include/                    # Header files (robot control logic)
│   ├── vex.h                  # VEX SDK includes and utility macros
│   ├── robot-config.h         # External device declarations
│   ├── void.h                 # Core control functions, PID algorithms, color sorting
│   ├── joystick.h             # Driver control mapping and threads
│   ├── mainauto.h             # Autonomous program dispatcher
│   ├── auto_All.h             # Common autonomous subroutines
│   ├── auto1.h - auto4.h      # Individual autonomous routines (4 variants)
│   ├── auto_skill.h           # Skills challenge autonomous
│   └── auto_backup.h          # Legacy autonomous code
│
├── vex/                        # VEX build system
│   ├── mkenv.mk               # Build environment configuration
│   └── mkrules.mk             # Compilation rules
│
├── app.py                     # Flask web server for interactive plotting
├── plot.py                    # CLI tool for matplotlib-based visualization
├── out.csv                    # Sample telemetry data file
├── run.command                # macOS shortcut to launch web app
├── makefile                   # Main build file
└── requirements.txt           # Python dependencies
```

---

## Build and Test Commands

### Building the Robot Code

```bash
# Build the project (produces build/*.bin)
make

# Build with verbose output
make VERBOSE=1

# Clean build artifacts
make clean
```

**Build Output**: `build/<project_name>.bin` - Upload this file to the VEX V5 Brain using VEXcode or the controller.

### Running the Visualization Tools

```bash
# Install Python dependencies
pip install -r requirements.txt
# or: pip install flask pandas matplotlib

# Launch interactive web plotter (default: http://localhost:5000)
python3 app.py
python3 app.py out.csv              # Pre-load a data file
python3 app.py --port 8080          # Custom port
python3 app.py --no-browser         # Don't auto-open browser

# Or use the CLI plotting tool
python3 plot.py                     # Plot out.csv
python3 plot.py data.txt            # Plot specific file
python3 plot.py --clipboard         # Read from clipboard
python3 plot.py --watch term.txt    # Watch mode (auto-refresh)
python3 plot.py --save output       # Save as PNG files
```

---

## Code Organization

### Robot Control Architecture

The robot code follows a multi-threaded architecture:

1. **Main Thread** (`main.cpp`):
   - Initializes competition callbacks
   - Runs pre-autonomous calibration
   - Dispatches to autonomous or driver control

2. **Autonomous Execution** (`mainauto.h`, `auto*.h`):
   - `AutoPro()` - Main autonomous dispatcher
   - 4 selectable autonomous routines (Auto_1 to Auto_4)
   - Alliance color detection and path selection

3. **Driver Control Threads** (`joystick.h`):
   - `base_control()` - Arcade drive with speed limiting
   - `Intake_Shoot_ctrl()` - Ball handling controls
   - `hook_control()` - Wing/pneumatic controls
   - `load_ctrl()` - Loading mechanism
   - `hold_ctrl()` - Brake mode toggle

4. **Core Control Functions** (`void.h`):
   - **Movement**: `Run_gyro()`, `Run_gyro_new()`, `Turn_Gyro()`
   - **PID Control**: PD controllers for distance and heading
   - **Ball Handling**: `Get_Ball()`, `Intake()`, `Shoot()`, `Ball()`
   - **Color Sorting**: `Color_Control()` thread for automatic ball filtering

### Key Algorithms

**Gyro-Assisted Driving (PD Control)**:
```cpp
// From void.h - Run_gyro_new()
// Uses dynamic dt calculation for accurate derivative term
gyro_kp = 7.0 * avg_rpm / 100;    // Adaptive proportional gain
gyro_kd = 0.7 * avg_rpm / 100;    // Adaptive derivative gain
turnpower = gyro_kp * gyro_err + gyro_kd * vg;
```

**Color-Based Ball Sorting**:
- Uses 3 optical color sensors (Color, Color_2, Color_3)
- Automatically ejects opponent-colored balls
- Alliance selection: `Alliance = 1` (red) or `Alliance = -1` (blue)

---

## Test Data Format

The project supports logging and visualization of 5 test types:

| Test Type | Columns | Purpose |
|-----------|---------|---------|
| `test_turn` | time_s, gyro_err, vg, turnpower, left_avg, right_avg | PID turning tuning |
| `test_straight` | time_s, menc, move_err, vm, current_power, gyro_err, vg, turnpower, left_avg, right_avg | Straight-line PD |
| `test_straight_v2` | (same as above + last_move_error, delta_move_err, dt) | Enhanced logging |
| `test_gyro_pd` | (same as v2) | Gyro PD parameter tuning |
| `test_minspeed` | time_s, power, diff | Minimum drive power calibration |

**Data Format in Files**:
```
test_turn
time_s,gyro_err,vg,turnpower,left_avg,right_avg
0.000,24.96,0.000,100.0,0.0,0.0
0.010,24.52,-0.440,98.2,5.2,-5.2
...
--- test_turn complete (total 156 samples) ---
```

---

## Development Conventions

### Code Style

- **Mixed Language Comments**: Comments are primarily in Chinese with some English
- **Naming Conventions**:
  - Functions: `PascalCase` for major functions (`Run_gyro`, `Turn_Gyro`)
  - Variables: `snake_case` for locals (`gyro_err`, `move_power`)
  - Globals: lowercase or camelCase (`Auto`, `Side`, `Alliance`, `motor_min_speed`)
  - Constants: `SCREAMING_SNAKE_CASE` for colors (`C_BLUE`, `C_RED`)

### Key Global Variables

```cpp
// Located in main.cpp
int Side = 1;           // Field side multiplier (1 or -1)
int Alliance = 1;       // Alliance color: 1=red, -1=blue, 0=manual
int Auto = 1;           // Selected autonomous routine (1-4)
float now = 0;          // Current target heading (relative)
float Start = 0;        // Gyro offset for field-oriented driving

// Located in void.h
float motor_min_speed = 24;  // Minimum drivetrain power (percent)
```

### Autonomous Programming

Autonomous routines are stored as header files (`auto1.h` to `auto4.h`) that are `#include`-ed into function bodies. This allows:
- Quick switching between routines
- Easy duplication and modification
- No need to recompile the entire project for small path changes

---

## Testing and Tuning Workflow

### PID Tuning Process

1. **Run the test function** in `autonomous()` (in `main.cpp`):
   ```cpp
   // Uncomment one of these:
   test_straight(1500);      // Test straight-line PID
   test_turn(90);            // Test turning PID
   test_minspeed();          // Calibrate minimum power
   ```

2. **Capture serial output** from the VEX V5 brain to a file (e.g., `out.csv`)

3. **Visualize the data**:
   ```bash
   python3 app.py out.csv
   # Open browser at http://localhost:5000
   ```

4. **Adjust PID constants** in `void.h` and repeat

### Data Corruption Handling

The Python tools include special handling for V5 USB serial buffer overflows:
- Detects and salvages concatenated lines
- Reports sample count mismatches
- Validates numeric data before parsing

---

## Hardware Configuration

### Motor Ports
| Motor | Port | Type | Reversed |
|-------|------|------|----------|
| LeftRun_1 | 10 | 18:1 | true |
| LeftRun_2 | 13 | 18:1 | false |
| LeftRun_3 | 3 | 18:1 | true |
| RightRun_1 | 11 | 18:1 | false |
| RightRun_2 | 12 | 18:1 | true |
| RightRun_3 | 14 | 18:1 | false |
| Intake_1 | 6 | 6:1 (turbo) | true |
| Shoot_1 | 2 | 6:1 (turbo) | false |
| Ball_1 | 1 | 18:1 | true |

### Sensor Ports
| Sensor | Port | Type |
|--------|------|------|
| Gyro (Inertial) | 20 | Inertial |
| Color | 9 | Optical |
| Color_2 | 19 | Optical |
| Color_3 | 14 | Optical |
| Distance1 | 5 | Distance |
| Distance2 | 4 | Distance |
| Vision | 20 | AI Vision |

### Pneumatics (Digital Out)
| Device | Port |
|--------|------|
| Load | Brain.ThreeWirePort.A |
| Wing_R | Brain.ThreeWirePort.H |
| Wing_L | Brain.ThreeWirePort.F |
| Up | Brain.ThreeWirePort.D |
| Basket | Brain.ThreeWirePort.C |
| Anchor | Brain.ThreeWirePort.E |

---

## Security Considerations

- **No network connectivity** in robot code (VEX V5 is air-gapped during competition)
- **Python tools are local-only**: Flask runs on `127.0.0.1` by default
- **File access**: Python tools only read/write CSV files in the project directory
- **No secrets or credentials** stored in the codebase

---

## Common Tasks

### Adding a New Autonomous Routine

1. Create `include/auto5.h` with the autonomous code
2. Add function declaration in `mainauto.h`:
   ```cpp
   void Auto_5();
   ```
3. Add dispatcher case in `AutoPro()`:
   ```cpp
   if (Auto == 5) { Auto_5(); }
   ```

### Modifying PID Constants

Edit the constants in `void.h`:
```cpp
// In Run_gyro_new()
float gyro_kp_base = 7.0;     // Increase for sharper correction
float gyro_kd_base = 0.7;     // Increase for more damping
float move_kp = 0.35;         // Distance control
float move_kd = 0.03;         // Speed control
```

### Calibrating Minimum Drive Power

Run `test_minspeed()` and check the `out.csv` output. The `motor_min_speed` value should be set slightly above the threshold where the robot starts moving consistently.

---

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Gyro drift | Ensure `Gyro.startCalibration()` runs in `pre_auton()` and wait for completion |
| Build fails with "SDK not found" | Set `VEX_SDK_PATH` environment variable or install VEXcode Pro |
| Python app shows "No data loaded" | Check CSV file format matches expected test headers |
| Motors don't move | Verify `motor_min_speed` is set appropriately (default: 24%) |
| Color sorting not working | Check optical sensor LED power (`setLightPower(100, percent)`) |

---

## Dependencies

### Firmware Build
- VEXcode Pro V5 SDK (or VEX SDK standalone)
- GNU Make
- ARM cross-compilation toolchain (clang, arm-none-eabi-ld)

### Python Tools
```
flask>=3.0
pandas>=2.0
matplotlib>=3.0  (for plot.py only)
```

---

*Last updated: 2026-03-06*
