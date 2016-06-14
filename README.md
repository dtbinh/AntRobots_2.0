# Ant Robot 2.0 #
### Small mobile robots running on Raspberry Pi 2 created to study collective excavation in ant colonies ###

#### Contents of this repository: ####

```arduino_antrobots_2.0/src/``` contains arduino program

```arduino_antrobots_2.0/lib/``` contains library for HMC6343 (3 axis compass)

```CAD_files/``` contains all SolidWorks files

```src/``` contains main c++ raspi code

```Makefile``` contains recipe to compile code in ```src/```

```pixycam.prm``` contains setting for pixy camera

```raspi_setup.txt``` contains all documentation on setup and running

To compile follow these steps in this directory:
```
$ mkdir build		// create a build directory
$ make		        // compile or do 'make rebuild' to recomplie
$ sudo ./run		// run the code
```
