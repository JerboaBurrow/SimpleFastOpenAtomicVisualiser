## SimpleFastOpenAtomicVisualiser (SFOAV)
<p align="center">
  <img src="https://github.com/user-attachments/assets/feb05885-b8ac-4417-9f60-4fa4acce35c5" />
  <br>125,000 "Atoms" in real time with 16x MSAA and 128 triangles per ball.
</p>
A simple program to quickly visualise atomic and molecular systems (AMSs).

The code is accelerated using OpenGL for 3D rendering and builds for Linux, Windows, macOS, and Android.

#### What it is

- **Simple**: a trivial interface for loading and displaying AMSs configurations and trajectories. Via command line or GUI. 

- **Fast**: rendering as fast as e.g. VMD or other 3d-viewers, with many efficiency options for level of detail.
- **Open**: MIT licensed and open source, enabling 
commercial use, adaptation, extension etc. Including the (permissive) upstream licenses in ```LICENSES.md```.

- **Atomic visualisation**: render, colourable, atoms and bonds in 3D space loaded from standard AMSs configuration and trajectory data files.

---

#### Features (in development)

- [ ] Atom ball meshes (of various triangle counts)
  - [ ] Hierarchical triangular mesh (8, 32, 128, ...).
  - [ ] Icosphere (20, 80, 320, ...).
  - [ ] Cubosphere (6, 24, 96, ...).
- [ ] Atom (bill-boarded) imposter spheres.
- [ ] Bond tube meshes.
- [ ] VDW scaled atom balls.
- [ ] User supplied editable colour sets.
- [ ] Standard colour sets.
- [ ] Configurable level of detail (LOD).
- [ ] Velocity colouring.
- [ ] Force colouring.
- [ ] Render to ```png```.
- [ ] Render to ```gif```.
- [ ] Headless context (e.g. for commandline HPC use).
- [ ] Atom position file formats: DL_POLY CONFIG, XYZ, EXTXYZ, CIF.
- [ ] Atom connectivity file formats: DL_POLY FIELD.
- [ ] Android app.

---

#### Out of scope

To keep it simple the following features won't be implemented. For these and more use a complex feature rich visualiser like VMD https://www.ks.uiuc.edu/Research/vmd/.

- Publication quality images.
- Ray tracing.
- Materials, fog, detailed lighting, PBR, and other effects.
- Structures and environments.
- System editing (atom positions, types, bonds).
- Script extensions.
- Simulation.

#### Contributions

Please follow this workflow:

1. Open an issue to discuss your proposed changes.
2. Fork the repository.
3. Make a feature branch in your fork ```git branch <name_of_feature>```.
4. Open a pull request into ```main```.

Contributions will be rebased and merged. Try to keep commits in a PR to 1 if possible. More is acceptable if it improve clarity.

---

Please consider attributing ```SimpleFastOpenAtomicVisualiser, Jerboa (https://jerboa.app), 2025```.
