## SimpleFastOpenAtomicVisualiser (SFOAV)
<p align="center">
  <img height=256px src="https://github.com/user-attachments/assets/8f109e8a-aa71-425a-8694-270695ce138c"/>
  <br>Pislocybin molecule with 2 triangles per bond (ray-traced).
</p>
A simple program to quickly visualise atomic and molecular systems (AMSs).

The code is accelerated using OpenGL for 3D rendering and builds for Linux, Windows, macOS, and Android.

## [Documentation](https://jerboaburrow.github.io/SimpleFastOpenAtomicVisualiser/)


## What it is

- **Simple**: a trivial interface for loading and displaying AMSs configurations and trajectories. Via command line or GUI.
- **Fast**: rendering as fast as e.g. VMD or other 3d-viewers, with many efficiency options for level of detail.
- **Open**: GPL licensed and open source, enabling commercial use, adaptation, extension etc.
- **Atomic visualisation**: render, colourable, atoms and bonds in 3D space loaded from standard AMSs configuration and trajectory data files.

# Quickstart

To render a structure file ```struct.xyz``` simply call

```shell
sfoav struct.xyz
```

> [!important]
> SFOAV can process ```.xyz```, ```.extxyz```, and DL_POLY ```CONFIG```, ```REVCON``` and ```HISTORY``` files. If the file name does not match these patterns all types will be attempted.

This will bring up the view centring the atoms in ```struct.xyz``` in the first frame (if applicable). The camera is centered on (0, 0, 0) and can be moved in spherical coordinates relative to it. By defaul the atoms are centred at (0, 0, 0). The atoms can also be translated relative to (0, 0, 0).

> [!note]
> Reading of structure files is done in a background thread. For large structure files you may be presented with a loading screen. An intel i7-4790K and Kingston A400 SATA SSD is capable of around 500,000 (positions only) atoms per second read.

If the structure file is a trajectory you may scan through its frames moving forward of backward in time using F and B respectively. Or auto-playing/pausing with P.

> [!note]
> When reading HISTORY files or XYZ/EXTXYZ with multiple frames, SFOAV will cache the filepositions (not data) of each frame in the background. For large trajectory files this may take some time, but you will always be able to play up to the most recently cached frame.

At runtime the following camera key-controls can be used:

| Key | Action  | Note |
| :----- | :---- | :---- |
| W      | Zoom towards the origin. | |
| S      | Zoom away from the origin. | |
| Q      | Incline the view. | |
| E      | Decline the view. | |
| A      | Rotate the view. | |
| D      | Rotate the view. | |
| SPACE  | Reset to the default view and atom positions | At (0,0,0), azimuth Pi/2 and inclination Pi. |

The following atom key-controls are available:

| Key | Action  | Note |
| :----- | :---- | :---- |
| H      | Toggle atom drawing.    | |
| LEFT   | Translate the atoms in -x | |
| RIGHT  | Translate the atoms in +x | |
| UP     | Translate the atoms in +z | |
| DOWN   | Translate the atoms in -z | |
| .      | Translate the atoms in -y | |
| /      | Translate the atoms in +y | |
| 1 to 9 | Toggle element emphasis | Elements assigned at startup. |

Trajectory playback may be controlled by the following key-bindings:
| Key | Action  | Note |
| :----- | :---- | :---- |
| F      | Move forward in time  | Sets forwarding playing with P. |
| B      | Move backward in time | Sets backward playing with P. |
| P      | Pause/Play a trajectory | |
| J      | Decrease play speed | The minimum is 1 frame per second. |
| K      | Increase play speed | The maximum is 60 frames per second. |
| R      | Reset to the first trajectory frame | |

Miscellaneous key bindings are:
| Key | Action  | Note |
| :----- | :---- | :---- |
| X      | Toggle drawing the coordinate axes | |
| C      | Toggle drawing the simulation cell | |
| I      | Toggle information text | |
| V      | Start or finish a video recording | |
| ESC    | Quit | |

To enable MSAA at 16x

```shell
sfoav struct.xyz -msaa 16
```

To draw bonds between atoms 1.5 Angstroms apart

```shell
sfoav struct.xyz -bondCutOff 1.5
```

## Lua scripting

It is possible to write Lua scripts to manipulate visualisation in SFOAV. By supplying a path as ```--script PATH.lua``` to a Lua file, SFOAV will run the file each frame update. The console exports the following methods in the sfoav library.

> [!warning]
> Lua indexes from 1, but all sfoav library functions index from 0.

| Method | Arguments  | Return/Effect |
| :----- | :---- | :---- |
| setAtomColour  | Atom index and an RGB/RGBA colour, in [0, 1] | |
| getAtomColour  | Atom index | The atoms RGBA colour |
| bond           | Atom index a, Atom index b | Bond atoms a and b |
| unbond         | Atom index a, Atom index b | Unbond atoms a and b |
| getAtomsBonds  | Atom index a | A table of all atom indices bonded to a|
| getAtom        | Atom index a | The Atom structure for a |
| atomCount      | | The number of atoms |
| getAtomsNeighbours | Atom index x, cutoff distance | The neighbours of a within the cutoff |
| setText | Text string | The neighbours of a within the cutoff |
| getFrame | | The current frame number (from 0) |

```lua
-- Set atom 0 to a random colour.
sfoav.setAtomColour(0, math.random(), math.random(), math.random())
```

Or a more complex example to render only the neighbours of atom 0 within 4 Angstroms

```lua
-- Get atom 0's neighbours up to 4 Angstroms.
neighbours = sfoav.getAtomsNeighbours(0, 4.0)

-- Fade all atoms.
for i = 1, sfoav.atomCount() do
    r, g, b, a = sfoav.getAtomColour(i-1)
    sfoav.setAtomColour(i-1, r, g, b, 0.0)
end

-- Unfade all neighbours to atom 0 within 4 Angstroms.
for i = 1, #neighbours do
    r, g, b, a = sfoav.getAtomColour(neighbours[i]["index"])
    sfoav.setAtomColour(neighbours[i]["index"], r, g, b, 1.0);
end
```

## Video

On macOS and Windows one release exists using jo_mpeg to write mp4 files.

On Linux two releases exist, the standalone ```sfoav``` which uses jo_mpeg for video writing, and the FFmpeg enabled version ```sfoav-ffmpeg``` which requires additional runtime dependencies (FFmpeg). The FFmpeg video quality is generally superior.

## Performance

For a system with an intel i7-4790K, Kingston A400 SATA SSD, a GTX 1080 ti, and 16 GB available RAM. SFOAV is capable of rendering at least 5,000,000 static atoms at 60 frames per second with 16x MSAA and with a moveable camera. At this scale moving the atoms will run cause drops to 30 fps, and frame increments will cost ~5 seconds.

Transparency sorting is on by default, if there are transparent atoms/bonds. This is expensive for the CPU on camera movements or atom/bond changes. This can be disabled with ```-noTransparencySorting```, but will render atoms/bonds out of order.

## MIT version

An MIT version prior to the inclusion of FFmpeg and the GPL v2 license can be found here https://github.com/JerboaBurrow/SimpleFastOpenAtomicVisualiser/releases/tag/v0-0.0.7

---

## Features (in development)

- [ ] High level viewing
  - [x] Play/pause/step through time.
  - [x] Atom emphasis.
  - [ ] Molecule/atom group emphasis.
  - [ ] Atom trajectory paths.
  - [ ] "Atom/molecule" cam.
  - [x] Simulation cell.
  - [x] Coordinate axes.
  - [ ] Dynamic bonds.
  - [ ] User supplied emphasis file.
  - [ ] Atom/molecule info hover.
  - [ ] Imgui UI??.
- [ ] Input
  - [x] Atom position file formats.
    - [x] XYZ/EXTXYZ.
    - [x] CONFIG/REVCON/HISTORY.
  - [ ] Atom connectivity file formats.
- [ ] Output
  - [ ] Render to ```png```.
  - [ ] Render to ```mp4```.
  - [ ] Headless context (e.g. for commandline HPC use).
- [x] Atom (bill-boarded) imposter spheres.
- [x] Impostor bonds.
- [ ] Bond tube meshes.
- [x] Atom ball meshes (of various triangle counts/LOD)
  - [x] Tetrasphere (4, 16, 64, ...).
  - [ ] Hexosphere (6, ...).
  - [x] Octasphere (8, 32, 128, ...).
  - [x] Icosphere (20, 80, 320, ...).
  - [ ] Dodecasphere (36).
  - [ ] TTP sphere (14, 56, 224, ...).
  - [x] VDW scaled atom balls.
- [ ] Colouring
  - [x] User supplied editable colour sets.
  - [x] CPK.
  - [ ] Velocity colouring.
  - [ ] Force colouring.
- [ ] LOD
  - [ ] Overrideable LOD per atom.
  - [ ] Automatic LOD scaling.
  - [ ] Configurable maximum level of detail (LOD).
- [ ] Desktop app (linux, windows, and macOS).
- [ ] Android app??.

---

## Out of scope

To keep it simple the following features won't be implemented.

- Structures and environments.
- System editing (atom positions, types, bonds).
- Simulation.
- Every structure format under the sun.

## Contributions

Please follow this workflow:

1. Open an issue to discuss your proposed changes.
2. Fork the repository.
3. Make a feature branch in your fork ```git branch <name_of_feature>```.
4. Open a pull request into ```main```.

Contributions will be rebased and merged. Try to keep commits in a PR to 1 if possible. More is acceptable if it improve clarity.

---

Please consider attributing ```SimpleFastOpenAtomicVisualiser, Jerboa (https://jerboa.app), 2025```.
