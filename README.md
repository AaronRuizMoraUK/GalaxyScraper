# Galaxy Scraper

Game developed as the final project of the 2007 Master in Videogame Development at Pompeu Fabra University.

**Awards**

- Art Futura 2007 Best Game Winner
- GDC Independent Games Festival 2008 Winner

## Game Synopsis

You are a real space adventurer. Your journey has taken you to a dying solar system. To escape, you must overcome the challenges presented by each planet.

Immerse yourself in a fully 3D world as you try to find your way out of the planet.

## Build and Run

Requirements
- Microsoft Windows 
- Microsoft Visual Studio 2022
- CMake 3.28
- Graphics card with DirectX 9.0c support

Steps
- Clone repo
  ````
  git clone https://github.com/AaronRuizMoraUK/GalaxyScraper.git
  ````
- Generate Visual Studio solution using CMake in `build` folder
  ```` 
  cd GalaxyScraper
  mkdir build
  cd build
  cmake .. -G "Visual Studio 17 2022"
  ````
- Open `GalaxyScraper.sln` with Visual Studio
- Build and run `GalaxyScraper` project

The game runs by default in window mode at `1280x720` resolution. To run the game in full screen set the `fullscreen` property to `1` in  `GalaxyScraper\data\xmls\config.xml` file. Resolution can be changed in the `Options` menu inside the game.

## Controls

A gamepad with analog stick, such as the Xbox Controller, is highly recommended:

- Use left analog stick to move
- `A` button to kick.
- `B` button to jump.
- `Start` button to pause the game.

Controls using keyboard:

- Use `WSAD` keys to run.
- Left mouse button to kick.
- Right mouse button to jump.
- Keep `Shift` key pressed to walk.
- Use `Enter` to pause the game.

## Improvements

I made the following enhancements while adapting the project for Github.

- Built the project using CMake.
- Added support for modern screen resolutions and refresh rates.
- Improved separation between game code and third-party libraries.

## Credits

Game created at Pompeu Fabra University during the 2007 Master in Videogame Development. Special thanks to all the teachers who provided guidance during the tutoring sessions!

Developed by:

- Aaron Ruiz
- Carlos Guzmán
- Xavi Lamarca
- Roger Solé
- Josep Soto

Music by:

- Nomar (sinplanb)

## Third-party Libraries

- **[DXSDK-D3DX](https://www.nuget.org/packages/Microsoft.DXSDK.D3DX)**: Headers, import libraries and runtime DLLs for the deprecated D3DX9 utilities libraries. 
- **[Bass](https://www.un4seen.com/)**: Audio library.
- **[Cal3D](https://github.com/mp3butcher/Cal3D)**: 3D character animation library.
- **[Expat](https://github.com/libexpat/libexpat)**: C library for parsing XML.
- **[Lua](https://github.com/lua/lua)**: Powerful, efficient, lightweight, embeddable scripting language.
- **[ParticleAPI](https://github.com/davemc0/Particle)**: Particle system library.
