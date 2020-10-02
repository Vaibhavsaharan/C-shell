#include <signal.h>
#include <sys/types.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <experimental/filesystem>
#include <fstream>
#include <cstdlib>
#include <sys/wait.h>

using namespace std;
extern char **environ;
vector<string> Lookuptable = {"cd","clr","dir","environ","echo","pause","help","quit","history"};

vector<string> parse(string &line){
	vector<string> tokens;
	string word;
	stringstream ss(line);
	while(ss >> word)
		tokens.push_back(word);
	return tokens;
}

int whichSR(string cmd){
	for(int i=0;i<Lookuptable.size();i++){
		if(cmd == Lookuptable[i])
			return i;
	}
	return -1;
}

int cmdecho(vector<string> args){
	for(int i=1;i<args.size();i++)
		cout<<args[i]<<" ";
	cout<<endl;
	return 1;
}

int cmdcd(vector<string> args){
	int status =1;
	if(args.size() == 1){
		cout<<get_current_dir_name()<<endl;
	}

	else{
		if(args[1][0] != '/'){
			string cwd = get_current_dir_name();
			int rc = chdir((cwd + "/" + args[1]).c_str());
			if(rc < 0){
				cout <<"cd : No such file or directory" <<endl;
			}
		}
		else if(args[1] == ".."){
			string cwd = get_current_dir_name();
			int n = cwd.size();
			while(*cwd.cbegin() != '/'){
				cwd.erase(cwd.cbegin());
			}
			chdir((cwd).c_str());
		}
		else{
			int rc = chdir(args[1].c_str());
			if(rc < 0){
				cout <<"cd : No such file or directory" <<endl;
			}
		}
	}

	return status;
}

int cmdclr(){
	system("clear");
	return 1;
}
int cmddir(vector<string> args){
	string path = get_current_dir_name();
	for (const auto & entry : experimental::filesystem::directory_iterator(path))
        cout << entry.path() << endl;
	return 1;
}
int cmdenviron(vector<string> args){
	vector<string> envs = {"SHELL","SESSION_MANAGER","QT_ACCESSIBILITY","COLORTERM","XDG_CONFIG_DIRS","XDG_MENU_PREFIX","CONDA_EXE","LANGUAGE","GNOME_SHELL_SESSION_MODE","SSH_AUTH_SOCK","SSH_AGENT_PID","GTK_MODULES","PWD","LOGNAME","GPG_AGENT_INFO","XAUTHORITY","HOME","USERNAME","LANG","VTE_VERSION","GNOME_TERMINAL_SCREEN","INVOCATION_ID","MANAGERPID","GJS_DEBUG_OUTPUT","LESSCLOSE","XDG_SESSION_CLASS","USER","DISPLAY","SHLVL","CONDA_PYTHON_EXE","XDG_DATA_DIRS","PATH","GDMSESSION","DBUS_SESSION_BUS_ADDRESS","OLDPWD"};
	for(int i=0;i<envs.size();i++){
		cout<<envs[i]<<" : "<<getenv(envs[i].c_str())<<endl;
	}
	return 1;
}
int cmdpause(vector<string> args){
	cout<<"Paused... Press Enter key to resume shell"<<endl;
	while(1){
		if(cin.get() == '\n')
			break;
	}
	return 1;
}
int cmdhelp(vector<string> args){
	cout<<"\n***WELCOME TO MY SHELL HELP***"
        "\nList of Commands supported:"
        "\n>cd"
        "\n>clr"
        "\n>dir"
        "\n>environ"
        "\n>echo"
        "\n>pause"
        "\n>help"
        "\n>quit"
        "\n>history"
        "\n>improper space handling\n\n";
	return 1;
}
int cmdhistory(vector<string> args){
	string line;
	fstream cmdhistry;
	cmdhistry.open("history.txt",fstream::in);
	int i=0,n;
	if(args.size()==1)
		n=5;
	else n = (-1)*(stoi(args[1]));
	vector<string> buffer(n," ");
	while(getline(cmdhistry, line)){
		buffer[i] = line;
		if(++i >= n)
			i=0;
	}
	for(int j=0;j<n;++j){
		if(buffer[i]!=" ")
			cout<<buffer[i]<<endl;
		if(++i >= n)
			i=0;
	}
	return 1;
}

int executecmd(int cmdNo, vector<string> args){
	int status;
	cmdNo++;
	switch(cmdNo){
		case(1):
			status = cmdcd(args);
			break;
		case(2):
			status = cmdclr();
			break;
		case(3):
			status = cmddir(args);
			break;
		case(4):
			status = cmdenviron(args);
			break;
		case(5):
			status = cmdecho(args);
			break;
		case(6):
			status = cmdpause(args);
			break;
		case(7):
			status = cmdhelp(args);
			break;
		case(9):
			status = cmdhistory(args);
			break;

	}
	return status;
}

int executecmd1(vector<string> args1){
	pid_t pid;
    int status;
    int size = args1.size();
    
    vector<char*>args;
    for(int i=0;i<size;i++){
    	char *tmp = &(args1[i])[0];
    	args.push_back(tmp);
    }
    char * prog = args[0];

    char** argv = new char*[args.size()+1];

    for( int k = 0; k < args.size(); k++ ){
		argv[k] = args[k];
	}
	argv[args.size()] = NULL;

     
	if ((pid = fork()) < 0) {
		cout<<"*** ERROR: forking child process failed\n";
		status = 0;
		return status;
	}
	else if (pid == 0) {
 		if (execvp(prog, argv)< 0) {
			cout<<endl<<"Command ' ";
			for(int i=0;i<size;i++)
				cout<<args1[i]<<" ";
			cout<<"' not found. Please press help to list available commands"<<endl<<endl;
 			exit(0);
		}
	}
	else {
		while (wait(&status) != pid);
     }
	return 1;
}

void startup(){
	
}


int main(int argc, char * argv[]){
	int status=1;
	string line;
	vector<string> args;
	cmdclr();
	fstream cmdhistry;
	cout<<"myshell verion 1.0.0 copyright @Vaibhav Saharan"<<endl;
	startup();
	cmdhistry.open("history.txt",fstream::out | fstream::app);
	if(!cmdhistry){
		cmdhistry.open("history.txt",  fstream::out | fstream::trunc);
	}
	if(argc == 1){
		do{
			int check = 0,check1=0;
			cout<<"\033[1;36m"<<getenv("USER")<<"~AT~"<<get_current_dir_name()<<"\033[0m"<<"\033[1;35;33m%\033[0m ";
			getline(cin,line);
			args = parse(line);
			if(args[0]!="history")
				cmdhistry<<line<<endl;
			if(line == "exit" || line == "quit")
				break;
			if(line =="")
				continue;
			int cmdNo = whichSR(args[0]);
			if(cmdNo != -1)
				check1 = executecmd(cmdNo,args);
			else{
				 check = executecmd1(args);
			}
			if (check == 0 && check1 == 0){
				cout<<endl<<"Command '" <<line<<"' not found. Please press help to list available commands"<<endl<<endl;
			}
			signal(SIGINT, SIG_IGN);

		}while(status);
		cmdhistry.close();
	}
	else if(argc == 2){
		fstream fileinput;
		fileinput.open(argv[1],fstream::in);
		if(!fileinput)
			cout<<endl<<"myshell : Could not run the provided script "<<endl<<endl;
		else{
			while(getline(fileinput,line)){
				int check = 0,check1=0;
				args = parse(line);
				if(args[0]!="history")
					cmdhistry<<line<<endl;
				if(line == "exit" || line == "quit")
					break;
				if(line =="")
					continue;
				int cmdNo = whichSR(args[0]);
				if(cmdNo != -1)
					check1 = executecmd(cmdNo,args);
				else{
					 check = executecmd1(args);
				}
				if (check == 0 && check1 == 0){
					cout<<endl<<"Command '" <<line<<"' not found. Please press help to list available commands"<<endl<<endl;
				}
			}
			cmdhistry.close();
		}
	}
	else{
		cout<<endl<<"myshell : Only accpets one command line argument as filename"<<endl<<endl;
	}
	
	
}