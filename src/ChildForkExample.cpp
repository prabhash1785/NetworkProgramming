// ChildForkExample.cpp
//
// Parent forks two children and then each child performs designated task for specified duration.
// When a child dies, parent cleans up resources by calling waitpid. Once all children dies,
// parent quits.
//
// @author Prabhash Rathore

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

using namespace std;

void performTask(const void*);

int main() {
    const int childCount = 2;
    const int childRunDuration[childCount] = {3, 6};
    pid_t myPid = getpid();
    cout << "My PID is: " << myPid << endl;  
    cout << "Going to spawn children: " << childCount << endl;

	pid_t child[childCount];

	for(int i = 0; i < childCount; i++) {
		pid_t pid = fork();

		if(pid == -1) {
			cout << "Failed to fork a child with error: " << errno << endl;
			return 1;
		} else if(pid > 0) {
            cout << "Forked child " << (i + 1) << " with pid: " << pid << endl;
            child[i] = pid;
        }	else {
            cout << "Going to start my task. I am child # " << (i + 1) << endl;
            performTask(childRunDuration + i);
            cout << "Child is going to EXIT.." << endl;
            exit(0); // exit out here so that child does not run undesired parent process code
        }
	}

    cout << "Here are all children pids:" << endl;
    for(int x = 0; x < childCount; x++) {
        cout << "Child PID = " << child[x] << endl;
    }

    // collect dead child
    cout << "Parent is going to wait to collect dead children..... " << endl;
    int deadChildCount = 0, wstatus, deadPid;
    do {
           cout << "Parent spinning on waitpid, deadChild so far: " << deadChildCount << endl;
           deadPid = waitpid(0, &wstatus, WUNTRACED | WCONTINUED);
           
           if (deadPid == -1) {
               perror("waitpid error.. parent is going to exit");
               exit(EXIT_FAILURE);
           }

           cout << "Parent thinks a child died: " << deadPid << endl;
           ++deadChildCount;
           if (WIFEXITED(wstatus)) {
               cout << deadPid << " exited, status= " << WEXITSTATUS(wstatus) << endl;
           } else if (WIFSIGNALED(wstatus)) {
               cout << deadPid << " killed by signal " << WTERMSIG(wstatus) << endl;
           } else if (WIFSTOPPED(wstatus)) {
               cout << deadPid << " stopped by signal " << WSTOPSIG(wstatus) << endl;
           } else if (WIFCONTINUED(wstatus)) {
               cout << deadPid << " continue to wait for child status change.... " << endl;
           }
       } while (deadChildCount != childCount);

    cout << "Parent is going to exit... bye!!!!!!!!!" << endl;
}

// Task for forked processes
void performTask(const void *interval) {
    pid_t pid = getpid();
    int count = 0;
    while(count < *(int*) interval) {
        cout << "I will keep spinning here until I die. My pid is: " << pid << endl;
        sleep(10);
        ++count;
    }
    cout << "Bye, I am going to exit. I am child pid: " << pid << endl;
}
