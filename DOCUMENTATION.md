```
 ________  ________ ________  ________  ___      ___
|\   ____\|\  _____\\   __  \|\   __  \|\  \    /  /|
\ \  \___|\ \  \__/\ \  \|\  \ \  \|\  \ \  \  /  / /
 \ \_____  \ \   __\\ \  \\\  \ \   __  \ \  \/  / /
  \|____|\  \ \  \_| \ \  \\\  \ \  \ \  \ \    / /
    ____\_\  \ \__\   \ \_______\ \__\ \__\ \__/ /
   |\_________\|__|    \|_______|\|__|\|__|\|__|/
   \|_________| SimpleFastOpenAtomicVisualiser
```

# About

SimpleFastOpenAtomicVisualiser (SFOAV) is intented to enable fast visualisation of atomic and molecular structure files and trajectories.

It is an MIT licensed C++ project hosted here [https://github.com/JerboaBurrow/SimpleFastOpenAtomicVisualiser/](https://github.com/JerboaBurrow/SimpleFastOpenAtomicVisualiser). The visualisation are produced using OpenGL 3.3. There are options for "ray-traced" atoms and bonds or procedural meshes.

You can find the latest html Doxygen docs at [https://jerboaburrow.github.io/SimpleFastAtomicVisualiser/](https://jerboaburrow.github.io/SimpleFastAtomicVisualiser/).

The API will be very unstable until 0.1.X, expect many breaking changes until then.

# Quickstart

To render a structure file ```struct.xyz``` simply call

```shell
sfoav -atoms struct.xyz
```

> [!important]
SFOAV can process ```.xyz```, ```.extxyz```, and DL_POLY ```CONFIG```, ```REVCON``` and ```HISTORY``` files. If the file name does not match these patterns all types will be attempted.

This will bring up the view centring the atoms in ```struct.xyz```. The camera can be rotated around the origin using ```A``` and ```S``` for distance, ```W``` and ```D``` for the azimuthal angle and ```Q``` and ```E``` for inclination angle. The atoms' Van der Waals sphere may be hidden and unhidden using ```H```.

To enable MSAA at 16x

```shell
sfoav -atoms struct.xyz -msaa 16
```

To draw bonds between atoms 1.5 Angstroms apart

```shell
sfoav -atoms struct.xyz -bondCutOff 1.5
```

> [!tip]
Meshes are much slower than the ray-traced elements due to higher triangle counts.

To render using meshes at 10 levels of detail

```shell
sfoav -atoms struct.xyz -levelOfDetail 10 -meshes
```

To render with only Tetrahedral bases meshes at 5 levels of detail

```shell
sfoav -atoms struct.xyz -levelOfDetail 5 -meshes 4
```