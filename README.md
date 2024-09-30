# ANEMOS
ANEMOS is a Near-Memory Simulation framework to model Compute-near-Memory cores integrated in DRAM.

The initial release is described in the following paper:
>R. Medina et al.
>"[**Bank on Compute-near-Memory: Design Space Exploration of Processing-near-Bank Architectures**](https://infoscience.epfl.ch/handle/20.500.14299/240684)".
>In ESWEEK - CODES+ISSS, October 2024.

## Dependencies

- [SystemC](https://github.com/accellera-official/systemc/tags) 2.3 build with C++11.
- [Ramulator](https://github.com/CMU-SAFARI/ramulator) patched with the files in [ramulator_files](./ramulator_files/).

## Execution

After installing the dependencies, the correct [environment variables](#environment-variables) should be defined to point to the correct folders.
Then, the default explorations for bank-level CnM processing units can be run by executing the batch scripts in the [scripts](./scripts/) folder:

```
cd scripts
bash run_kernels_hbmCR.sh
```

## Project structure

- 📁 [**build**:](./build/) build folder.
- 📁 [**eda_script**:](./eda_scripts/) TCL files for:
    - 📁 [**catapult**:](./eda_scripts/catapult/) high level synthesis of SystemC model using Catapult HLS. 
    - 📁 [**genus**:](./eda_scripts/genus/) synthesis and power estimation of RTL model using Cadence Genus. 
    - 📁 [**questasim**:](./eda_scripts/questasim/) simulation of SystemC and RTL models. 
- 📁 [**inputs**:](./inputs/) files and traces employed for the programming interface.
- 📁 [**ramulator_files**:](./ramulator_files/) patched files to support all-bank DRAM mode.
- 📁 [**scripts**:](./scripts/) bash scripts to run parameterized explorations.
- 📁 [**src**:](./src/) source files of SystemC template.
- 📁 [**waveforms**:](./waveforms/) GTKWave format files to analyze VCD waveforms.

## Environment variables
- **${PROJECT_ROOT}**: Root folder of cloned repository.
- **${RAMULATOR_ROOT}**: Root folder of patched ramulator.
- **${EDA_PROJECT_ROOT}**: Root folder of EDA project for HLS, synthesis and simulation.
- **${VCD_FILES}**: Folder to store VCD files output by the post-synthesis simulation.
- **${HLS_GCC}**: GCC version employed in High Level Synthesis (Originally employed 5.4.0).
