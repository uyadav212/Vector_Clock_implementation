/* Program:   Vector logical clock
*
*			This is the header file which defines all classes and structures which are used to implement Vector logical clock 
*/

#include <bits/stdc++.h>
using namespace std;

struct Message {				//to hold message information which will be send on the channel
	string fromProcess;
	string toProcess;
	string msgContent;
	vector<int> *timestamp;				//timestamp from vector clock
};

class Process {					//to hold information of process
	vector<int> *clock;
	string processId;
	bool blocked;
	bool ended;

	void updateClock(vector<int> *timestamp);
	void updateClock();

public:
	Process();
	void printTimestamp();

	//getter and setter functions
	bool getBlocked();
	bool getEnded();
	int getClock(int index);
	string getProcessId();
	void setBlocked(bool flag);

	void sendMsgEvent(string to, string msg);
	void printMsgEvent(string msg);
	bool receiveMsgEvent(string from, string msg);
	void beginProcessEvent(string id);
	void endProcessEvent();
};

class Events {					//to hold events/operations which are to be performed by a process
public:
	string operation;
	string arg1;
	string arg2;
	string onProcessId;

	Events();
	void readEventCmd(string orgLine, int lineNum, string pId);
	bool runEventCmd(Process *processPointer);
};

int tmpGlobalClock;
vector<string> allProcessId;
vector<string> completedProcessId;
vector<Message*> channelMsgPool;
vector<Process*> activeProcess;
vector<pair<string, vector<Events*>* > > eventSequence;

vector<Events*>* findEventListForPID(string pId);
Process* findProcessForPID(string pId);