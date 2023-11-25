#pragma once
#include <iostream>
#include <string>
#include <iomanip>
#include "recordStruct.h"

using namespace std;

struct Vertex {
    Record* data;
    Vertex* left;
    Vertex* right;
};

string prompt(const string& str) {
    cout << str;
    cout << "\n> ";
    string ans;
    cin >> ans;
    return ans;
}

void print_head() {
    cout << "Author       Title                           Publisher        Year  Num of pages\n";
}

void print_record(Record* record) {
    cout << record->Author
        << "  " << record->Tittle
        << "  " << record->Publisher
        << "  " << record->Year
        << "  " << record->PageNumber << "\n";
}
void Print_tree(Vertex* p) {
    static int i = 0;
    if (p) {
        Print_tree(p->left);
        std::cout << "[" << i++ << "] ";
        print_record(p->data);
        Print_tree(p->right);
    }
}

void SDPREC(Record* D, Vertex*& p) {
    if (!p) {
        p = new Vertex;
        p->data = D;
        p->left = nullptr;
        p->right = nullptr;
    }

    else if (D->PageNumber < p->data->PageNumber) {
        SDPREC(D, p->left);
    }
    else if (D->PageNumber >= p->data->PageNumber) {
        SDPREC(D, p->right);
    }
}

void A2(int L, int R, int w[], Record* V[], Vertex*& root) {
    int wes = 0, sum = 0;
    int i;
    if (L <= R) {
        for (i = L; i <= R; i++)
            wes += w[i];
        for (i = L; i <= R - 1; i++) {
            if ((sum < (wes / 2)) && ((sum + w[i]) > (wes / 2))) break;
            sum += w[i];
        }
        SDPREC(V[i], root);
        A2(L, i - 1, w, V, root);
        A2(i + 1, R, w, V, root);
    }
}

void rmtree(Vertex* root) {
    if (root) {
        rmtree(root->right);
        rmtree(root->left);
        delete root;
    }
}

void search_in_tree(Vertex* root, int key) {
    if (root) {
        if (root->data->PageNumber > key) {
            search_in_tree(root->left, key);
        }
        else if (root->data->PageNumber < key) {
            search_in_tree(root->right, key);
        }
        else if (root->data->PageNumber == key) {
            search_in_tree(root->left, key);
            print_record(root->data);
            search_in_tree(root->right, key);
        }
    }
}

void tree(Record* arr[],int n) {
    Vertex* root = nullptr;
    int* w = new int[n + 1];
    for (int i = 0; i <= n; ++i) {
        w[i] = rand() % 100;
    }
    A2(1, n, w, arr, root);
    print_head();
    Print_tree(root);
    int key;
    do {
        while (true) {
            try {
                key = std::stoi(prompt("Input search key (page num), 0 - exit"));
                break;
            }
            catch (invalid_argument& exc) {
                cout << "Input a number\n";
                continue;
            }
        }
        print_head();
        search_in_tree(root, key);
    } while (key != 0);
    rmtree(root);
}