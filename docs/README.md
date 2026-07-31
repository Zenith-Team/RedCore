<img alt="banner" src="https://github.com/user-attachments/assets/4060bc9b-185c-4944-9b66-d18025d08a15" />
<div align="center">
    <img alt="wiiu" height="56" src="https://github.com/user-attachments/assets/fc1f38b1-46a1-44ee-acfb-ac436d900dc6">
    <a href="https://go.nsmbu.net/discord">
        <img alt="discord" height="56" src="https://github.com/user-attachments/assets/495433d7-1ddc-47f1-8c8e-f4ef137754c2">
    </a>
    <a href="https://docs.zenith.red/redcore">
        <img alt="docs" height="56" src="https://github.com/user-attachments/assets/dc583842-a4d3-4635-b036-33d5f9bfc4d5">
    </a>
</div>

## Overview
A modern C++ API and modding framework for **New Super Mario Bros. U** code mods.

## Features
- **Dynamic**: Powered by [Telkin](https://github.com/Zenith-Team/Telkin), multiple code mods using RedCore can be loaded simultaneously!
- **Headers**: All libraries and game interfaces are accessible and rapidly updated.
- **Target Support**: Pre-configured for US, EU, JP regions along with the bundle (+NSLU) release on both Cemu and Console.
- **Named Registries**: Register custom actors in a quick and compatible manner using string names.
- **Event System**: Delegate-based event handling with listeners for simple hooks without ASM/RE or conflicts.
- **ImGui**: Easily produce visual interfaces for fast debugging and development.

## Quick Start
Fork the [Example Mod](https://github.com/Zenith-Team/RedCore-Example-Mod), and clone the repository
```sh
git clone https://github.com/Zenith-Team/RedCore-Example-Mod MyMod
cd MyMod
```
Install [Tachyon](https://github.com/Zenith-Team/Tachyon) (requires [Node.js](https://nodejs.org/) v24+)
```yml
npm i -g https://github.com/Zenith-Team/Tachyon/releases/latest/download/Tachyon.tgz
```
Build and run the project for your region (example with `US`)
```rb
tachyon pm install
tachyon compile US
tachyon launch US
```

Refer to the [documentation](https://docs.zenith.red/redcore) for further information.

## Credits
- [Luminyx](https://github.com/Luminyx1) - API, Infrastructure, Headers
- [stupidestmodder](https://github.com/stupidestmodder) - API, Headers
- [jhmaster](https://github.com/jhmaster2000) - Infrastructure

## Special Thanks
- [AboodXD](https://github.com/aboood40091) - Headers
- [Omar](https://github.com/ocornut) - ImGui Library
- [GaryOderNichts](https://github.com/GaryOderNichts) - ImGui GX2 Backend
