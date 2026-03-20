# Night Light Manager

A lightweight Windows system tray application that automatically manages the Windows Night Light setting based on user-defined excluded games.

## The Problem
Many gamers and creative professionals use the Windows "Night Light" feature to reduce eye strain. However, it can distort colors in games or creative apps. Manually toggling it everytime I want to play a game is a hassle.

## Features
- **Auto-Toggle:** Automatically disables Night Light when a specified game is launched and re-enables it when you exit.
- **System Tray Integration:** Runs silently in the background with minimal memory footprint.
- **Easy Configuration:** Add games manually or via a file browser to navigate to your games '.exe' files.
- **Persistence:** Saves your preferences to a local config file.
- **Auto-Start:** Includes a built-in option to start with Windows during boot.

## Tech Stack
- **Language:** C++17
- **API:** Win32 API
- **Build System:** Visual Studio 2022

## How to Use
1. Clone repository and build the project in Visual Studio (x64 Release).
2. Run 'NightLightManager.exe'.
3. Right-click tray icon and select **Settings (edit list)**.
4. Add your game executables (e.g., 'EldenRing.exe') to the exclusion list.