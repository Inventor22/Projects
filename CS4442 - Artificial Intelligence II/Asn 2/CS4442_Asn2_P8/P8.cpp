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

unordered_map<vector<T>, int> getNgramsFromSentence(vector<T>& stringTokens, unsigned int nGramSize)
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

int main(int argc, char** argv)
{
    //argv[1] = "hugeTrain.txt";
    //argv[2] = "textCheck.txt";
    //argv[3] = "dictionary.txt";
    //argv[4] = "2";
    //argv[5] = "3";
    //argv[6] = "0.01";
    //argv[7] = "1";


    string textTrain = argv[1];
    string textTest = argv[2];
    string dictionary = argv[3];
    unsigned int nGramSize = atoi(argv[4]);
    int threshold = atoi(argv[5]);
    double delta = atof(argv[6]);
    int model = atoi(argv[7]);

    /*
    Use word language model without reading EOS markers to read textTrain.txt and dictionary.txt.
    File textCheck will contain several sentences to spell checking of. Check each sentence sepa-
    rately. It is convenient to read textCheck.txt with EOS marker to make spell checking easier.
    */

    unsigned int numTokensTrain = 0;
    unordered_map<vector<T>, int> trainNgrams = getNgramsForAllN(textTrain, nGramSize, false, numTokensTrain);
    unsigned int vocabularySize = numTokensTrain*2;

    unsigned int numTokensDict = 0;
    unordered_set<T> dictNGrams = generateUnigrams(dictionary, false);
    numTokensDict = dictNGrams.size();

    auto testSentences = getSentences(textTest, false);

    /*
    Assume that there is only one misspelled word per sentence. Given the next sentence S to check,
    first compute its probability. Next iterate over all words in the sentence. Suppose current
    word is w. Find all words in the dictionary that are within edit distance of one from w, using
    the function for edit distance that I provide in utilsToStudents.h. Let C(w) be the set of all
    words within edit distance of 1 from w. Replace w in the sentence with a word from C(w).
    Compute probability of this new sentence. If you get a larger probability, store the sentence.
    Repeat for all words in C(w), updating the current best sentence (i.e. the highest probability
    sentence). Repeat this process for all words in sentence S. Print to the output the best sentence.
    Note that the highest probability sentence can be the input sentence S unchanged.
    */

    for (auto i = testSentences.begin(); i != testSentences.end(); ++i) {
        auto currentSentence = *i;
        auto bestSentence = currentSentence;
        double bestProb = -DBL_MAX;
        auto sentenceNgrams = getNgramsFromSentence(*i, nGramSize);
        bestProb = getProbOfSentence(trainNgrams, sentenceNgrams, nGramSize, delta, numTokensTrain, vocabularySize);

        cout << bestProb << endl;

        for (unsigned int j = 0; j < currentSentence.size(); j++) {
            vector<T> lessThan1EditDistanceWords;
            for (auto k = dictNGrams.begin(); k != dictNGrams.end(); ++k) {
                if ((unsigned int)uiLevenshteinDistance(currentSentence[j], *k) <= 1) {
                    lessThan1EditDistanceWords.push_back(*k);
                }
            }

            //printSentence(lessThan1EditDistanceWords);

            for (auto m = lessThan1EditDistanceWords.begin(); m != lessThan1EditDistanceWords.end(); m++) {
                auto modSentence(currentSentence);
                modSentence[j] = *m;
                //printSentence(modSentence);
                auto newSentNgrams = getNgramsFromSentence(modSentence, nGramSize);
                double newProb = getProbOfSentence(trainNgrams, newSentNgrams, nGramSize, delta, numTokensTrain, vocabularySize);
                if (newProb > bestProb) {
                    bestProb = newProb;
                    bestSentence = modSentence;
                    cout << "bestProb " << bestProb << ", ";
                    printSentence(bestSentence);
                }
                if (m->compare("hear") == 0) {
                    cout << "hear " << newProb << ", ";
                    printSentence(modSentence);
                } else if (m->compare("it") == 0) {
                    cout << "it " << newProb << ", ";
                    printSentence(modSentence);
                }
            }
        }
        //printSentence(currentSentence);
        printSentence(bestSentence);
        cout << endl;
    }

    /*
    Padd1(ab) = C(ab)+1/(N+vocabularySize^2)
    */

    //getchar();

    return 0;
}