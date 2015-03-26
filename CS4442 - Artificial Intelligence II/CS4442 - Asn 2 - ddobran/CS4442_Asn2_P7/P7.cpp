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

typedef string T;   // Currently tests string language model.
//typedef char T;   // Uncomment this and comment above for char language model.

unordered_map<vector<T>, int> getNgramsForAllN(string f0, unsigned int nGramSize, bool useEos, unsigned int& numTokens)
{
    // First read all tokens from file into a vector
    vector<T> tokens;     // stores tokens read from file
    try {
        //read_tokens(f0, tokens, false); // reads tokens from file without EOS
        read_tokens(f0, tokens, useEos); // reads tokens from file with EOS
    }
    catch (FileReadException e) {
        e.Report();
        exit(EXIT_FAILURE);
    }

    numTokens = tokens.size();

    unordered_map<vector<T>, int> database;

    for (unsigned int n = 1; n <= nGramSize; n++) {
        for (unsigned int i = 0; i <= tokens.size() - n; i++) {
            vector<T> nGram(n);   // for temporarily storing tokens to go into next n-gram

            // Take next n tokens read from the input file 
            for (unsigned int j = 0; j < n; j++) // put next n tokens into vector temp
                nGram[j] = tokens[i+j];

            if (database.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
                database[nGram] = 1;
            else // nGram is already in the database, increase its count by 1
                database[nGram] = database[nGram] + 1;
        }
    }
    return database;
}

unordered_map<vector<T>, int> getNgramsFromTokens(vector<T>& stringTokens, unsigned int nGramSize)
{
    unordered_map<vector<T>, int> database;

    for (unsigned int n = 1; n <= nGramSize; n++) {
        for (unsigned int i = 0; i <= stringTokens.size() - n; i++) {
            vector<T> nGram(n);   // for temporarily storing tokens to go into next n-gram

            // Take next n tokens read from the input file 
            for (unsigned int j = 0; j < n; j++) // put next n tokens into vector temp
                nGram[j] = stringTokens[i+j];

            if (database.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
                database[nGram] = 1;
            else // nGram is already in the database, increase its count by 1
                database[nGram] = database[nGram] + 1;
        }
    }
    return database;
}

unordered_set<T> generateUnigrams(string f0, bool readWithEoS)
{
    try {
        // First read all tokens from file into a vector
        vector<T> tokens;     // stores tokens read from file

        //read_tokens(f0, tokens, false); // reads tokens from file without EOS
        read_tokens(f0, tokens, readWithEoS); // reads tokens from file with EOS

        unordered_set<T> unigrams;
        for (vector<T>::iterator i = tokens.begin(); i != tokens.end(); ++i) {
            unigrams.emplace(*i);
        }

        return unigrams;
    }
    catch (FileReadException e) {
        e.Report();
    }

    return unordered_set<T>();
}

vector<vector<T>> getSentences(string f0, bool keepEosMarker)
{
    // First read all tokens from file into a vector
    vector<T> tokens;     // stores tokens read from file
    vector<vector<T>> sentences;

    try {
        read_tokens(f0, tokens, true); // reads tokens from file with EOS
    }
    catch (FileReadException e) {
        e.Report();
    }

    for (unsigned int i = 0; i < tokens.size(); i++) {
        vector<T> sentence;
        while (strcmp(tokens[i].c_str(), "<END>") != 0 && i < tokens.size()) {
            sentence.push_back(tokens[i]);
            i++;
        }
        if (keepEosMarker) {
            sentence.push_back(tokens[i]);
        }
        sentences.push_back(sentence);
    }

    return sentences;
}

double getProbOfSentence(unordered_map<vector<T>, int>& trainNgrams,
                         unordered_map<vector<T>, int>& sentNgrams,
                         unsigned int nGramSize, double delta,
                         unsigned int numTokensTrain,
                         unsigned int vocabularySize)
{
    double prob;
    for (auto itr = sentNgrams.begin(); itr != sentNgrams.end(); itr++) {
        for (unsigned int j = 1; j < nGramSize; j++) {
            if (trainNgrams.count(itr->first)) {
                prob +=
                    log(
                    (trainNgrams[itr->first] + delta) /
                    (numTokensTrain + delta*pow(vocabularySize, j))
                    );
            } else {
                prob += log(delta/(numTokensTrain + delta*pow(vocabularySize, j)));
            }
        }
    }
    return prob;
}

void printSentence(vector<T> sentence)
{
    for (auto i = sentence.begin(); i != sentence.end(); i++) {
        cout << *i << " ";
    }
    cout << endl;
}

//struct ClassifierHolder
//{
//    ClassifierHolder() : probAddDelta(-DBL_MAX), probAddOne(-DBL_MAX) {}
//    double probAddDelta;
//    double probAddOne;
//};

int main(int argc, char** argv)
{
    //argv[1] = "KafkaTrial.txt";
    //argv[2] = "testFile.txt";
    argv[1] = "DostoevskyIdiot.txt";
    argv[2] = "DostoevskyKaramazov.txt";

    string textTrain = argv[1];
    string textTest = argv[2];

    unsigned int nGramSize = 3;
    double delta = 0.001;

    bool readEoS = false;

    /*
    Use word language model without reading EOS markers to read textTrain.txt and dictionary.txt.
    File textCheck will contain several sentences to spell checking of. Check each sentence sepa-
    rately. It is convenient to read textCheck.txt with EOS marker to make spell checking easier.
    */

    unsigned int numTokensTrain = 0;

    vector<T> trainTokens, testTokens;
    read_tokens(textTrain, trainTokens, false);
    read_tokens(textTest, testTokens, false);

    int sz0 = trainTokens.size();
    int sz1 = testTokens.size();
    if (sz0 > sz1) {
        for (int i = 0; i < sz0-sz1; i++) {
            trainTokens.pop_back();
        }
    } else {
        for (int i = 0; i < sz1-sz0; i++) {
            testTokens.pop_back();
        }
    }

    numTokensTrain = trainTokens.size();

    auto trainNgrams = getNgramsFromTokens(trainTokens, nGramSize);
    auto testNgrams = getNgramsFromTokens(testTokens, nGramSize);

    unsigned int vocabularySize = numTokensTrain*2;

    double probAddDelta = 0, probAddOne = 0;

    // Add Delta
    for (auto itr = testNgrams.begin(); itr != testNgrams.end(); itr++) {
        for (unsigned int j = 1; j < nGramSize; j++) {
            if (trainNgrams.count(itr->first)) {
                probAddDelta +=
                    log(
                    (trainNgrams[itr->first] + delta) /
                    (numTokensTrain + delta*pow(vocabularySize, j))
                    );
            } else {
                probAddDelta += log(delta/(numTokensTrain + delta*pow(vocabularySize, j)));
            }
        }
    }

    // Add One
    for (auto itr = testNgrams.begin(); itr != testNgrams.end(); itr++) {
        for (unsigned int j = 1; j < nGramSize; j++) {
            if (trainNgrams.count(itr->first)) {
                probAddOne +=
                    log(
                    (trainNgrams[itr->first] + 1) /
                    (numTokensTrain + 1*pow(vocabularySize, j))
                    );
            } else {
                probAddOne += log(1/(numTokensTrain + 1*pow(vocabularySize, j)));
            }
        }
    }

    cout << "Prob. Add Delta\tProb. Add One" << endl;
    printf("%-15.2lf\t%-13.2lf\t", probAddDelta, probAddOne);

    getchar();

    return 0;
}