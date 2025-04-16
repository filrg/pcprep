# Point Cloud Prepare (PCP)

The **Point Cloud Prepare (PCP)** is a command-line tool designed for preparing, processing, and managing point cloud data. This document provides a guide on how to use the tool effectively.

---

## Usage

```bash
./bin/pcp [OPTION...] -i <point-cloud-source-file> -o <output-source-file(s)>
```

### Description

The `pcp` program processes point cloud (tiles) data from a source file and generates one or more output files based on specified options. It supports customizable processing steps, status calculations, and tiling/merging configurations.


                       Point Cloud (tiles)                     
                                |                           
                                v                           
      +------------------------------------------------+    
      |                  Pre-process                   |    
      |             (TILE, MERGE, or NONE)             |    
      +------------------------------------------------+    
            |                |                    |         
            v                v                    v         
          tile#1           tile#2               tile#T      
            |                |                    |         
            v                v                    v         
    +--------------+ +--------------+     +--------------+  
    |   Process#1  | |   Process#1  | ... |   Process#1  |  
    +--------------+ +--------------+     +--------------+  
            |                |                    |         
            v                v                    v         
           ...              ...                  ...        
            |                |                    |         
            v                v                    v         
    +--------------+ +--------------+     +--------------+  
    |   Process#N  | |   Process#N  | ... |   Process#N  |  
    +--------------+ +--------------+     +--------------+  
            |                |                    |         
            v                v                    v         
    +--------------+ +--------------+     +--------------+  
    |   Status#1   | |   Status#1   | ... |   Status#1   |  
    +--------------+ +--------------+     +--------------+  
            |                |                    |         
            v                v                    v         
           ...              ...                  ...        
            |                |                    |         
            v                v                    v         
    +--------------+ +--------------+     +--------------+  
    |   Status#M   | |   Status#M   | ... |   Status#M   |  
    +--------------+ +--------------+     +--------------+  
            |                |                    |         
            v                v                    v         
      +------------------------------------------------+    
      |                  Post-process                  |    
      |             (TILE, MERGE, or NONE)             |    
      +------------------------------------------------+    
                              |                             
                              v                             
                        Output File(s)                      



---

## Options

### General Options
#### `-b, --binary=0|1`  
  Specifies the output format:  
  - `0`: Output in ASCII format.  
  - `1` (default): Output in binary format.

#### `-i, --input=FILE`  
  Specifies the input point cloud (tiles) source file.  
  - Supported formats: Polygon File Format (`.ply`).
  Example: `tiles%04d.ply` is the input file path for a set of point cloud tiles.
  Example: `longdress0000.ply` is the input file path for a point cloud.
  

#### `-o, --output=FILE`  
  Specifies the output file(s). 
  Example: `tiles%04d.ply` is the output path for multiple output files. 

#### `-?, --help`  
  Displays the help message.

#### `--usage`  
  Displays a short usage message.

#### `-V, --version`  
  Prints the program version.

---

### Set pre-process action Option 
#### `--pre-process=ACTION`

Set the pre-process action of the program (ACTION can be either TILE, MERGE, or NONE, default is NONE). If the input are file path to point cloud tiles, ACTION can only be MERGE or NONE.
  - `TILE`: Tile the point cloud into multiple tiles before performing any process/status. 
  - `MERGE`: Merge the point cloud tiles into one point cloud before performing any process/status. 
  - `NONE`: Do nothing to the point cloud (tiles) before performing any process/status.

### Set post-process action Option 
#### `--post-process=ACTION`
Set the post-process action of the program (ACTION can be either TILE, MERGE, or NONE, default is NONE). Post-process ACTION must be different from pre-process ACTION, except for action NONE.
  - `TILE`: Tile the point cloud into multiple tiles after done performing all process(es)/status(es). 
  - `MERGE`: Merge the point cloud tiles into one point cloud after done performing all process(es)/status(es). 
  - `NONE`: Do nothing to the point cloud (tiles) after done performing all process(es)/status(es).

### Tiling Option
#### `-t, --tile=nx,ny,nz`  
  Set the number of division per axis for TILE action.
  - `nx,ny,nz`: Number of divisions along the x, y, and z axes.  
  Example: `2,2,2`.

### Tiled-input Option
#### `--tiled-input=NUM`
  Specified `NUM` point cloud tiles if the input are point cloud tiles.
  - `NUM`: Number of point cloud tiles input (1 for normal input, default is 1).

---

### Process Option
#### `-p, --process=PROCESS [<ARG>...]`  
  Defines a specific process to be applied to the input point cloud.  
  - `PROCESS`: An string identifier of the process.  
  - `<ARG>,...`: Arguments for the process.  
  Example: `--process=sample 0.5 0`, `-p sample 0.5 0`

#### Sample process
##### `sample <ratio> <binary>`
  Sample the processing point cloud given a ratio.
- `ratio=FLOAT`
  Specifies the sample ratio compare to the processing point cloud.
- `binary=0|1`
  Strategy for sampling.
  | Value | Description                 |
  |:-----:| ----------------------------|
  | 0     | Uniform (default)           |
  | 1     | Still working ...           |

#### Voxel process
##### `voxel <voxel-size>`
  Voxel the processing point cloud given the voxel size.
  - `voxel-size=FLOAT`
  Specifies the step size to voxel the processing point cloud.

#### Remove duplicates process
##### `remove-dupplicates`
  Remove dupplicated point in the processing point cloud.

--- 

### Status Option
#### `-s, --status=STATUS [<ARG>...]`  
  Calculates the status of the input point cloud based on the given factors.  
  - `STATUS`: An string identifier of the status calculation method.  
  - `<ARG>,...`: Arguments for the calculation.  
  Example: `--status=aabb 1 0 bbox%04d.ply`,`-s aabb 1 0 bbox%04d.ply`

#### Axis-Aligned Bounding Box (AABB)
##### `aabb <output> <binary> <output-path>`
  Calculate the Axis-Aligned Bounding Box of the processing point cloud.
- `output=0|1|2`
  Specifies how to get the bounding box:
  - `0` (default): Print on terminal.  
  - `1`: Save to file(s) (Supported formats: `.ply`).
  - `2`: Both of the above.
- `binary=0|1`
  Specifies the output format:  
  - `0`: Output in ASCII format.  
  - `1` (default): Output in binary format.
- `output-path=FILE`
  Specifies the output file(s). 
  Example: `bbox%04d.ply` is the output path for multiple output files. 

#### Pixel per Tile
##### `save-viewport <camera=JSON> <nx,ny,nz> <output-visibility=JSON>`
  Calculate the number of pixels each point cloud tile occupies in the camera viewport when view the processing point cloud from a given camera trajectory.
- `camera=JSON`
  Specifies the JSON file path of the camera trajectory in MVP matrix. An example JSON can be found [here](assets/cam-matrix.json).
- `nx,ny,nz`
  Number of divisions along the x, y, and z axes.  
- `output-visibility=JSON`
  Specifies the output JSON file for each processing point cloud. 

#### Screen Area Estimation
##### `screen-area-estimation <camera=JSON> <output-estimation=JSON>`
Estimate the portion of the screen occupied by the processing point cloud, given a specific camera trajectory.
- `camera=JSON`
  Specifies the JSON file path of the camera trajectory in MVP matrix. An example JSON can be found [here](assets/cam-matrix.json).
- `output-estimation=JSON`
  Specifies the output JSON file for each processing point cloud. 


#### Save Viewport
##### `save-viewport <camera=JSON> <background-color=R,G,B> <output-png(s)=FILE>`
Calculate the camera viewport when view the processing point cloud given a camera trajectory and the background color.
- `camera=JSON`
  Specifies the JSON file path of the camera trajectory in MVP matrix. An example JSON can be found [here](assets/cam-matrix.json).
- `background-color=R,G,B`
  Specifies the background color for the viewport in RGB.
  Example: `255,255,255` for black.
- `output-png(s)=FILE`
  Specifies the output PNG image(s) for each processing point cloud. 
  Example: `view%04d.tile%04d.png`, notice the first `%04d` is for viewport index (if the input JSON have multiple MVP matrixes), second `%04d` is for tile index.

---

## Examples usage

   Import a point cloud and save it to an output file:  
   ```bash
   ./bin/pcp -i input.ply -o output.ply
   ```

2. **Output in Non-Binary Format**  

   Disable binary output for the result:  
   ```bash
   ./bin/pcp -b 0 -i input.ply -o output.ply
   ```

3. **Apply a Processing Step**  

   Sample a point cloud with halves of the points using uniform rule:  
   ```bash
   ./bin/pcp -p 0,0.5,0 -i input.ply -o output.ply
   ```

4. **Tile the Point Cloud**  

   Divide the point cloud into 2x2x2 tiles:  
   ```bash
   ./bin/pcp -i input.ply -o tiles%04d.ply -t 2,2,2
   ```

5. **Combine**

   Tile the point cloud and sample the tiles:
   ```bash
   ./bin/pcp \
       --process=sample 0.5 0 \
       --tile=2,2,2 \
       --binary=0 \
       --input=longdress0000.ply \
       --output=tile%04d.ply
   ```
5. **Sequencing**

   Tile the point cloud and voxel then sample the tiles:
   ```bash
   ./bin/pcp \
       --process=voxel 3 \
       --process=sample 0.5 0 \
       --tile=2,2,2 \
       --binary=0 \
       --input=longdress0000.ply \
       --output=tile%04d.ply
   ```
