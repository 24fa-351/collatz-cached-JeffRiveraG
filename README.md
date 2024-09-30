# Collatz conjecture, cached

Write a program that exercises the Collatz conjecture.
The program should accept three arguments:

1. Number of values to test (N)

2. Smallest value to test (MIN)

3. Largest value to test (MAX)

All three must be present.

* Choose a random number (RN) between MIN and MAX. Apply the formula to it, and count the number of steps until it results in "1". Output the number and number of steps (perhaps CSV?) for import into a GSheet. You don't need to input the intermediate values for the steps between RN and 1. *but you might in test mode if you write one*

* Perform this for N random numbers.

* Exercise with the time command and large enough (and the same) N to be interesting (1000? 10000? 1M?). Report those in one tab of a GSheet, and estimate the mean time for each number to be processed, given the MIN and MAX. 

* Now, put a caching layer (wrapper) in front of the core function (that takes a number and returns the number of steps). Do not modify the core function.

* Add program arguments that specify the cache policy (none, LRU, x) and cache size (int).

* Implement an LRU cache and one of the other basic policies.

  * LFU - Least Frequently Used (if the last digit of your CWID is 0-2)
  
  * FIFO/Round-Robin - First in, First Out (if...3-5)
  
  * MRU/LIFO - Most Recently Used (6-7)
  
  * RR - Random Replacement (8-9)

* In addition to reporting the number of steps for a number, report the cache hit %.

* Determine the smallest cache size for (run spec: N=10000, MIN=1, MAX=500000) that results in an average 30% cache hit over several iterations of this run spec (or multiply N by 5). 

* Do this for both cache types, reporting each in the GSheet with a column indicating cache type.

EXTRA CREDIT: 75 MORE points - Implement ARC in addition. Run it several times with different cache sizes, and log how the parts (T1, B1, etc.) and the pointers ("^," "!") change for one run. (A nicely formatted line is desired) Run it once without the logging enabled to determine the cache size as above.

# Time

This has enough parts that it may take you 8 hours. 

The pieces are not hard; you should use lessons from the fib memorization (DO NOT "memorize" this). 

It is "big" because it's got a lot of parts.

I'd guess you get the core program (fixed number, no cache) in 30 minutes. Another 30 to get the random and looping.

Then it's cache-ville! Please don't hesitate to ask on Discord if you are stuck after spending more than 2 hours trying to figure out one of the caches.

# Notes

* This is for your job! ***Professional!***

*  You choose the file name(s). Write a Makefile.

*  Create a ```.gitignore``` file to skip your executable name.

*  I did not supply any tests.

   *  If it was *me*, I'd add a way to just say "run collatz of N" from the command line so you can test the basic function

   * *Then* consider providing a list of numbers (maybe in a file) so you can test the caching. **Up to you**
 
   
