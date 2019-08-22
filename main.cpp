/*****************************************************************
 Title: Home Project
 Author: Ali Almashhadani
 Date Created: 12/13/2018
 Class: Fall 2018, CSCI 34000-02, Mon & Wed 5:35pm-6:50pm
 Professor: Pavel Shostak
 Purpose: Project (CPU Scheduling)
 Description: Driver File
 *****************************************************************/

#include <iostream>
#include "CPU_Class.hpp"
#include "CPU_Class.cpp"

using namespace std;

int main(int argc, char const *argv[]){
    // taking the Memory input from the user.
    unsigned long int ram;
    // taking the number of the disk from the user.
    int nharddisk;
    
    // printing.
    cout<<"How much RAM does your computer have: ";
    // input.
    cin >> ram;
    // printing.
    cout<<"Number of hardisks that your computer have: ";
    // input.
    cin >> nharddisk;
    // creating object of the class with the [memory, disk number neede] from the user input.
    CPU cpu1(ram , nharddisk);
    //printing the commands to the screen needed for the user.
    cpu1.commands_list();
    // taking input commands from the user to run the CPU Scheduling.
    cpu1.user_input();

}
