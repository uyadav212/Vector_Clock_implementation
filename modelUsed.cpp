/* Program:   Vector logical clock
*
*			Here we are defining all the funcions of all classes which are used to implement Vector logical clock.
*/

#include <bits/stdc++.h>
#include "modelUsed.h"
using namespace std;

vector<Events*>* findEventListForPID(string pId) {
	vector<pair<string, vector<Events*>* > >::iterator eventListIt;

	for (eventListIt = eventSequence.begin(); eventListIt < eventSequence.end(); eventListIt++) {
		if (strcmp(eventListIt->first.c_str(), pId.c_str()) == 0)
			return eventListIt->second;
	}
	//should not come here, as for every process events sequence exist
}

Process* findProcessForPID(string pId) {
	vector<Process*>::iterator activeProcessIt;

	for (activeProcessIt = activeProcess.begin(); activeProcessIt < activeProcess.end(); activeProcessIt++) {
		if (strcmp((*activeProcessIt)->getProcessId().c_str(), pId.c_str()) == 0)
			return (*activeProcessIt);
	}
	return NULL;	//if process not present in the active list
}

int generateUniqueId() {
	static int uniqueNum = 0;
	return (uniqueNum++);
}

vector<Message*>::iterator findMsgInPool(string msg, string from, string to) {

	//loop and check if desired msg is in channel pool means in transit
	vector<Message*>::iterator it;
	for (it = channelMsgPool.begin(); it < channelMsgPool.end(); it++) {
		if ((0 == strcmp(((*it)->fromProcess).c_str(), from.c_str()))
			&&(0 == strcmp(((*it)->toProcess).c_str(), to.c_str()))
			&&(0 == strcmp(((*it)->msgContent).c_str(), msg.c_str())))
		{
			return it;
		}
	}
	return it;
}

Events::Events() {
	operation = " ";
	arg1 = " ";
	arg2 = " ";
	onProcessId	= " ";
}

//returns the first word from the line
string getWord(string *line) {

	string tmp = " ";
	if ((strcmp(line->c_str(), "") != 0)){
		tmp = line->substr(0, line->find(' '));
	
		if (tmp.length() < line->length())
			*line = line->substr(tmp.length() + 1);
		else
			*line = line->substr(tmp.length());
	}
	return tmp;
}

//read the input event command and extract arguments
void Events::readEventCmd(string orgLine, int lineNum, string pId) {
	string line = orgLine;
	operation = getWord(&line);
	
	if (strcmp(operation.c_str(), "begin") == 0) {
		arg1 = getWord(&line);
		arg2 = getWord(&line);

		if ((strcmp(arg1.c_str(), "process") != 0) || (strcmp(arg2.c_str(), " ") == 0) || (strcmp(line.c_str(), "") != 0)) {
			cout << ": ERROR...\n";
			cout << ": Format for BEGIN operation is not correct {line " << lineNum << ": " << orgLine << "}\n";
			cout << ": Please follow format => begin process <processID>\n";
			exit(0);
		}
		onProcessId = arg2;

	} else if (strcmp(operation.c_str(), "end") == 0) {
		arg1 = getWord(&line);
		arg2 = getWord(&line);

		if ((strcmp(arg1.c_str(), "process") != 0) || (strcmp(arg2.c_str(), " ") == 0) || (strcmp(line.c_str(), "") != 0)) {
			cout << ": ERROR...\n";
			cout << ": Format for END operation is not correct {line " << lineNum << ": " << orgLine << "}\n";
			cout << ": Please follow format => end process <processID>\n";
			exit(0);
		}
		
		onProcessId = pId;
		if (strcmp(arg2.c_str(), onProcessId.c_str()) != 0) {
			cout << ": ERROR...\n";
			cout << ": Expecting END operation for process " << onProcessId << " but received for " << arg2 << " {line " << lineNum << ": " << orgLine << "}\n";
			exit(0);
		}

	} else if (strcmp(operation.c_str(), "print") == 0) {
		arg1 = line;		//everthing after print is treated as msg
		
		if (strcmp(arg1.c_str(), "") == 0) {
			cout << ": ERROR...\n";
			cout << ": Format for PRINT operation is not correct, nothing to print {line " << lineNum << ": " << orgLine << "}\n";
			cout << ": Please follow format => print <dataToPrint>\n";
			exit(0);
		}
		onProcessId = pId;

	} else if (strcmp(operation.c_str(), "recv") == 0) {
		arg1 = getWord(&line);
		arg2 = line;		//after process id everthing is considered as msg
		
		if ((strcmp(arg1.c_str(), " ") == 0) || (strcmp(arg2.c_str(), "") == 0)) {
			cout << ": ERROR...\n";
			cout << ": Format for RECEIVE operation is not correct {line " << lineNum << ": " << orgLine << "}\n";
			cout << ": Please follow format => recv <processID> <msg>\n";
			exit(0);
		}
		onProcessId = pId;
		
	} else if (strcmp(operation.c_str(), "send") == 0) {
		arg1 = getWord(&line);
		arg2 = line;		//after process id everthing is considered as msg
		
		if ((strcmp(arg1.c_str(), " ") == 0) || (strcmp(arg2.c_str(), "") == 0)) {
			cout << ": ERROR...\n";
			cout << ": Format for SEND operation is not correct {line " << lineNum << ": " << orgLine << "}\n";
			cout << ": Please follow format => send <processID> <msg>\n";
			exit(0);
		}	
		onProcessId = pId;
	
	} else {
		cout << ": ERROR...\n";
		cout << ": Unrecognised event/operation {line " << lineNum << ": " << orgLine << "}\n";
		exit(0);
	}
}

bool Events::runEventCmd(Process *processPointer) {

	if (strcmp(operation.c_str(), "begin") == 0) {
		Process newProcess;
		newProcess.beginProcessEvent(onProcessId);
		return true;
	}

	if (strcmp(operation.c_str(), "end") == 0) {
		if (processPointer == NULL) {
			cout << ": ERROR...\n";
			cout << ": Process " << onProcessId << " Not found in active processes (unable to execute END event)";
			exit(0);
		} else
			processPointer->endProcessEvent();
			return true;
	} 

	if (strcmp(operation.c_str(), "print") == 0) {
		if (processPointer == NULL) {
			cout << ": ERROR...\n";
			cout << ": Process " << onProcessId << " Not found in active processes (unable to execute PRINT event)";
			exit(0);
		} else
			processPointer->printMsgEvent(arg1);
			return true;
	}

	if (strcmp(operation.c_str(), "recv") == 0) {
		if (processPointer == NULL) {
			cout << ": ERROR...\n";
			cout << ": Process " << onProcessId << " Not found in active processes (unable to execute RECEIVE event)";
			exit(0);

		} else {
			//check if process exist from which we are receiving message
			vector<string>::iterator foundAt = find(allProcessId.begin(), allProcessId.end(), arg1);
			
			if (foundAt == allProcessId.end()) {
				cout << ": ERROR...\n";
				cout << ": Process " << arg1 << " from which we expect message doesn't exist. [" << operation
					<< " " << arg1 << " " << arg2 << "] \n";
				exit(0);
			}
			return processPointer->receiveMsgEvent(arg1, arg2);
		}
	}

	if (strcmp(operation.c_str(), "send") == 0) {
		if (processPointer == NULL) {
			cout << ": ERROR...\n";
			cout << ": Process " << onProcessId << " Not found in active processes (unable to execute SEND event)";
			exit(0);

		} else {
			//check if process exist to which we are sending message
			vector<string>::iterator foundAt = find(allProcessId.begin(), allProcessId.end(), arg1);

			if (foundAt == allProcessId.end()) {
				cout << ": ERROR...\n";
				cout << ": Process " << arg1 << " to which message is sent doesn't exist. [" << operation
					<< " " << arg1 << " " << arg2 << "] \n";
				exit(0);
			}
			processPointer->sendMsgEvent(arg1, arg2);
			return true;
		}
	}
}

void Process::updateClock(vector<int> *timestamp) {

	vector<string>::iterator foundAt = find(allProcessId.begin(), allProcessId.end(), processId);
	*(clock->begin() + (foundAt - allProcessId.begin())) += 1;

	//to cover the scenario when we receive msg with timestamp
	//			clock[i] = max(clock[i], timestamp[i])
	for (int i = 0; i < allProcessId.size(); i++) {
		if (*(timestamp->begin() + i) > *(clock->begin() + i))
			*(clock->begin() + i) = *(timestamp->begin() + i);
	}
}

void Process::updateClock() {
	vector<string>::iterator foundAt = find(allProcessId.begin(), allProcessId.end(), processId);
	*(clock->begin() + (foundAt - allProcessId.begin())) += 1;
}

Process::Process() {
	vector<int> *c = new vector<int> (allProcessId.size(), 0);
	clock = c;
	c = NULL;
	processId = "P" + to_string(generateUniqueId());
	blocked = false;
	ended = false;
};

void Process::sendMsgEvent(string to, string msg) {

	updateClock();

	//add msg to the message pool on the channel [means are in transit]
	Message *msgToSend = new Message();

	msgToSend->fromProcess = processId;
	msgToSend->toProcess = to;
	msgToSend->msgContent = msg;
	msgToSend->timestamp = clock;

	cout << ": " << msg << "\t [Sent by process " << processId << " to process " << to << " at timestamp: {";
	printTimestamp();
	cout << " }]\n";
	channelMsgPool.push_back(msgToSend);

	//if destination process is already blocked due to this msg, unblock it
	Process *processPointer = findProcessForPID(to);
	if (processPointer != NULL && processPointer->getBlocked()) {
		vector<Events*> *pIdEventSeq = findEventListForPID(to);
		if (strcmp((*(pIdEventSeq->begin()))->operation.c_str(), "recv") == 0
			&& strcmp((*(pIdEventSeq->begin()))->arg1.c_str(), processId.c_str()) == 0
			&& strcmp((*(pIdEventSeq->begin()))->arg2.c_str(), msg.c_str()) == 0) {
			processPointer->setBlocked(false);
		}
	}
}

void Process::printMsgEvent(string msg) {

	updateClock();

	cout << ": " << msg << "\t [Printed by process " << processId << " at timestamp: {";
	printTimestamp();
	cout << " }]\n";
}

//returns 	true: 	if msg received
//			false:	if msg not received and process is blocked
bool Process::receiveMsgEvent(string from, string msg) {

	//find in the channel message pool
	vector<Message*>::iterator foundAt = findMsgInPool(msg, from, processId);

	if (foundAt == channelMsgPool.end()) {
		blocked = true;
	} else {
		Message *recMsg = *foundAt;
		updateClock(recMsg->timestamp);

		//print msg and remove from pool
		cout << ": " << recMsg->msgContent << "\t [Received by process " << processId << " from process " 
			<< recMsg->fromProcess << " at timestamp: {";
		printTimestamp();
		cout << " }]\n";

		channelMsgPool.erase(foundAt);
		delete recMsg;
	}
	return (!blocked);
}

void Process::beginProcessEvent(string id) {
	Process *p = new Process();
	p->processId = id;

	activeProcess.push_back(p);
}

void Process::endProcessEvent() {
	ended = true;
	completedProcessId.push_back(processId);
}

//getter and setter functions
bool Process::getBlocked() {
	return blocked;
}

string Process::getProcessId() {
	return processId;
}

bool Process::getEnded() {
	return ended;
}

int Process::getClock(int index) {
	return (*(clock->begin() + index));
}

void Process::setBlocked(bool flag) {
	blocked = flag;
}

void Process::printTimestamp() {
	for (int i = 0; i < clock->size(); i++)
		cout << " " << getClock(i);
}