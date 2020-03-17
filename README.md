# Producer-Consumer-System-using-Threads

This project revolves around understanding and using the POSIX thread functions in C.

The program generates a thread for each Producer and Consumer we want to create. There is a circular buffer, that each Producer and Consumer uses interchangeably, which is locked and unlocked using a mutex call so only one thread can access it at a time. Each Producer generates a random number and when the Producer gets added into the buffer it opens up the file "prod_in.txt" and appends it's thread ID and the number it has generated. This process repeats until each producer has generated a desired amont of random numbers. On the contrary, when a Consumer gets removed from the buffer it will open the file "prod_in.txt", find the oldest record that hasnt been checked, and then it will open the file "cons_out.txt" and append it's thread ID and the number it has checked. This process repeats until we have checked all the lines in the file "prod_in.txt". Since we are using threads we have assigned some rules. All Producers wake up and try to access the buffer if the buffer hasnt reached it maximum capacity and if they have to generate more numbers, otherwise they go to sleep. All Consumers wake up and try to access the buffer if the buffer isnt empty and there are still numbers to check in the "prod_in.txt" file. When the program finishes the buffer should be empty and the 2 files (prod_in.txt,cons_out.txt) should show an identical sequence of the numbers that were appended. 

The program when executed requires 5 parameters in order to work.
1st Parameter = Number of Producers
2nd Parameter = Number of Consumers
3rd Parameter = Buffer Capacity
4th Parameter = Amount of numbers that each producer generates
5th Parameter = Seed for the random number generator
