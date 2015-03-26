#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <stdbool.h>

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>

#include "fileRead.h"
#include "VectorHash.h"
#include "utilsToStudents.h"

using namespace std;

////////////////////////////////////////////////////////////////
/// Illustrates how to use unordered_map STD library for language model.
// Each n-gram is stored as vector<T> of size n.
// T is "string" for word language model, and "char" for character language model.
// The function creates all n-grams from the input file, and then prints them to the
// screen with their counts.  Also outputs the count of the most frequent n-gram,
// the total number of distinct n-grams, and number n-grams read from file.
// By default, tokens are read without EOS.
// Comment line
//    read_tokens(filename, tokens, false);
// and uncomment line
//   read_tokens(filename, tokens, true);
// to read tokens with EOS marker.  Note that for T = char, EOS marker is not produced.
////////////////////////////////////////////////////////////////


typedef string T;   // Currently tests string language model.
//typedef char T;   // Uncomment this and comment above for char language model.

unordered_map<vector<T>, int> getNgrams(string f0, unsigned int n)
{
    //cout <<"\n\nTesting language model with n = " << n;

    int maxCount = 0; // will store the count of most frequent nGram
    int N = 0;        // will store the number of tokens read from file

    try {
        // First read all tokens from file into a vector
        vector<T> tokens;     // stores tokens read from file

        N = tokens.size();    // that is the number of words to process

        read_tokens(f0, tokens, false); // reads tokens from file without EOS
        //read_tokens(filename, tokens, true); // reads tokens from file with EOS

        // Initialize hash table storing n-grams. Each nGram is a vector<T>, used as hashtable key.
        // Associated with the key is the count of that n-gram
        unordered_map<vector<T>, int> database;

        // Now create all n-grams from vector of tokens and insert them into the database
        if (tokens.size() < n) {
            cout << "\nInput file is too small to create any nGrams of size " << n;
        } else {
            for (unsigned int i = 0; i <= tokens.size() - n; i++) {
                vector<T> nGram(n);   // for temporarily storing tokens to go into next n-gram

                // Take next n tokens read from the input file
                for (unsigned int j = 0; j < n; j++) // put next n tokens into vector temp
                    nGram[j] = tokens[i+j];

                if (database.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
                    database[nGram] = 1;
                else // nGram is already in the database, increase its count by 1
                    database[nGram] = database[nGram] + 1;

                if (maxCount < database[nGram])   // update maximum nGram count, if needed
                    maxCount = database[nGram];

            }
        }

        return database;
    }
    catch (FileReadException e) {
        e.Report();
    }

}

int main(int argc, char** argv)
{
    //argv[1] = "DostoevskyPart1.txt";
    //argv[2] = "DostoevskyPart2.txt";
    //argv[3] = "1"; // n = 5, 99.2807
    //argv[4] = "1";

    string s0 = argv[1];
    string s1 = argv[2];
    int nGramSize = atoi(argv[3]);
    bool printCommonNgrams = (strcmp(argv[4], "1") == 0);

    unordered_map<vector<T>, int> f0 = getNgrams(s0, nGramSize);
    unordered_map<vector<T>, int> f1 = getNgrams(s1, nGramSize);
    vector<vector<T>>  similar;

    int inF1notInF0 = 0;

    for (std::unordered_map<vector<T>, int>::iterator i = f1.begin(); i != f1.end(); ++i) {
        if (f0.count(i->first) == 0) {
            inF1notInF0++;
        } else {
            similar.push_back(i->first);
        }
    }
    //printf("\nin F1 not F0:  %d\n", inF1notInF0);
    //printf("\ntotal size F0: %d\n", f1.size());
    printf("%.3f\n", 100*((double)inF1notInF0)/f1.size());

    if (printCommonNgrams) {
        for (std::vector<vector<T>>::iterator i = similar.begin(); i != similar.end(); ++i) {
            for (std::vector<T>::iterator j = i->begin(); j != i->end(); ++j) {
                cout << *j << " ";
            }
            printf("\n");
        }
    }

    //getchar();

    return 0;
}

