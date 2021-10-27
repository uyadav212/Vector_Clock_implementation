/* Program:   Vector logical clock
*
*			Write a program in C++ / Java to implement Vector logical clocks. Your program should take as input a description of 
*			several process schedules (i.e., lists of send, receive or print operations. The output of your program will be a 
*			linearization of these events in the order actually performed, annotated with Lamport clock values.
*
* Assumption:
*		a) Once process is ended it will not begin again.
*		b) No two process Id can be same
*		c) All operations of a process must be between begin & end command
*		d) Format to be followed by input commands
*			1. begin process <processID>
*			2. end process <processID>
*			3. print <dataToPrint>
*			4. recv <processID> <msg>
*			5. send <processID> <msg>
*		NOTE: All commands are CASE-SENSITIVE.
*/

#include <bits/stdc++.h>
#include "modelUsed.cpp"
using namespace std;

void readInputFromFile() {

	ifstream inputFile ("eventSeqInput.txt");		//read input from this file
	int lineNumber = 0;
	if (inputFile.is_open()) {

		bool definingProcess = false;				//to keep track that we are in between being and end cmd (defining the events for process)
		vector<Events*> *eventSequencrForPID;
		string pId, line;

		while(getline(inputFile,line)) {

			lineNumber++;
			if ((strcmp(line.c_str(), " ") == 0)					//in order to ignore any empty line read from input file
				|| (strcmp(line.c_str(), "\t") == 0)
				|| (strcmp(line.c_str(), "\n") == 0)
				|| (strcmp(line.c_str(), "") == 0))
				continue;

			Events *lineToEvent = new Events();

			//this to ensure all operations/events for process are in between begin and end command
			if (!definingProcess) {

				lineToEvent->readEventCmd(line, lineNumber, " ");
				if (strcmp(lineToEvent->operation.c_str(), "begin") != 0) {
					cout << ": ERROR...\n";
					cout << ": Event command {line " << lineNumber << ": " << line << "} do not belong between process begin & end command.\n";
					exit(0);
				}

				vector<string>::iterator foundAt = find(allProcessId.begin(), allProcessId.end(), lineToEvent->onProcessId);
				if (foundAt != allProcessId.end()) {
					cout << ":\n: ERROR...\n";
					cout << ": Process " << lineToEvent->onProcessId << " already exist, duplicate processId {line " << lineNumber << ": " << line << "}\n";
					exit(0);
				}
				allProcessId.push_back(lineToEvent->onProcessId);
				pId = lineToEvent->onProcessId;
				definingProcess = true;
				eventSequencrForPID = new vector<Events*>();
				eventSequencrForPID->push_back(lineToEvent);

			} else {
				lineToEvent->readEventCmd(line, lineNumber, pId);

				if (strcmp(lineToEvent->operation.c_str(), "begin") == 0) {
					cout << ":\n: ERROR...\n";
					cout << ": End process event command missing for process " << pId << "\n";
					exit(0);
				}
				eventSequencrForPID->push_back(lineToEvent);

				if (strcmp(lineToEvent->operation.c_str(), "end") == 0) {
					eventSequence.push_back(pair<string, vector<Events*>* >{pId, eventSequencrForPID});
					definingProcess = false;
				}
			}
		}
		if (definingProcess) {
			cout << ": ERROR...\n";
			cout << ": End process event missing for process " << pId << " [at the end of input file]\n";
			exit(0);
		}

		inputFile.close();
	}
}

void unblockAndCheckDeadLock() {

	vector<Process*>::iterator activeProcessIt;
	vector<string>::iterator processIt;
	bool isDeadlock = true;

	for (processIt = allProcessId.begin(); processIt < allProcessId.end(); processIt++) {

		vector<string>::iterator isCompletedProcess = find(completedProcessId.begin(), completedProcessId.end(), *processIt);
		
		//here we will unblock all the processes whose msg is present in the channel pool and is in blocked state
		if (isCompletedProcess == completedProcessId.end()) {

			Process *processPointer = findProcessForPID(*processIt);
			if (processPointer != NULL && processPointer->getBlocked()) {

				//get the event to be performed (as it is in blocked state it will be RECV)
				vector<Events*> *pIdEventSeq = findEventListForPID(*processIt);
				vector<Message*>::iterator foundAt = findMsgInPool((*(pIdEventSeq->begin()))->arg2, (*(pIdEventSeq->begin()))->arg1, (*(pIdEventSeq->begin()))->onProcessId);
				if (foundAt != channelMsgPool.end()) {
					processPointer->setBlocked(false);
					isDeadlock = false;
				}
			}
		}
	}

	//deadlock when processes are in blocked state and desired message are not present on channel 
	//(means waiting for other process to send message)
	if (isDeadlock) {
		cout << ":\n:         *** DEADLOCK encountered ***\n";
		cout << ": Cannot proceed further. [Blocked process:";

		vector<Process*>::iterator activeProcessIt;
		for (activeProcessIt = activeProcess.begin(); activeProcessIt < activeProcess.end(); activeProcessIt++) {
			if ((*activeProcessIt)->getBlocked())
				cout << " " << (*activeProcessIt)->getProcessId();
		}
		cout << " ]" << endl;
		exit(0);
	}
}

bool isCausallyBeforeAllOther(Process *process) {

	bool isCausallyLess = false;
	vector<Process*>::iterator activeProcessIt;

	for (activeProcessIt = activeProcess.begin(); activeProcessIt < activeProcess.end(); activeProcessIt++) {
		if ((*activeProcessIt) != process) {
			//compare timestamps
			for (int i = 0; i < activeProcess.size(); i++)
				if (process->getClock(i) <= (*activeProcessIt)->getClock(i))
					isCausallyLess = true;
		}
	}
	return isCausallyLess;
}

string chooseProcessToExec() {
	vector<string>::iterator processIt;
	for (processIt = allProcessId.begin(); processIt < allProcessId.end(); processIt++) {
		vector<string>::iterator isCompletedProcess = find(completedProcessId.begin(), completedProcessId.end(), *processIt);

		if (isCompletedProcess == completedProcessId.end()) {
			//Compare this [pi] process's vector clock with all other [pj] process's clocks if timestamp Ti is less then all Tj 
			//proceed with executing even of that process
			Process *processPointer = findProcessForPID(*processIt);
			if (processPointer == NULL){
				return *processIt;
			}

			if (!(processPointer->getBlocked()) 
				&& !(processPointer->getEnded()) 
				&& isCausallyBeforeAllOther(processPointer)){
				return *processIt;
			}
		}
	}

	//found no process which is not blocked and can be executed, now will check for deadlock and unblock processes (if any)
	//whose recv message is present in the channel message pool
	unblockAndCheckDeadLock();

	return chooseProcessToExec();
}

void runProcessEvent() {

	string executingPID = chooseProcessToExec();
	vector<Events*> *pIdEventSeq = findEventListForPID(executingPID);
	Process *processPointer = findProcessForPID(executingPID);

	if ((processPointer == NULL && strcmp((*(pIdEventSeq->begin()))->operation.c_str(), "begin") == 0)
		|| (processPointer != NULL && !processPointer->getEnded())) {

		bool executedSuccessfully = (*(pIdEventSeq->begin()))->runEventCmd(processPointer);
		if (executedSuccessfully)
			pIdEventSeq->erase(pIdEventSeq->begin());
	}
}

int main() {

	cout << ":\n:	  *** Vector Clock Simulation ***\n";
	cout << ": Reading Process event sequence from input file \"eventSeqInput.txt\" \n:\n";
	readInputFromFile();

	while (completedProcessId.size() != allProcessId.size())
		runProcessEvent();

	return 0;
}