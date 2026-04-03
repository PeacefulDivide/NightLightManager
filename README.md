<h1 align="center"> Night Light Manager </h1>

Night Light Manager is a lightweight Windows application built in C++ that manages monitor color temperatures (Gamma Ramps) with a focus on seamless transitions between desktop use and high-performance gaming. Unlike the built-in Windows Night Light, this utility offers an automated "Exclusion list" that restores color accuracy the moment a 3D application is detected.

<p align="center">
 <img src="screenshots/main_ui.png" alt="Night Light Manager UI" width="45%"/>
 <img src="screenshots/tray.png" alt="System Tray Menu" width="35%" />
 </p>

 <p align="center">
  <em>The primary configuration interface (Left) and system tray integration (Right).</em>
  </p>

## The Problem
Many gamers and creative professionals use the Windows "Night Light" feature to reduce eye strain. However, it can distort colors in games or creative apps. Manually toggling it everytime I want to play a game is a hassle.

## Features
- **Auto Game Detection:** Background polling (via CreateToolhelp32Snapshot) monitors for specified executables (e.g., EldenRing.exe) to disable filtering instantly for gaming.
- **System Tray Integration:** Runs silently in the background with minimal memory footprint.
- **Easy Configuration:** Add games manually or via a file browser to navigate to your games '.exe' files.
- **Persistence:** Uses Windows power states to automatically apply blue light filter after the system wakes from sleep.
- **Auto-Start:** Includes a built-in option to start with Windows during boot.
- **Privacy Focused:** Runs locally with no telemetry.

## Quick Start
1. Download NightLightManager.exe from the Latest Release.
2. Move the .exe to a permanent folder (e.g. Documents).
3. Run the application and use the system tray icon to adjust settings.

## Tech Stack
- **Language:** C++17
- **API:** Win32 API
- **Build System:** Visual Studio 2022
- **Mathematics:** Custom 256-entry Gammy LUT generation with strict monotonicity guards to ensure driver compatibility.
- **Performance:** Low polling (~0.1% CPU) with high priority wake up handlers for WM_POWERBROADCAST and WM_DISPLAYCHANGE events.

## How to Use
1. Clone repository and build the project in Visual Studio (x64 Release).
2. Run 'NightLightManager.exe'.
3. Use slider to adjust blue light filtration.
4. Add your game executables (e.g., 'EldenRing.exe') to the exclusion list through the browse or input bar.
	4(a). Tip: Start application you do not want the filter applied to. Then right-click start menu, select task manager. Right click the process for the application and select properties. The applications name with an exe should appear e.g. (firefox.exe).
5. Confirm changes with OK button.
6. Right-click tray icon and select **Settings (edit list)**.
7. Checking "Start with Windows" the application will launch minimized to system tray on boot, applying your settings instantly without interruption.

## Project Structure
- **main.cpp:** Entry point, system tray management, and background process watcher.
- **NightLightManager.h:** The engine for calculating and applying GPU Gamma Ramps.
- **SettingsUI.cpp:** The modal dialog interface and slider logic.
- **Utils.cpp:** Registry and system-level helper functions for persistence.

## License
Distributed under the **GPLv3 License**. See [LICENSE](LICENSE) for more information.
