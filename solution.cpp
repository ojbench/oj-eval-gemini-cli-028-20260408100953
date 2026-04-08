#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace std;
using namespace __gnu_pbds;

struct Student {
    string name;
    char gender;
    int cls;
    int scores[9];
    int sum;
    
    int snap_scores[9];
    int snap_sum;
    
    Student() : sum(0), snap_sum(0) {
        for (int i = 0; i < 9; ++i) {
            scores[i] = 0;
            snap_scores[i] = 0;
        }
    }
};

struct CompareStudent {
    bool operator()(const Student* a, const Student* b) const {
        if (a->snap_sum != b->snap_sum) {
            return a->snap_sum > b->snap_sum;
        }
        for (int i = 0; i < 9; ++i) {
            if (a->snap_scores[i] != b->snap_scores[i]) {
                return a->snap_scores[i] > b->snap_scores[i];
            }
        }
        return a->name < b->name;
    }
};

typedef tree<
    Student*,
    null_type,
    CompareStudent,
    rb_tree_tag,
    tree_order_statistics_node_update>
ordered_set;

unordered_map<string, Student*> name_to_student;
ordered_set ranking_tree;
unordered_set<Student*> dirty_students;
bool started = false;

void calculate_sum(Student* s) {
    s->sum = 0;
    for (int i = 0; i < 9; ++i) {
        s->sum += s->scores[i];
    }
}

void flush_ranking() {
    for (Student* s : dirty_students) {
        ranking_tree.erase(s);
        s->snap_sum = s->sum;
        for (int i = 0; i < 9; ++i) {
            s->snap_scores[i] = s->scores[i];
        }
        ranking_tree.insert(s);
    }
    dirty_students.clear();
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string cmd;
    while (cin >> cmd) {
        if (cmd == "ADD") {
            string name;
            char gender;
            int cls;
            int scores[9];
            cin >> name >> gender >> cls;
            for (int i = 0; i < 9; ++i) {
                cin >> scores[i];
            }

            if (started) {
                cout << "[Error]Cannot add student now.\n";
            } else if (name_to_student.count(name)) {
                cout << "[Error]Add failed.\n";
            } else {
                Student* s = new Student();
                s->name = name;
                s->gender = gender;
                s->cls = cls;
                for (int i = 0; i < 9; ++i) {
                    s->scores[i] = scores[i];
                }
                calculate_sum(s);
                name_to_student[name] = s;
            }
        } else if (cmd == "START") {
            started = true;
            for (auto& pair : name_to_student) {
                Student* s = pair.second;
                s->snap_sum = s->sum;
                for (int i = 0; i < 9; ++i) {
                    s->snap_scores[i] = s->scores[i];
                }
                ranking_tree.insert(s);
            }
            dirty_students.clear();
        } else if (cmd == "UPDATE") {
            string name;
            int code, score;
            cin >> name >> code >> score;
            if (!name_to_student.count(name)) {
                cout << "[Error]Update failed.\n";
            } else {
                Student* s = name_to_student[name];
                s->scores[code] = score;
                calculate_sum(s);
                dirty_students.insert(s);
            }
        } else if (cmd == "FLUSH") {
            flush_ranking();
        } else if (cmd == "PRINTLIST") {
            int rank = 1;
            for (Student* s : ranking_tree) {
                cout << rank++ << " " << s->name << " " 
                     << (s->gender == 'M' ? "male" : "female") << " " 
                     << s->cls << " " << s->snap_sum / 9 << "\n";
            }
        } else if (cmd == "QUERY") {
            string name;
            cin >> name;
            if (!name_to_student.count(name)) {
                cout << "[Error]Query failed.\n";
            } else {
                Student* s = name_to_student[name];
                int rank = ranking_tree.order_of_key(s) + 1;
                cout << "STUDENT " << name << " NOW AT RANKING " << rank << "\n";
            }
        } else if (cmd == "END") {
            break;
        }
    }

    for (auto& pair : name_to_student) {
        delete pair.second;
    }

    return 0;
}
