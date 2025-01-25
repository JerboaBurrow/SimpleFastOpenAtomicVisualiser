## SimpleFastOpenAtomicVisualiser (SFOAV)
<p align="center">
  <img height=256px src="https://github.com/user-attachments/assets/feb05885-b8ac-4417-9f60-4fa4acce35c5" />
  <img height=256px src="https://github.com/user-attachments/assets/8f109e8a-aa71-425a-8694-270695ce138c"/>
  <br>[Left] 125,000 "Atoms" in real time with 16x MSAA and 128 triangles per ball.
  <br>[Right] Pislocybin molecule with 2 triangles per bond (ray-traced).
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

This will bring up the view centring the atoms in ```struct.xyz```.

The camera is centered on (0, 0, 0) and can be moved in spherical coordinates relative to it. The atoms can also be translated relative to (0, 0, 0).

At runtime the following key-controls can be used:

| Key | Action  |
| :----- | :---- |
| H      | Toggle atom drawing.    |
| W      | Zoom towards the origin. |
| S      | Zoom away from the origin. |
| Q      | Incline the view. |
| E      | Decline the view. |
| A      | Rotate the view. |
| D      | Rotate the view. |
| LEFT   | Translate the atoms in -x |
| RIGHT  | Translate the atoms in +x |
| UP     | Translate the atoms in +z |
| DOWN   | Translate the atoms in -z |
| .      | Translate the atoms in -y |
| /      | Translate the atoms in +y |
| SPACE  | Reset to the default view and atom positions |
| F      | Move forward in time  |
| B      | Move backward in time |

To enable MSAA at 16x

```shell
sfoav struct.xyz -msaa 16
```

To draw bonds between atoms 1.5 Angstroms apart

```shell
sfoav struct.xyz -bondCutOff 1.5
```

## Meshes

> [!tip]
> Meshes are much slower than the ray-traced elements due to higher triangle counts.

To render using meshes at 10 levels of detail

```shell
sfoav struct.xyz -levelOfDetail 10 -meshes
```

To render with only Tetrahedral bases meshes at 5 levels of detail

```shell
sfoav struct.xyz -levelOfDetail 5 -meshes 4
```

The available meshes are.

| Mesh | value of -meshes  | Note |
| :----- | :---- | :---- |
| ICOSAHEDRON | 0 ||
| OCTAHEDRON | 1 ||
| DODECAHEDRON | 1 | Known issues at higher LOD|
| CUBE | 3 |Known issues at higher LOD|
| TETRAHEDRON | 4 ||
| TRIAUGMENTED_TRIANGULAR_PRISM | 5 | Known issues at higher LOD|
| ANY | 6 | Uses all mesh types controlled by LOD|

The maximum level of detail is 7 for invidual meshes and 23 for ANY. This is the number of refinements to the mesh or for ANY refinements for all meshes ordered by triangle count.

---

## Features (in development)

- [x] Atom ball meshes (of various triangle counts/LOD)
  - [x] Tetrasphere (4, 16, 64, ...).
  - [ ] Hexosphere (6, ...).
  - [x] Octasphere (8, 32, 128, ...).
  - [x] Icosphere (20, 80, 320, ...).
  - [ ] Dodecasphere (36).
  - [ ] TTP sphere (14, 56, 224, ...).
  - [x] VDW scaled atom balls.
<p align="center">
  <img src="https://github.com/user-attachments/assets/f2533df3-28fc-4b32-8954-1735ceb001b0"/>
  <br>A motley collection of sphere meshes for LOD scaling.
</p>

- [ ] LOD
  - [x] Overrideable LOD per atom.
  - [ ] Automatic LOD scaling.
  - [ ] Configurable maximum level of detail (LOD).
- [x] Atom (bill-boarded) imposter spheres.
- [ ] Bonds.
  - [ ] Bond tube meshes.
  - [x] Impostor bonds.
- [ ] Colouring
  - [ ] User supplied editable colour sets.
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
- [ ] Desktop app (linux, windows, and macOS).
- [ ] Android app.

---

## Out of scope

To keep it simple the following features won't be implemented. For these and more use a complex feature rich visualiser like VMD https://www.ks.uiuc.edu/Research/vmd/.

- Structures and environments.
- System editing (atom positions, types, bonds).
- Script extensions.
- Simulation.

## Contributions

Please follow this workflow:

1. Open an issue to discuss your proposed changes.
2. Fork the repository.
3. Make a feature branch in your fork ```git branch <name_of_feature>```.
4. Open a pull request into ```main```.

Contributions will be rebased and merged. Try to keep commits in a PR to 1 if possible. More is acceptable if it improve clarity.

---

Please consider attributing ```SimpleFastOpenAtomicVisualiser, Jerboa (https://jerboa.app), 2025```.
