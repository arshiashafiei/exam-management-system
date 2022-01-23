#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>
#include <chrono>
using namespace std;

struct user
{
    string username;
    string password;
    string type;
};

struct exam_info
{
    int id;
    string teacher_username;
    time_t start_time;
    time_t end_time;
};

struct question_info
{
    string question;
    string option[4];
    string answer;
};

string repeater(char, int = 30);
void menu();
void login();
void signup(const string &, const string &);
void write_user_data(const user &);
void write_time_data(const exam_info &);
void write_question_data(const question_info &question, int);
user read_admin_file();
user *read_all_file(int &);
int get_file_size(const string &, char);
bool is_user_authenticate(const user &);
bool get_user_data(user &);
void admin_dashboard();
void teacher_dashboard(const user &);
bool compare_user(const user &, const user &);
void make_exam(const user &);
void show_exam(const user &);
void take_exam(const user &, const string &, const time_t &);
void student_dashboard(const user &);
void show_points(const string &);
void show_student_points(const user &, const string &);
bool already_taken_exam(const user &, const string &);

int main()
{
    ifstream db;
    db.open("userdb.dat");
    if (!db.is_open())
    {
        user admin;
        cout << "Please enter admin username: ";
        cin >> admin.username;
        cout << "Please enter admin password: ";
        cin >> admin.password;
        admin.type = "admin";
        write_user_data(admin);
        cout << repeater('\n', 60);
    }
    db.close();
    menu();
    return 0;
}

// Repeat a single character for n times (default n = 30) 
string repeater(char c, int n)
{
    string result;
    for (int i = 0; i < n; i++) {
        result += c;
    }
    return result; 
}

void menu()
{
    string command;
    cout << "(!" << repeater('-') << "MENU" << repeater('-') << "!)\n";
    cout << "(1)  Log In\n";
    cout << "(2)  Sign Up (For Students)\n";
    cout << "(3)  exit the program\n";
    cout << "Please enter a number from above: ";
    cin >> command;
    cout << repeater('\n', 60);
    if (command == "1") {
        login();
    }
    if (command == "2") {
        signup("student", "student");
    }
    if (command == "3") {
        // exit
        exit(0);
    } 
    else {
        // restart
        cout << "Please try again\n";
        menu();
    }
}

void login()
{
    user user_check;
    // get user data
    cout << "Please enter your username: ";
    cin >> user_check.username;
    cout << "Please enter your password: ";
    cin >> user_check.password;
    cout << repeater('\n', 60);
    // order is important
    bool password_correct = is_user_authenticate(user_check);
    bool username_exist = get_user_data(user_check);
    if (password_correct) {
        cout << "Successfully logged In!\n";
        // dashboard
        if (user_check.type == "admin") {
            admin_dashboard();
        }
        if (user_check.type == "teacher") {
            teacher_dashboard(user_check);
        }
        else {
            student_dashboard(user_check);
        }
    }
    else if (username_exist) {
        cout << "Wrong password! Try again\n";
    }
    else {
        cout << "This username don't exist! Try again\n";
    }
    login();
}

void signup(const string &who_add, const string &type)
{
    user user_signup;
    // get user data
    cout << "Please enter a username: ";
    cin >> user_signup.username;
    cout << "Please enter a password: ";
    cin >> user_signup.password;
    user_signup.type = type;
    cout << repeater('\n', 60);
    // Check if user already exist if yes it will not be created
    bool username_exist = get_user_data(user_signup);
    if (!username_exist) {     
        write_user_data(user_signup);
        
        if (who_add == "admin") {
            cout << "Successfully Registred!\n";
            admin_dashboard();
        }
        else {
            cout << "Successfully Registred! now you can log In\n";
            menu();
        }
    }
    else {
        if (who_add == "admin") {
            cout << "User already exist!\n";
            admin_dashboard();
        }
        else {
            cout << "User already exist! please log In\n";
            menu();
        }
    }
}

// Write user struct to userdb
void write_user_data(const user &user_write)
{
    fstream fpwrite;
    fpwrite.open ("userdb.dat", ios::out | ios::app);
    if (!fpwrite.is_open()) {
        cout << "cannot open file!\n";
    }
    fpwrite << user_write.username << ' ' << user_write.password << ' ' << user_write.type << '\n';
    fpwrite.close();
}

// write exam_info struct in exam_time.dat
void write_time_data(const exam_info &exam)
{
    fstream fpwrite;
    fpwrite.open ("exam_time.dat", ios::out | ios::app);
    if (!fpwrite.is_open()) {
        cout << "cannot open file!\n";
    }
    fpwrite << exam.id << ' ' << exam.teacher_username << ' ' << exam.start_time << ' ' << exam.end_time << '\n';
    fpwrite.close();
}

// write question_info in exam and answer file
void write_question_data(const question_info &question, int exam_id)
{
    string id = to_string(exam_id);
    fstream fpwrite_questions;
    fstream fpwrite_answers;
    string filename_questions = "exam" + id + ".dat";
    string filename_answers = "answer" + id + ".dat";
    fpwrite_questions.open (filename_questions, ios::out | ios::app);
    fpwrite_answers.open (filename_answers, ios::out | ios::app);
    if (!fpwrite_questions.is_open()) {
        cout << "cannot open file question!\n";
    }
    if (!fpwrite_answers.is_open()) {
        cout << "cannot open file answer!\n";
    }
    fpwrite_questions << question.question << '\n';
    for (int i = 0; i < 4; i++) {
        fpwrite_questions << i + 1 << ") " << question.option[i] << '\t';
    }
    fpwrite_questions << "\n\\";
    fpwrite_questions.close();
    fpwrite_answers << question.answer << ' ';
    fpwrite_answers.close();
}

// Read admin user struct from first line of userdb
user read_admin_file()
{
    struct user user_read;
    fstream fpread;
    fpread.open ("userdb.dat", ios::in);
    if (!fpread.is_open()) {
        cout << "cannot open file!\n";
    }
    fpread >> user_read.username >> user_read.password >> user_read.type;
    fpread.close();
    return user_read;
}

// read all user structs from a specific file and give array size as well
user *read_all_file(int &size)
{
    string line;
    size = 0;
    fstream fpread;
    fpread.open ("userdb.dat", ios::in);
    if (!fpread.is_open()) {
        cout << "cannot open file!\n";
    }
    // get how many records are available in file
    while (getline(fpread, line)) {
        size++;
    }
    fpread.close();
    fpread.open ("userdb.dat", ios::in);
    user *user_read = new user[size];
    
    for (int i = 0; i < size; i++) {
        fpread >> user_read[i].username >> user_read[i].password >> user_read[i].type;
    }
    fpread.close();
    return user_read;
}

// return the size of a file with seperator 0 if no file exist
int get_file_size(const string &filename, char seperator)
{
    string line;
    int size = 0;
    fstream fpread;
    fpread.open (filename, ios::in);
    if (!fpread.is_open()) {
        return 0;
    }
    // get how many records are available in file
    while (getline(fpread, line, seperator)) {
        size++;
    }
    fpread.close();
    return size;
}

// Return true if username and pass exist
// Return false otherwise
bool is_user_authenticate(const user &user_check)
{
    user user_get = user_check;
    // check for every user in database
    if(get_user_data(user_get)) {
        if (user_get.password == user_check.password) {
            return true;
        }
    }
    return false;
}

// Return true if username exist
// change the user passed to it and complete user info
// Return false if nothing exist
bool get_user_data(user &user_check)
{
    // read all users
    int size = 0;
    user* user_info = read_all_file(size);
    // check for every user in database
    for (int i = 0; i < size; i++) {
        if (user_check.username == user_info[i].username) {
            user_check = user_info[i];
            delete[] user_info;
            return true;
        }
    }
    delete[] user_info;
    return false;
}


void admin_dashboard()
{
    user admin = read_admin_file();
    string command;
    cout << "(!" << repeater('-') << " WELCOME " << admin.username << ' ' << repeater('-') << "!)\n";
    cout << "(1)  Add Teacher\n";
    cout << "(2)  Add Student\n";
    cout << "(3)  Show exams\n";
    cout << "(4)  Show Students and Teachers\n";
    cout << "(5)  Log out\n";
    cout << "(6)  Exit\n";
    cout << "Please enter a number from above: ";
    cin >> command;
    cout << repeater('\n', 60);
    if (command == "1") {
        signup("admin", "teacher");
    }
    if (command == "2") {
        signup("admin", "student");
    }
    if (command == "3") {
        show_exam(admin);
    }
    if (command == "4") {
        char command;
        int size = 0;
        user* user_info = read_all_file(size); // get all user data
        sort(user_info, user_info + size, compare_user); //sort user data with compare function

        for (int i = 0; i < size; i++) {
            cout << user_info[i].username << '\t' << user_info[i].password << '\t' << user_info[i].type << '\n';
        }
        delete[] user_info;
        cout << "Press anything and press Enter to return\n";
        cin >> command;
        cout << repeater('\n', 60);
        admin_dashboard();
    }
    if (command == "5") {
        menu();
    }
    if (command == "6") {
        exit(0);
    } 
    else {
        // restart
        cout << "Please try again\n";
        admin_dashboard();
    }
}

// compare two user first by their type then by username
bool compare_user(const user &a, const user &b)
{
    if (a.type != b.type)
        return a.type > b.type;
    return a.username < b.username;
}

void teacher_dashboard(const user &us)
{
    string command;
    cout << "(!" << repeater('-') << " WELCOME " << us.username << ' ' << repeater('-') << "!)\n";
    cout << "(1)  Show exams\n";
    cout << "(2)  Make exams\n";
    cout << "(3)  Log out\n";
    cout << "(4)  Exit\n";
    cout << "Please enter a number from above: ";
    cin >> command;
    cout << repeater('\n', 60);
    if (command == "1") {
        show_exam(us);
    }
    if (command == "2") {
        make_exam(us);
    }
    if (command == "3") {
        menu();
    }
    if (command == "4") {
        exit(0);
    }
    else {
        // restart
        cout << "Please try again\n";
        teacher_dashboard(us);
    }
}

void make_exam(const user &teacher)
{
    tm start_time;
    start_time.tm_isdst = -1;
    int minutes, num_of_questions;
    char colon;
    exam_info exam_data;
    question_info question_data;
    int size = get_file_size("exam_time.dat", '\n');
    exam_data.id = size + 1;
    exam_data.teacher_username = teacher.username;

    cout << "Enter the year of the Exam: ";
    cin >> start_time.tm_year; // year
    start_time.tm_year -= 1900;
    cout << repeater('\n', 60);

    cout << "Enter the month of the Exam: ";
    cin >> start_time.tm_mon; // month
    start_time.tm_mon--;
    cout << repeater('\n', 60);

    cout << "Enter the day of the Exam: ";
    cin >> start_time.tm_mday; // day of the month
    cout << repeater('\n', 60);

    cout << "Enter the time(hour and minute) of the Exam like this HH:MM : ";
    cin >> start_time.tm_hour >> colon >> start_time.tm_min;// time
    start_time.tm_sec = 0;
    cout << repeater('\n', 60);

    exam_data.start_time = mktime(&start_time); // change tm struct to time_t type

    cout << "Enter the time of the Exam (minutes): ";
    cin >> minutes;
    exam_data.end_time = exam_data.start_time + (60 * minutes); // calculate end time
    cout << repeater('\n', 60);

    write_time_data(exam_data); // write data to exam_time file

    cout << "Enter number of the questions: ";
    cin >> num_of_questions;
    cin.ignore();
    cout << repeater('\n', 60);

    // taking every question data of the exam
    for (int i = 1; i <= num_of_questions; i++) {
        cout << "Please enter question number " << i << "\n"
             << "put a backslash '\\' and press enter when question end\n"
             << "WARNING: Do not use '\\' in the middle of question\n";
        getline(cin, question_data.question, '\\'); // get question
        cin.ignore();
        // answer options
        for (int j = 0; j < 4; j++) {
            cout << "enter option " << j + 1 << ": ";
            getline(cin, question_data.option[j]);
        }
        cout << "Please enter answer to question number " << i << " (1-4): ";
        cin >> question_data.answer; // correct answer
        cin.ignore();
        // write to file
        write_question_data(question_data, size + 1);
        cout << repeater('\n', 60);
    }
    cout << "Successfully added!\n";
    teacher_dashboard(teacher);
}

void show_exam(const user &us)
{
    tm *stime;
    tm *etime;
    time_t now;
    time(&now); // current system time
    char start_buffer[80], end_buffer[80];
    int id;
    fstream fpread;
    fpread.open ("exam_time.dat", ios::in);
    // no file means no exam
    if (!fpread.is_open()) {
        cout << "There is no exam yet!\n";
        if (us.type == "admin")
            return admin_dashboard();
        if (us.type == "teacher")
            teacher_dashboard(us);
        student_dashboard(us);
    }
    // read all exam info
    int size = get_file_size("exam_time.dat", '\n'); // number of exams 
    exam_info *exam_read = new exam_info[size];
    for (int i = 0; i < size; i++) {
        fpread >> exam_read[i].id >> exam_read[i].teacher_username >> exam_read[i].start_time >> exam_read[i].end_time;
    }
    fpread.close();

    //showing exams info to user
    for (int i = 0; i < size; i++) {
        stime = localtime(&exam_read[i].start_time);
        strftime (start_buffer, 80, "%Y/%m/%d %H:%M", stime);
        etime = localtime(&exam_read[i].end_time);
        strftime (end_buffer, 80, "%Y/%m/%d %H:%M", etime);
        cout << '(' << exam_read[i].id << ")\tteacher: " << exam_read[i].teacher_username
             << "\tStart time: " << start_buffer
             << "\tEnd time: " << end_buffer << '\t';
        if (difftime (now, exam_read[i].start_time) < 0) {
            // not started
            cout << "Not started yet";
        }
        else if (difftime (now, exam_read[i].end_time) > 0) {
            // ended
            cout << "Ended";
        }
        else {
            // happening now
            cout << "In progress";
        }
        cout << '\n';
    }
    cout << "enter '0' to return or enter exam id: ";
    cin >> id;
    cout << repeater('\n', 60);
    char r;
    string string_id = to_string(id); // change id to string
    
    if (id == 0) {
        // return
        delete[] exam_read;
        if (us.type == "admin")
            admin_dashboard();
        if (us.type == "teacher")
            teacher_dashboard(us);
        student_dashboard(us);
    }
    else if (already_taken_exam(us, string_id)) {
        delete[] exam_read;
        cout << "you already taken this exam!\n";
        show_student_points(us, string_id);
        cout << "Press anything and then enter to return\n";
        cin >> r;
        cout << repeater('\n', 60);
        show_exam(us);
    }
    else if (difftime (now, exam_read[id - 1].start_time) < 0) {
        delete[] exam_read;
        // not started
        cout << "This exam has not started yet\n";;
        show_exam(us);
    }
    else if (difftime (now, exam_read[id - 1].end_time) > 0) {
        // ended
        if (us.type == "student") {
            delete[] exam_read;
            cout << "the time for this exam has ended you cannot take this exam\n";
            cout << "Press anything and then enter to return\n";
            cin >> r;
            cout << repeater('\n', 60);
            show_exam(us);
        }
        else if (us.type == "admin" | exam_read[id - 1].teacher_username == us.username) {
            delete[] exam_read;
            show_points(string_id);
            cout << "Press anything and then enter to return\n";
            cin >> r;
            cout << repeater('\n', 60);
            show_exam(us);
        }
        else {
            delete[] exam_read;
            cout << "you did not create this exam\n";
            teacher_dashboard(us);
        }
    }
    else {
        // happening now
        time_t endtime = exam_read[id - 1].end_time;
        delete[] exam_read;
        // taking exam if user is student
        if (us.type == "student") {
            take_exam(us, string_id, endtime);
        }
        cout << "still in progress\n";
        show_exam(us);
    }
}

void take_exam(const user &student, const string &exam_id, const time_t &end_time)
{
    string question_filename = "exam" + exam_id + ".dat";
    string answer_filename = "answer" + exam_id + ".dat";
    string points_filename = "points" + exam_id + ".dat";
    string question_text, answer_text;
    int i = 1 , student_answer, answer, points = 0;

    fstream fpread_question, fpread_answer;
    fpread_question.open(question_filename, ios::in);
    fpread_answer.open(answer_filename, ios::in);
    if (!fpread_question.is_open()) {
        cout << "exam does not exist!";
    }
    if (!fpread_answer.is_open()) {
        cout << "exam answer does not exist!";
    }

    int num_of_questions = get_file_size(answer_filename, ' '); // get number of questions from answer file
    chrono::system_clock::time_point end = chrono::system_clock::from_time_t(end_time);
    while (chrono::system_clock::now() < end) {
        while (getline(fpread_question, question_text, '\\')) {
            fpread_question.ignore();
            cout << "question " << i << "\n"
                << question_text << "Please enter your answer(1-4): ";
            cin >> student_answer;
            cout << repeater('\n', 60);
            fpread_answer >> answer;
            if (answer == student_answer) {
                points++;
            }
            i++;
        }
        if (num_of_questions == i - 1) {
            break;
        } 
    }
    fpread_answer.close();
    fpread_question.close();

    // write result in file
    fstream fwrite_points;
    fwrite_points.open(points_filename , ios::out | ios::app);
    fwrite_points << student.username << ' ' << points << '\n';
    fwrite_points.close();

    student_dashboard(student);
}

void student_dashboard(const user &us)
{
    string command;
    cout << "(!" << repeater('-') << " WELCOME " << us.username << ' ' << repeater('-') << "!)\n";
    cout << "(1)  Show exams\n";
    cout << "(2)  Log out\n";
    cout << "(3)  Exit\n";
    cout << "Please enter a number from above: ";
    cin >> command;
    cout << repeater('\n', 60);
    if (command == "1") {
        show_exam(us);
    }
    if (command == "2") {
        menu();
    }
    if (command == "3") {
        exit(0);
    }
    else {
        // restart
        cout << "Please try again\n";
        student_dashboard(us);
    }
}

// read all points from file
void show_points(const string &exam_id)
{
    string answer_filename = "answer" + exam_id + ".dat";
    int num_of_questions = get_file_size(answer_filename, ' ');
    string points_filename = "points" + exam_id + ".dat";
    string points_text;
    fstream fread_points;
    fread_points.open(points_filename , ios::in);
    if (!fread_points.is_open()) {
        cout << "nobody participated in your exam\n";
    }
    
    while (getline(fread_points, points_text)) {
        cout << points_text << " / " << num_of_questions << '\n';
    }
    fread_points.close();
}

// read a single student result for a single exam
void show_student_points(const user &student, const string &exam_id)
{
    bool find = false;
    string points_filename = "points" + exam_id + ".dat";
    string answer_filename = "answer" + exam_id + ".dat";
    int num_of_questions = get_file_size(answer_filename, ' '); // get number of questions from answer file
    int size = get_file_size(points_filename, '\n'); // get number of people who participated in exam
    string username, points;

    fstream fread_points;
    fread_points.open(points_filename , ios::in);
    if (!fread_points.is_open()) {
        cout << "nobody participated in this exam\n";
    }
    for (int i = 0; i < size; i++) {
        fread_points >> username >> points;
        if (username == student.username) {
            cout << points << " / " << num_of_questions << '\n';
            find = true;
            break;
        }   
    }
    if (!find) {
        cout << "couldn't find your result\n";
    }
    fread_points.close();
}

bool already_taken_exam(const user &student, const string &exam_id)
{
    string points_filename = "points" + exam_id + ".dat";
    string answer_filename = "answer" + exam_id + ".dat";
    int num_of_questions = get_file_size(answer_filename, ' '); // get number of questions from answer file
    int size = get_file_size(points_filename, '\n'); // get number of people who participated in exam
    string username, points;

    fstream fread_points;
    fread_points.open(points_filename , ios::in);
    if (!fread_points.is_open()) {
        return false;
    }
    for (int i = 0; i < size; i++) {
        fread_points >> username >> points;
        if (username == student.username) {
            cout << points << " / " << num_of_questions << '\n';
            fread_points.close();
            return true;
        }   
    }
    fread_points.close();
    return false;
}
