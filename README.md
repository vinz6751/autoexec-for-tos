# autoexec for TOS
Atari TOS program to run programs with command line during boot.

This program was created because the TOS doesn't allow passing command line parameters to programs while launching from the AUTO folder during boot.
So this program does it, it takes a AUTOEXEC.CNF file from the root, which is just a list of commands to execute, and executes them in sequence.

The command line parameters can only be 124 characters long because that's what the TOS is limited to.
Strings starting with a hash sign '#' or empty lines are ignored.

Each line then looks like:
<full_program_path> <parameters>

Spaces and tabs between program name and parameter are ignored.
Les lignes vides sont ignor‚es.
If you want messages during the execution add this line:
*VERBOSE ON
Or, to disable:
*VERBOSE OFF
This "VERBOSE" mode can be changed at any time.

Advantages of this program:
* One can specify command line parameters
* One can have different autoexec.cnf files and select one of them using a boot selector.
* One can change the order of execution of programs in the AUTO folder without having to mess with the physical order of files in the filesystem.

This program was written with MicroEmacs 4 on the Foenix A2560U, with Pure C as C compiler and Otto Thorsten's PCMAKE to run the project file.


Vinz/MPS
Decembre 30th 2023

Public domain
