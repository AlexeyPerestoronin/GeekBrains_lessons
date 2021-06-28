# GeekBrains lesson
In this lesson we will create step-by-step the function of recursive OS-directory traversal.  
You will learn from this course:  
* how to use recursion and why it should be avoided.  
* get acquainted with the namespace `std:: filesystem` in C++17.  
* learn how to use 4 ways of code parallelization.  
***
All material can be found this: https://onedrive.live.com/?authkey=%21AJmMTpMiNU0IJks&id=81DDEC6BD4DE0C58%21189234&cid=81DDEC6BD4DE0C58
***

## step-1
We create a simple function that recursively traverses the directories of the file system starting from some initial directory and outputs all the files found to the console.  

## step-2
Adding the crawl depth parameter - `deep`.  

## step-3
Eliminates function defects:  
* passing the parameter via the `fs::directory_entry` instead of `fs:: path`.  
* removing the recursive call.  

## step-4
Adding:  
* output of directories in tree format.  
* optional setting of the output in **depth** or **width**.  
* improving tests: a test file search directory is created for each test.  

## step-5
Makes improvements:  
* wrapper of the solution in the form of a class.  
* generalization of the class to the type of work with files.  

## step-6
* multithreaded implementation.  

## step-7
Makes improvements:  
* creating a list type that is resistant to adding and extracting data.  
* we use `std::atomic` for tracking the presence of divided work.  
* replacing the implementation of parallelization from `std::thread` to `std::future`.  

## step-8
Upgrading the code:  
* replacing the `std::future` parallelization implementation with **SLT-algorithms**.  
* improving the tests: adding the output of the number of threads involved.  

## step-9
Makes improvements:  
* making the method of implementing parallelization a configurable template parameter.  
* improving tests: compare the speed of different parallelization methods.  
