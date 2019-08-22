/*****************************************************************
 Title: Home Project
 Author: Ali Almashhadani
 Date Created: 12/13/2018
 Class: Fall 2018, CSCI 34000-02, Mon & Wed 5:35pm-6:50pm
 Professor: Pavel Shostak
 Purpose: Project (CPU Scheduling)
 Description: CPU class functions Declaration
 *****************************************************************/

#ifndef CPU_Class_hpp
#define CPU_Class_hpp


#include <algorithm> 
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <exception>

class CPU {

private:
    // ram varible.
    unsigned long int ram;
    // the number of HD assainge by the user.
    unsigned long int nharddisk;
    // to save the last indext of PID.
    unsigned long int lastIndex;
    // vector for the process table [PID].
    std::vector<std::pair<int, int> > ptable;
    // vector for the process using Hard Drive.
    std::vector<std::pair<int, int> > inHdd;
    // vecotr for the all process.
    std::priority_queue<std::pair<int, int>> pq;
    // vector for the memory status
    std::vector<std::pair<int, long long> > mem;
    // vector for the Hard drives process queue.
    std::vector<std::list<std::pair<int, std::string>>> hdds;
    
    // private utility function for the creating process.
    int allocateMem (unsigned long int , unsigned long int );
    
    // private utility function for the ternimating process.
    void deallocateMem (int );
    
    // private utility function for the creating the Hard drive.
    void initializeHDD (int );
    
    // private utility function for removing process from the Hard Drive.
    void removeProcessFromHDDQ (int );
    
    // private utility function for removing process from the process table [PID].
    void removeProcessFromPTable (int );
    
    // private utility function for removing from CPU to Hard Drive.
    int removeProcessFromAddedToHDD (int );
    
    // private utility function to get the priority of process.
    int getPriority (int );
    
    // private utility function for added the process back to CPU from Hard Drive.
    void addProccessBackInCPU (int , int );


public:
    
    // parameter constructor.
    CPU(unsigned long int , int );
    
    // default destructor
    ~CPU() = default;
    
    // display function.
    void commands_list();
    
    // function take input.
    void user_input();
    
    // public function to creat process.
    void createProcess (int , unsigned long);
    
    // public function to ternimate process.
    void terminateProcess ();
    
    // public function to add process to the Hard Drive.
    void addProcessToHDDQ (int , std::string);
    
    // public function deal with the finished process from the Hard drive.
    void hddFinishProcess (int);
    
    // public function to display all process.
    void showAllProcesses ();
    
    // public function to show process that use Hard Drive
    void showHDDStatus ();
    
    // public function to display the memory status.
    void showMemStatus ();
    
    // public utility function that split the user input.
    void split(const std::string & , std::vector<std::string> &, char );
    
};

// Driver function to sort the vector of proccess
// by second element of pairs
bool sortbysec(const std::pair<int,int> &a,
               const std::pair<int,int> &b);
#endif /* CPU_Class_hpp */
