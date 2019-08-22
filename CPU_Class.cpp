/*****************************************************************
 Title: Home Project
 Author: Ali Almashhadani
 Date Created: 12/13/2018
 Class: Fall 2018, CSCI 34000-02, Mon & Wed 5:35pm-6:50pm
 Professor: Pavel Shostak
 Purpose: Project (CPU Scheduling)
 Description: CPU class functions implementation
 *****************************************************************/

#include "CPU_Class.hpp"

// parameter constructor will initializes the CPU class with the RAM and Disk number needed by the user.
CPU::CPU(unsigned long int cram, int cnharddisk) {
    // the ram needed.
    ram = cram;
    // number of disk needed.
    nharddisk = cnharddisk;
    // what the process number.
    lastIndex = 0;
    // initializing the hard memory.
    mem.push_back(std::make_pair(0, cram));
    
    // initializing the hard drive number.
    initializeHDD(cnharddisk);
    
    // print.
    std::cout<<"CPU created"<<std::endl;
}

// Function will print all the commands the user need to know to run the program.
void CPU::commands_list(){
    std::cout <<std::endl;
    std::cout<<"Welcome to the CPU scheduling program"<<std::endl;
    std::cout<<"Here is the list of commands you need to run the program;"<<std::endl;
    std::cout<<"1 - (A [Priority number] [Ram needed]) To add process" << std::endl;
    std::cout<<"2 - (t) To Terminate a process in the CPU" <<std::endl;
    std::cout<<"3 - (d [disk number] [file name]) To allow the process currenly using the CPU to use I/O" <<std::endl;
    std::cout<<"4 - (D [disk number]) To let the current process that use the disk finish"<<std::endl;
    std::cout<<"5 - (S r) To view all process that are using the CPU and in the ready queue"<<std::endl;
    std::cout<<"6 - (S i) To view all process that are using I/O"<<std::endl;
    std::cout<<"7 - (S m) To view the memory distribution between process" <<std::endl;
}

// Function will take input from the user and do based on the commands above.
void CPU::user_input(){
    // the input from the user.
    std::string input_line;
    // to ignored the cin that happen in the main function.
    std::cin.ignore();
    // keep the loop running till the user ternimate the program.
    while(true) {
        // print.
        std::cout<<">> ";
        // input.
        getline(std::cin, input_line);
        // split input vector that will has alll the input from the user.
        std::vector<std::string> splitInput;
        // calling the split utility function which will help slpiting the input into parts and check with the below conditions.
        split(input_line, splitInput, ' ');
        
        // gard if the user input something wrong.
        try{
            // the if statments conditions as menations above.
            if ((0 == (splitInput.at(0).compare("a"))) || (0 == (splitInput.at(0).compare("A")))){
                createProcess(stoi(splitInput.at(1)), stoi(splitInput.at(2)));
            } else if (0 == splitInput.at(0).compare("t")) {
                terminateProcess();
            } else if (0 == splitInput.at(0).compare("d")) {
                addProcessToHDDQ(stoi(splitInput.at(1)), splitInput.at(2));
            } else if (0 == splitInput.at(0).compare("D")) {
                hddFinishProcess(stoi(splitInput.at(1)));
            } else if ((0 == splitInput.at(0).compare("S")) || (0 == splitInput.at(0).compare("s"))) {
                if (0 == splitInput.at(1).compare("r")) {
                    showAllProcesses();
                } else if (0 == splitInput.at(1).compare("i")) {
                    showHDDStatus();
                } else if (0 == splitInput.at(1).compare("m")) {
                    showMemStatus();
                }else{
                    std::cout<<"Nothing to input" << std::endl;
                }
            }else{
                std::cout<<"Nothing to input" << std::endl;
            }
        // to catch any wrong input.
        }catch(std::exception& e){
            std::cout<< "Wrong input!"<< std::endl;
        }
    }
}

// Public function that will create a process for the CPU class.
void CPU::createProcess (int priority, unsigned long reqRam) {
    // getting the PID.
    lastIndex = lastIndex + 1;
    
    // gard to check if we have enough meory in the RAM.
    if (allocateMem(lastIndex, reqRam) == -1) {
        std::cout<<"Not enough free memory to handle this process"<<std::endl;
        lastIndex = lastIndex - 1;
        return;
    }
    
    // pushing the PID and priority to the Pricee table vector.
    ptable.push_back(std::make_pair(lastIndex, priority));
    
    // pushing to the process queue the process and it priority.
    pq.push(std::make_pair(priority, lastIndex));
}

// Public function that will ternimated the process when the process finish using the CPU.
void CPU::terminateProcess () {
    // Gard to check if there no process using the CPU.
    if (pq.empty()) {
        std::cout<<"No process running currently"<<std::endl;
        return;
    }
    // getting the top [the peocess that using the CPU now]  from the process queue.
    std::pair<int, int> currentRunning = pq.top();
    // to remove the process.
    pq.pop();
    
    // Below, I am doing the following to make sure the process is completely removed from the OS.
    // remove its memory.
    deallocateMem(currentRunning.second);
    // remove it from the HHD table.
    removeProcessFromHDDQ(currentRunning.second);
    // remove it from the vector to add to the HHD
    removeProcessFromAddedToHDD(currentRunning.second);
    // remove it from the procee table [PID]
    removeProcessFromPTable(currentRunning.second);
}

// Public function will take process from the CPU and added to the hard disk.
void CPU::addProcessToHDDQ (int hddNo, std::string filename) {
    // Gard to see if the hard disk input is exist.
    if (hddNo >= hdds.size()) {
        std::cout<<"This hard-disk does not exist"<<std::endl;
        return;
    }
    // Gard to check if there process using CPU.
    if (pq.empty()) {
        std::cout<<"No process to add to hard-disk"<<std::endl;
        return;
    }
   
    // take the process and file name and push it to the hard disk vector with it index.
    hdds[hddNo].push_back(std::make_pair(pq.top().second, filename));
    // push the it also to the hard disk number vector.
    inHdd.push_back(std::make_pair(pq.top().second, pq.top().first));
    // removing the porcess from the CPU.
    removeProcessFromPTable(pq.top().second);
    // remove it from the queue.
    pq.pop();
}

// Public function to move the process that finish using I/O back to the CPU.
void CPU::hddFinishProcess (int hddNo) {
    // Gard to see if the hard disk number exist.
    if (hddNo >= hdds.size()) {
        std::cout<<"This harddisk does not exist"<<std::endl;
        return;
    }
    // Gard to check if there process using CPU.
    if (hdds[hddNo].empty()) {
        std::cout<<"This hard-disk is already IDLE"<<std::endl;
        return;
    }
    // Get the process with its index to the pair popped.
    std::pair<int, std::string> popped = hdds[hddNo].front();
    // take it out from the hard drive queue.
    hdds[hddNo].pop_front();
    // get it priority.
    int priority = removeProcessFromAddedToHDD(popped.first);
    // push it back to the process table.
    ptable.push_back(std::make_pair(popped.first, priority));
    // Adding the process and checking if it has heigh priorpty the we can preemitive the process in the CPU.
    addProccessBackInCPU(priority, popped.first);
}

// Public function to show all process that using the CPU and in the ready queue.
void CPU::showAllProcesses () {
    // Gard to see if there process is running in the CPU.
    if (pq.empty()) {
        std::cout<<"No process running currently"<<std::endl;
        return;
    }
    // print the first process that using the CPU.
    std::cout<<"Current running - "<<"PID "<<pq.top().second<<" Priority "<<pq.top().first<<std::endl;
    // ready queue.
    std::cout<<"Ready queue"<<std::endl;
    // check if there is a ready queue.
    if(pq.size() < 2){
        std::cout<<"Empty"<<std::endl;
        return;
    }
    // sorting the process from the highest priority to the lowest.
    std::sort(ptable.begin(), ptable.end(), sortbysec);
    // printing the process.
    for (int i=0; i<ptable.size(); i++) {
        // checking if if the top process is similar to the first in the process table [PID].
        if (ptable[i].first == pq.top().second) {
            continue;
        }
        // printing.
        std::cout<<"PID "<<ptable[i].first<<" Priority "<<ptable[i].second<<std::endl;
    }
    
}

// Public function that will show all the process that using I/O.
void CPU::showHDDStatus(){
    // index.
    int i=0;
    // range loop.
    for (auto q: hdds) {
        // itertor.
        std::list <std::pair<int, std::string> > :: iterator it;
        
        // check if the disk is not in use.
        if (q.empty()) {
            // printing.
            std::cout<<"Disk "<<i<<" is IDLE"<<std::endl;
            // incrementing.
            i++;
            continue;
        }
        // flag to make the print know which is first ans which is in the ready queue.
        bool first = true;
        // loop.
        for (it = q.begin(); it!=q.end(); ++it) {
            if (first) {
                // printing.
                std::cout<<"Disk "<<i<<" Running PID "<<(*it).first<<" File - "<<(*it).second<<std::endl;
                // update the flag.
                first = false;
            } else {
                // printing.
                std::cout<<"Disk "<<i<<" queue's PID "<<(*it).first<<" File - "<<(*it).second<<std::endl;
            }
        }
        // incrementing.
        i++;
    }
}

// Public function that will show the memory status for each process.
void CPU::showMemStatus () {
    // starting index.
    long long start = 0;
    // checking if there no process at all in our CPU.
    if (mem.size() == 1 && mem[0].first == 0) {
        std::cout<<"No process is using memory"<<std::endl;
    }
    // for loop to print all status.
    for (int i=0; i<mem.size(); i++) {
        //  getting the status by it index.
        std::pair<int, long long> citem = mem[i];
        
        if (citem.first == 0) {
            // add up the memory.
            start+=citem.second;
        } else {
            // printing.
            std::cout<<"PID "<<citem.first<<" Mem range "<<start<<" - "<<(start+citem.second) - 1 <<std::endl;
            // adding memory.
            start+=citem.second;
        }
    }

}

/************************************************************************/
/******************* Private Utility Functions *************************/

// Private utility function that allecate the memory for each process.
// also will check if there is enough memory to place a process in the RAM.
int CPU::allocateMem (unsigned long int pid, unsigned long int reqRam) {
    
    // checking to where put the process in the memory.
    int bestfitBlockInd = -1;
    // getting how much memory we got now.
    long long memDiff = ram;
    
    // checking all the possilbe memory.
    for (int i=0; i<mem.size(); i++) {
        // getting the memory at the location.
        std::pair<int, long long> memItem = mem.at(i);
        
        // checking for if the rang of the set is good.
        if (memItem.first == 0 && memItem.second >= reqRam && (memItem.second - reqRam) < memDiff ) {
            // taking the index.
            bestfitBlockInd = i;
           // getting the memory different [how much memory we need for this process].
            memDiff = (memItem.second - reqRam);
        }
    }
    
    // checking if we need more.
    if (bestfitBlockInd > -1) {
        // getting better location.
        std::pair<int, long long> memItem = mem.at(bestfitBlockInd);
        //  setting the pair.
        mem[bestfitBlockInd] = std::make_pair(pid, reqRam);
        // chekcing if it good. 
        if (memItem.second != reqRam) {
            mem.insert(mem.begin()+bestfitBlockInd+1, std::make_pair(0,memItem.second - reqRam));
        }
    }
    return bestfitBlockInd;
}

// Private utility function that deallcate the memory after ternimating it.
void CPU::deallocateMem (int pid) {
    // getting the size memory.
    unsigned long int initialMemSize = mem.size();
    // loop for the size of the memory.
    for (int i=0; i<initialMemSize; i++) {
        // getting the memory at the index.
        std::pair<int, unsigned int> memItem = mem.at(i);
        
        // check if the pid match
        if (memItem.first == pid) {
           // getting the process with it.
            unsigned int blockSize = memItem.second;
            // add it memoty back.
            mem[i] = std::make_pair(0, blockSize);
            
            // check for the index if it still not 0.
            if (i > 0) {
                // deallcating.
                if (mem.at(i-1).first == 0) {
                    mem[i] = std::make_pair(0, blockSize + mem.at(i-1).second);
                    mem.erase(mem.begin()+i-1);
                    i=i-1;
                }
            }
            
            // if the size of the meoery bigger than index.
            if (i < (mem.size() - 1)) {
                // deallcating.
                if (mem.at(i+1).first == 0) {
                    mem[i] = std::make_pair(0, mem.at(i).second + mem.at(i+1).second);
                    mem.erase(mem.begin()+i+1);
                }
            }
            // breaking.
            break;
        }
    }
    
}

// Private function to start the Hard Disk.
void CPU::initializeHDD (int nHDD) {
    for (int i=0; i<nHDD; i++) {
        std::list<std::pair<int, std::string>>  temp;
        // pushing back for the hard disk vector.
        hdds.push_back(temp);
    }
}

// Private function to remove process from the Hard disk.
void CPU::removeProcessFromHDDQ (int pid) {
    // loop for all process.
    for (std::list<std::pair<int, std::string> > q : hdds) {
        // creating iterator.
        std::list <std::pair<int, std::string> > :: iterator it;
        // check what is the right posiation for the process.
        for (it = q.begin(); it!=q.end(); ++it) {
            if ((*it).first == pid) {
               // take it out of the queue.
                it = q.erase(it);
            }
        }
    }
}

// Private function to remove the process from the process table.
void CPU::removeProcessFromPTable (int pid) {
    // for loop to check for the PID/
    for (int i=0; i<ptable.size(); i++) {
        // check if we got the right PID.
        if (ptable[i].first == pid) {
            // if we got it, delele it.
            ptable.erase(ptable.begin()+i);
        }
    }
}

// Private utility function to help remove procee from CPU and add it to Hard Disk.
int CPU::removeProcessFromAddedToHDD (int pid) {
    // loop.
    for (int i=0; i<inHdd.size(); i++) {
        // check with PID.
        if (inHdd[i].first == pid) {
            int priority = inHdd[i].second;
            // remove.
            inHdd.erase(inHdd.begin()+i);
            // get the priority to check with it later.
            return priority;
        }
    }
    // if there is nothing, the we return -1.
    return -1;
}

// Private utility to help get the input from the user.
// It will take the input and try to split it in part.
void CPU::split(const std::string &txt, std::vector<std::string> &strs, char ch) {
    //getting the space pos.
    size_t pos = txt.find(ch);
    // first pos
    size_t initialPos = 0;
    // clear the input.
    strs.clear();
    // check where is the space.
    while (pos != std::string::npos) {
        // getting the input.
        strs.push_back(txt.substr(initialPos, pos-initialPos));
        // getting to the next one after space.
        initialPos = pos + 1;
        // finding the space.
        pos = txt.find(ch, initialPos);
    }
    // setting the input to the splited words.
    strs.push_back(txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ));
}

// Private utility that will return the priority of a process.
int CPU::getPriority (int pid) {
    // loop.
    for (int i=0; i<ptable.size(); i++) {
        // if it match with PID.
        if (ptable[i].first == pid) {
            // return the PID.
            return ptable[i].second;
        }
    }
    // if nothing then return -1/
    return -1;
}

//  Private utility that will add the process back to the CPU after it finish I/O
void CPU::addProccessBackInCPU (int priority, int pid) {
    // it will push the process into the process qeueu.
    pq.push(std::make_pair(priority, pid));
}


// Driver function to sort the vector elements
// by second element of pairs
bool sortbysec(const std::pair<int,int> &a,
               const std::pair<int,int> &b){
    return (a.second > b.second);
}
