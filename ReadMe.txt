Vector logical clock simulation

1. In order to implement Vector clock below mentioned files are used:
    • modelUsed.h
          This file contains the model we are using to implement Vector logical clock [means declaration of classes, structures, functions and global variables are present here]
          
    • modelUsed.cpp
          This file contains the defination of declared classes and functions in declared “modelUsed.h”.
          
    • vector_Logical_Clock.cpp
          This file contains the main deriving functions.
          
    • eventSeqInput.txt
          This file is used to read the input from. All the event sequence for all the processes are defined here.

2. How to compile and run the simulation
    • Input is read from file, hence write the desired event sequence for processes in “eventSeqInput.txt” file.
      
    • Ensure all the above mentioned 4 files are in same directory.
      
    • Compile the code using below command from terminal
        ◦ g++ vector_Logical_Clock.cpp -o VC
          
    • Run the code using below command from terminal
        ◦ ./VC
          
    • Output will be printed on terminal. If some error or deadlock is encountered it will be highlighted in the output.

3. Allowed format of events/operations  sequence to be given in input file for processes
    • begin process <processID>
    • end process <processID>
    • print <dataToPrint>
    • recv <processID> <msg>
    • send <processID> <msg>
      
[NOTE: The format above are case sensitive and if command is not as per above format ERROR will be raised]


