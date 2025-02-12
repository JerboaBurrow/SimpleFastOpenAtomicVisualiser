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
- **Open**: MIT licensed and open source, enabling
commercial use, adaptation, extension etc. Including the (permissive) upstream licenses in ```LICENSES.md```.

- **Atomic visualisation**: render, colourable, atoms and bonds in 3D space loaded from standard AMSs configuration and trajectory data files.

# Quickstart

To render a structure file ```struct.xyz``` simply call

```shell
sfoav struct.xyz
```

> [!important]
> SFOAV can process ```.xyz```, ```.extxyz```, and DL_POLY ```CONFIG```, ```REVCON``` and ```HISTORY``` files. If the file name does not match these patterns all types will be attempted.

This will bring up the view centring the atoms in ```struct.xyz``` in the first frame (if applicable). The camera is centered on (0, 0, 0) and can be moved in spherical coordinates relative to it. The atoms can also be translated relative to (0, 0, 0).

> [!note]
> Reading of structure files is done in a background thread. For large structure files you may be presented with a loading screen. An intel i7-4790K and Kingston A400 SATA SSD is capable of around 1,000,000 (positions only) atoms per second read.

If the structure file is a trajectory you may scan through its frames moving forward of backward in time using F and B respectively.

> [!note]
> When reading HISTORY files or XYZ/EXTXYZ with multiple frames, SFOAV will cache the filepositions (not data) of each frame in the background. For large trajectory files this may take some time, but you will always be able to play up to the most recently cached frame.

At runtime the following key-controls can be used:

| Key | Action  | Note |
| :----- | :---- | :---- |
| H      | Toggle atom drawing.    | |
| W      | Zoom towards the origin. | |
| S      | Zoom away from the origin. | |
| Q      | Incline the view. | |
| E      | Decline the view. | |
| A      | Rotate the view. | |
| D      | Rotate the view. | |
| LEFT   | Translate the atoms in -x | |
| RIGHT  | Translate the atoms in +x | |
| UP     | Translate the atoms in +z | |
| DOWN   | Translate the atoms in -z | |
| .      | Translate the atoms in -y | |
| /      | Translate the atoms in +y | |
| SPACE  | Reset to the default view and atom positions | At (0,0,0), azimuth Pi/2 and inclination Pi. |
| F      | Move forward in time  | |
| B      | Move backward in time | |
| 1 to 9 | Toggle element emphasis | Elements assigned at startup. |
| X      | Toggle coordinate axes | |
| C      | Toggle simulation cell | |
| ESC    | Quit | |

To enable MSAA at 16x

```shell
sfoav struct.xyz -msaa 16
```

To draw bonds between atoms 1.5 Angstroms apart

```shell
sfoav struct.xyz -bondCutOff 1.5
```

## Performance

For a system with an intel i7-4790K, Kingston A400 SATA SSD, a GTX 1080 ti, and 16 GB available RAM. SFOAV is capable of rendering at least 5,000,000 static atoms at 60 frames per second with 16x MSAA and with a moveable camera. At this scale moving the atoms will run cause drops to 30 fps, and frame increments will cost ~5 seconds.

---

## Features (in development)

- [x] Atom (bill-boarded) imposter spheres.
- [ ] Bonds.
  - [ ] Bond tube meshes.
  - [x] Impostor bonds.
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
  - [ ] Standard colour sets.
    - [x] CPK.
  - [ ] Velocity colouring.
  - [ ] Force colouring.
- [ ] Input
  - [ ] Atom position file formats.
    - [x] XYZ/EXTXYZ.
  - [ ] Atom connectivity file formats.
- [ ] Output
  - [ ] Render to ```png```.
  - [ ] Render to ```gif```.
  - [ ] Headless context (e.g. for commandline HPC use).
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
- Script extensions.
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
