# A point cloud preparation tool

This is the pcprep project.

# Building and installing

See the [BUILDING](https://github.com/filrg/pcprep/blob/main/BUILDING.md) document.

# Documentation

The The libpcprep developer documentation is available at [filrg.github.io/pcprep](https://filrg.github.io/pcprep) 

# Point Cloud Prepare (pcp)

The user manual for the pcp application can be found in the [USAGE](https://github.com/filrg/pcprep/blob/main/USAGE.md) document.

# Compressed Point Cloud Tiles (fil-CPCTv1) Dataset  

The [`pcprep.sh`](https://github.com/filrg/pcprep/blob/main/pcprep.sh) script was used to generate a dataset of V-PCC-compressed point cloud tiles, derived from the [8i-VFBv2](http://plenodb.jpeg.org/pc/8ilabs) point cloud sequences. This dataset is publicly available [here](https://drive.google.com/drive/folders/1qQp4go6xP--utvJNhyrMi8RexRt6J5Dh).

# Synthetic Trajectory Generation

To edit and create synthetic user's camera trajectory, a tool has been designed to be used in Unity Engine as a script. More information about how to use the scripts can be found [here](https://github.com/nghiantran03/Camera-Path-Tool).

# Examples
## Camera to matrix

This program takes input a JSON file which specified camera parameters, and output the MVP matrixes in a JSON file.
<br/>`cam2mat <input-cam-param.json> <output-cam-matrix.json>`<br/>

For example:
```sh
cam2mat cam-param.json cam-matrix.json
```

# Contributing

See the [CONTRIBUTING](https://github.com/filrg/pcprep/blob/main/CONTRIBUTING.md) document.

