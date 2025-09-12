#include "header.h"




void printWelcome() {
    cout << "********************************************" << endl;
    cout << "*                                          *" << endl;
    cout << "*           WELCOME TO MY SHELL            *" << endl;
    cout << "*                                          *" << endl;
    cout << "********************************************" << endl;
    cout << endl;
}
void printGoodbye() {
    cout << endl;
    cout << "********************************************" << endl;
    cout << "*                                          *" << endl;
    cout << "*            THANK YOU FOR USING           *" << endl;
    cout << "*               MY SHELL                   *" << endl;
    cout << "*                                          *" << endl;
    cout << "********************************************" << endl;
    cout << endl;
}

int main() {
    printWelcome();
    init_signals();
    load_history();
    shell_loop();
    //save_history(); 
     
    return 0;
}