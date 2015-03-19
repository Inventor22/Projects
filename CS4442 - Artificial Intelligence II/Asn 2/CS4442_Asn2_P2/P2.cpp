#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <stdbool.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
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

unordered_set<vector<T>> getNgramsForAllN(string f0, unsigned int n)
{
    try {
        // First read all tokens from file into a vector
        vector<T> tokens;     // stores tokens read from file

        tokens.size();    // that is the number of words to process

        //read_tokens(f0, tokens, false); // reads tokens from file without EOS
        read_tokens(f0, tokens, true); // reads tokens from file with EOS
        vector<vector<T>> sentences;

        for (unsigned int i = 0; i < tokens.size(); i++) {
            vector<T> sentence;
            while (strcmp(tokens[i].c_str(), "<END>") != 0 && i < tokens.size()) {
                sentence.push_back(tokens[i]);
                i++;
            }
            sentence.push_back(tokens[i]);
            sentences.push_back(sentence);
        }

        // Initialize hash table storing n-grams. Each nGram is a vector<T>, used as hashtable key.
        // Associated with the key is the count of that n-gram
        unordered_set<vector<T>> database;

        // Now create all n-grams from vector of tokens and insert them into the database
        if (tokens.size() < n) {
            cout << "\nInput file is too small to create any nGrams of size " << n;
        } else {
            for (int x = 1; x <= n; x++) {
                for (vector<vector<T>>::iterator i = sentences.begin(); i != sentences.end(); ++i) {
                    for (int k = 0; k <= (int)i->size() - x; k++) {
                        vector<T> nGram(x);   // for temporarily storing tokens to go into next n-gram

                        // Take next n tokens read from the input file
                        for (int j = 0; j < x; j++) { // put next n tokens into vector temp
                            nGram[j] = (*i)[k+j];
                        }

                        if (database.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
                            database.emplace(nGram);
                    }
                }
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
    //argv[1] = "Dickens.txt";
    //argv[2] = "MarxEngelsManifest.txt";
    //argv[1] = "1.txt";
    //argv[2] = "2.txt";
    //argv[3] = "2";

    string s0 = argv[1];
    string s1 = argv[2];
    int nGramSize = atoi(argv[3]);

    //printf("1\n");
    // generate all (1-n)-grams for document 1
    unordered_set<vector<T>> f0 = getNgramsForAllN(s0, nGramSize);
    //printf("2\n");

    vector<T> tokens;     // stores tokens read from file

    read_tokens(s1, tokens, true); // reads tokens from file with EOS

    int sentenceCount = 0, zeroProbSentence = 0;
    for (unsigned int i = 0; i < tokens.size(); i++) {
        vector<T> sentence;
        // get the next sentence from the second file
        while (strcmp(tokens[i].c_str(), "<END>") != 0 && i < tokens.size()) {
            sentence.push_back(tokens[i]);
            i++;
        }
        sentence.push_back(tokens[i]);

        unordered_set<vector<T>> nGramsForSentence;

        for (int x = 1; x <= nGramSize; x++) {
            for (int k = 0; k <= (int)sentence.size() - x; k++) {
                vector<T> nGram(x);   // for temporarily storing tokens to go into next n-gram

                // Take next n tokens read from the input file
                for (unsigned int j = 0; j < x; j++) // put next n tokens into vector temp
                    nGram[j] = sentence[k+j];

                nGramsForSentence.emplace(nGram);
            }
        }
        bool nGramNotFoundInFirst = false;
        for (unordered_set<vector<T>>::iterator j = nGramsForSentence.begin(); j != nGramsForSentence.end(); ++j) {
            if (f0.count(*j)  ==  0) {
                nGramNotFoundInFirst = true;
                break;
            }
        }
        if (nGramNotFoundInFirst) {
            zeroProbSentence++;
            if (nGramSize >= 6) {
                for (vector<T>::iterator i = sentence.begin(); i != sentence.end(); ++i) {
                    cout << *i << " ";
                }
                cout << endl;
            }
        }
        sentenceCount++;
    }
    printf("%.3f\n", 100*((double)zeroProbSentence)/sentenceCount);

    getchar();

    return 0;
}

