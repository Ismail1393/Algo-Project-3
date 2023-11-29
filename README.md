# Arrow Path Finding Program

## Description
This C++ program reads a matrix of arrows from a file, generates an adjacency list, computes a path based on specific rules, and outputs the resulting path to a file.

## Getting Started
### Prerequisites
- C++ Compiler (Tested with g++)
  
### How to Compile
1. Open terminal/command prompt.
2. Navigate to the directory containing the `finalcopy.cpp` file.
3. Run the following command to compile the code:
    ```sh
    g++ finalcopy.cpp -o arrow_path
    ```
   This will generate an executable file named `arrow_path` in the same directory.

### How to Run
1. Ensure you have an input file named `input.txt` in the same directory as the executable.
2. Run the compiled program using the following command:
    ```sh
    ./arrow_path
    ```
3. The program will read the input file, perform computations, and generate an output file named `Output.txt` with the resulting path.

## File Structure
- `finalcopy.cpp`: Contains the C++ code for the arrow path finding program.
- `input.txt`: Input file containing the matrix of arrows.
- `Output.txt`: Output file containing the computed arrow path.

## File Formats
### Input File Format (`input.txt`)
The input file should contain the following format:
<Number of Rows> <Number of Columns>
<Arrow_1_1> <Arrow_1_2> ... <Arrow_1_n>
<Arrow_2_1> <Arrow_2_2> ... <Arrow_2_n>
...
<Arrow_m_1> <Arrow_m_2> ... <Arrow_m_n>
Each arrow entry should contain the shade and direction separated by a hyphen (`-`).

### Output File Format (`Output.txt`)
The output file contains the computed arrow path in the following format:
<Distance1><Direction1> <Distance2><Direction2> ... <DistanceN><DirectionN>
Where `DistanceX` represents the distance and `DirectionX` represents the direction of the arrows in the computed path.

## Notes
- The program assumes a specific format for input files and generates the path accordingly.
- Any deviations from the expected input format may result in erroneous output.

