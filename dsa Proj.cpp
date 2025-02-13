//Data Structures 
//Semester Project
//Zara Zaman (231248)
//Noor ul Huda (231140)

#include <iostream>    
#include <string>
#include<cstdlib>   //used for system("cls")
using namespace std;


//FOR MESSAGES
struct MessageNode {
    string sender;
    string text;
    MessageNode* next;
    MessageNode(string s, string t) : sender(s), text(t), next(nullptr) {}
};

class ChatRoom {
private:
    MessageNode* front; // Front ptr of the message queue
    MessageNode* rear;  // Rear ptr of the message queue  (these two are for showing chat history)
    MessageNode* top;   // Top of the message stack (for undo functionality)
    string groupName;

public:
    ChatRoom(string name) : front(nullptr), rear(nullptr), top(nullptr), groupName(name) {}

    //sender and text are passed by ref. and kept const so sender or text change na hojaye
    void sendMessage(const string& sender, const string& text)
    {
        MessageNode* newMessage = new MessageNode(sender, text);

        // Add to queue
        if (rear == nullptr)
        {
            front = rear = newMessage;
        }

        else
        {
            rear->next = newMessage;
            rear = newMessage;
        }

        //the new msg is also pushed to the top of the stack
        MessageNode* addToStack = new MessageNode(sender, text);
        addToStack->next = top;
        top = addToStack;
        cout << sender << ": " << text << endl;
    }

    //for showing history, we use queue its FIFO
    //this func is const so the no msg content is altered since we're only reading the msgs
    //it doesnt allow us to alter the msg history
    void showHistory() const
    {
        if (front == nullptr) {
            cout << "No messages in the chat history" << endl;
            return;
        }

        cout << "\nChat history:" << endl;

        MessageNode* temp = front;
        //temp equals front since we're showing msgs from the front and traverse towards rear (first message sent gets displayed first)
        while (temp)
        {
            cout << temp->sender << ": " << temp->text << endl;
            temp = temp->next;

        }
    }

    //implementing stack for this since it is LIFO
    //this also updates the queue
    void undoLastMessage()
    {
        if (top == nullptr)
        {
            cout << "No messages to undo" << endl;
            return;
        }

        // Remove top element from  stack
        MessageNode* lastMessageStack = top;
        top = top->next;
        delete lastMessageStack;

        //deleting from queue (in case theres no msg in the queue)
        if (front == nullptr)
        {
            return;
        }

        //(in case there only one msg in the queue)
        if (front == rear)
        {
            delete front;
            front = rear = nullptr;
        }

        //(in case theres more than one msg stored i.e multiple nodes in queue)
        else
        {
            MessageNode* temp = front;
            //the temp ptr traverses til the second last node since last wala ko wesay bhi undo karna hai is liye we dont traverse till the end
            while (temp->next != rear)
            {
                temp = temp->next;
            }

            delete rear;
            //now temp becomes the last node as rear is deleted
            temp->next = nullptr;
            //temp now equals rear (temp->next is null)
            rear = temp;
        }

        cout << "~Last message undone." << endl << endl;
    }

    //original groupname isnt to be altered unless altered by the admin thats why it is passed as ref. and ket constant
    void changeGroupName(const string& newName)
    {
        groupName = newName;
        cout << "~Group name changed to: " << groupName << endl << endl;
    }

    string getGroupName() const
    {
        return groupName;
    }


    ~ChatRoom()
    {
        while (front != nullptr)
        {
            //for undoing memory is being free-d (del last node) QUEUE
            MessageNode* temp = front;
            front = front->next;
            delete temp;
        }
        while (top != nullptr)
        {
            //popping top msg from STACK (del top)
            MessageNode* temp = top;
            top = top->next;
            delete temp;
        }
    }
};

//FOR USERS IN THE CHATROOM (singly linked list is used to store the users)
struct UserNode {
    string name;
    bool isAdmin;
    UserNode* next;
    UserNode(string n, bool admin) : name(n), isAdmin(admin), next(nullptr) {}
};

//functionalities performaed by the admin only
class Admin {
private:
    UserNode* head;         // Head of the user linked list
    string adminPassword;
    ChatRoom& chatRoom;      //ref to chatroom class object

public:
    Admin(ChatRoom& cr, const string& initialPassword) : chatRoom(cr), adminPassword(initialPassword), head(nullptr)
    {
        addUser("xyz", true);   // Initial admin (the users creating the chatroom is the default admin)
    }

    //admin enters the valid password to access group settings (const and by ref.)
    bool verifyPassword(const string& password) const
    {
        return password == adminPassword;
    }

    //allows admin to change password
    void changePassword(const string& newPassword)
    {
        adminPassword = newPassword;
        cout << "~Admin password changed successfully." << endl;
    }

    //the rest users being added by the admin are not admins hence bool is kept false
    void addUser(const string& name, bool isAdmin = false)
    {
        UserNode* newUser = new UserNode(name, isAdmin);

        //if head empty then no user currently in the chatroom hence only admin is there tou isADmin will be true only for him
        if (head == nullptr)
        {
            head = newUser;
            if (isAdmin)
            {
                cout << "\t\t\t\t" << name << " created a chatroom" << endl << endl;
            }
        }

        //insertion (at end)
        else
        {
            UserNode* temp = head;
            while (temp->next != nullptr)
            {
                temp = temp->next;
            }
            temp->next = newUser;
            cout << name << " was added to the chat room " << chatRoom.getGroupName() << endl;
        }
    }

    void removeUser(const string& name)
    {
        if (head == nullptr)
        {
            cout << "No users in the chat room." << endl;
            return;
        }

        //cant remove head user since its the admin
        if (head->name == name)
        {
            cout << "~~WARNING! Admin cant be removed unless another admin assigned." << endl;
            return;
        }

        //traversing the whole list to find thename of the user whom admin wants to remove
        UserNode* current = head;
        while (current->next != nullptr && current->next->name != name)
        {
            current = current->next;
        }

        //if user found tou delete that (from middle or end)
        if (current->next != nullptr)
        {
            UserNode* temp = current->next;
            current->next = current->next->next;
            delete temp;
            cout << name << " was removed.\n";
        }

        ///in case user not found
        else
        {
            cout << name << " not found in the chat room.\n";
        }
    }

    //searching a specific user by name and then alllows to send message
    bool isUserInChat(const string& name) const
    {
        UserNode* temp = head;
        while (temp != nullptr)
        {
            if (temp->name == name)
            {
                return true;
            }
            temp = temp->next;
        }
        return false;
    }

    string getCurrentAdminName() const
    {
        if (head != nullptr && head->isAdmin)
        {
            return head->name; // head pointinf to the current admin
        }
        return "";
    }


    string assignNewAdmin()
    {
        if (head != nullptr && head->next != nullptr)
        {
            //the prev admin(head) reassigned to the very next user added in the chatroom (head->next)
            head->next->isAdmin = true;
            UserNode* temp = head->next->next;
            while (temp)
            {
                temp->isAdmin = false;
                temp = temp->next;
            }
            return head->next->name;
        }
        return "";
    }

    //thisfucntionality is of the chatroom class hence that classroom's object is used to access that class' function
    void changeGroupName(const string& newName)
    {
        chatRoom.changeGroupName(newName);
    }

    //deletes the whole chatroom
    void deleteChatRoom()
    {
        while (head != nullptr)
        {
            UserNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    //when admin wants to leave chatroom
    //make someone else the admin
    //after the prev admin has left, he can no longer access group settings, only choice is to exit to main menu then the new admin can access the group settings
    void leaveGroupChat(const string& currentAdmin)
    {
        if (head == nullptr || head->name != currentAdmin)
        {
            cout << "Admin not found." << endl;
            return;
        }

        //if admin is the only one added then when he leaves the chatroom is no longer available
        if (head->next == nullptr)
        {
            deleteChatRoom();
            cout << "ChatRoom deleted!" << endl;
            exit(1);
        }

        //in case other members are added then another one is made new admin
       // UserNode* newAdmin = head->next;
        //newAdmin->isAdmin = true;


        // Remove the old admin from the chat
        UserNode* oldAdmin = head;
        head = head->next;
        head->isAdmin = true;
        delete oldAdmin;  // Delete the old admin

        cout << currentAdmin << " left the chat" << endl;
        cout << head->name << " is the admin now" << endl;
    }

    //displays all the users present in the chatroom
    void displayUsers()
    {
        int countUsers = 1;
        cout << "Chat Room: " << "'" << chatRoom.getGroupName() << "'" << endl;
        cout << "\nParticipants:\n";
        UserNode* temp = head;
        cout << countUsers << ". " << temp->name << " (admin)" << endl;
        temp = temp->next;
        while (temp != nullptr)
        {
            countUsers++;
            cout << countUsers << ". ";
            cout << temp->name;
            cout << endl;
            temp = temp->next;
        }
    }
};

//function declarations
void generalChatMenu(Admin& admin, ChatRoom& chatRoom);
void adminMenu(Admin& admin, ChatRoom& chatRoom);
void mainMenu(Admin& admin, ChatRoom& chatRoom);
void generalChatMenu(Admin& admin, ChatRoom& chatRoom) {
    char option;
    cout << "========================================================================================================================" << endl;
    cout << "                                              ---- "<<chatRoom.getGroupName()<<" ----                                   " << endl;
    cout << "=========================================================================================================================" << endl;
    cout << "1. Send Message\n";
    cout << "2. Show Chat History\n";
    cout << "3.Undo Last Message\n";
    cout << "4.Exit General Chat\n";
    cout << "========================================================================================================================" << endl;

    do {
        //cout << "Enter your choice: ";
        cin >> option;
        cin.ignore();

        switch (option) {
        case '1':
        {

            string sender, content;
            cout << "Enter sender name: ";
            getline(cin, sender);
            if (admin.isUserInChat(sender))
            {
                cout << "Enter message: ";
                getline(cin, content);
                chatRoom.sendMessage(sender, content);
            }
            else
            {
                cout << "Sender is not a member of the chat room.\n";
            }
            break;
        }

        case '2':

            chatRoom.showHistory();
            break;


        case '3':

            chatRoom.undoLastMessage();
            break;

        case '4':
            cout << "Exiting General Chat.\n";
            system("cls");
            mainMenu(admin, chatRoom);
            break;

        default:
            break;
        }
        if (option != '1' && option != '2' && option != '3' && option != '4')
        {
            cout << "Invalid choice. Try again" << endl;

        }
    } while (option != '4');
}

//this function contains the settings of the chatroom only admin can access
void adminMenu(Admin& admin, ChatRoom& chatRoom) {
    bool isAdmin = true;
    char option;
    string confirmation; // This var will be used to confirm the old password before changing to the new one
    bool exitFlag = false;

    cout << "========================================================================================================================" << endl;
    cout << "\t\t\t\t\t\t---- ADMIN MENU ----\t\t\t\t" << endl;
    cout << "========================================================================================================================" << endl;
    cout << "1. Add Users" << endl;
    cout << "2. Remove Users" << endl;
    cout << "3. Display Group Chat" << endl;
    cout << "4. Rename Group Chat" << endl;
    cout << "5. Change Password" << endl;
    cout << "6. Leave Chat Room" << endl;
    cout << "7. Delete Chat Room" << endl;
    cout << "8. Exit to Main Menu" << endl;
    cout << "========================================================================================================================" << endl;

    do {
        // cout << "Enter your choice: ";
        cin >> option;
        cin.ignore();

        //validating the choice
        if (option < '1' || option > '8' || cin.fail())
        {
            cin.clear();
            cout << "Invalid choice. Try again" << endl;
            cin.ignore();
            continue;
        }

        // Accessing admin class functions using the admin class object
        switch (option) {
        case '1':
        {
            string name;
            cout << "Add username: ";
            getline(cin, name);
            admin.addUser(name);
            break;
        }
        case '2':
        {
            string name;
            cout << "Remove username: ";
            getline(cin, name);
            admin.removeUser(name);
            break;
        }

        case '3':
        {
            admin.displayUsers();
            break;
        }

        case '4':
        {
            string newName;
            cout << "New group name: ";
            getline(cin, newName);
            admin.changeGroupName(newName);
            break;
        }
        case '5':
        {
            string oldPassword, newPassword;
            while (true)
            {

                cout << "Current Password: ";
                getline(cin, oldPassword);

                if (admin.verifyPassword(oldPassword))
                {
                    cout << "New Password: ";
                    getline(cin, newPassword);
                    admin.changePassword(newPassword);
                    break;
                }
                else
                {
                    cout << "Incorrect password. Try again." << endl;
                }
            }
            break;
        }

        case '6':
        {
            admin.leaveGroupChat(admin.getCurrentAdminName()); // Automatically get the current admin's name
            cout << "Press any key to exit to the main menu" << endl;
            cin.get();                                              //prev admin presses any key, screen goes back to main menu
            system("cls");
            mainMenu(admin, chatRoom);
            break;
        }
        case '7':
        {
            admin.deleteChatRoom();
            cout << "Chat Room deleted" << endl;
            exit(0);
            break;
        }

        case '8':
        {
            exitFlag = true;
            system("cls");
            mainMenu(admin, chatRoom);
            break;
        }

        default:
            cout << "Invalid choice. Please try again.\n";
        }

    } while (!exitFlag);
}

void mainMenu(Admin& admin, ChatRoom& chatRoom) {
    char option;
    bool exitFlag = false;
    string password;

    do {
        cout << "========================================================================================================================" << endl;
        cout << "\t\t\t\t\t\t---- MAIN MENU ----\t\t\t\t" << endl;
        cout << "========================================================================================================================" << endl;
        cout << "1. Group Settings (only for Admins)\n";
        cout << "2. General Chat Menu\n";
        cout << "3. Exit\n";
        cout << "========================================================================================================================" << endl;

        //cout << "Enter your choice: ";
        cin >> option;
        cin.ignore();

        switch (option)
        {

        case '1':
        {
            cout << "Password: ";
            getline(cin, password);
            if (admin.verifyPassword(password))
            {
                system("cls");
                adminMenu(admin, chatRoom);
            }
            else
            {
                cout << "Incorrect password.\n";
            }
            break;
        }

        case '2':
            system("cls");
            generalChatMenu(admin, chatRoom);
            break;
        case '3':
            exitFlag = true;
            exit(0);
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }

    } while (!exitFlag);
}

int main() {
    ChatRoom chatRoom("ChatRoom");
    Admin admin(chatRoom, "123");

    mainMenu(admin, chatRoom);

    return 0;
}