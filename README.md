# GeekBrains lesson
In this lesson we will create step-by-step the function of recursive OS-directory traversal.  
You will learn from this course:  
* how to use recursion and why it should be avoided.  
* get acquainted with the namespace `std:: filesystem` in C++17.  
* learn how to use 4 ways of code parallelization.  
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
