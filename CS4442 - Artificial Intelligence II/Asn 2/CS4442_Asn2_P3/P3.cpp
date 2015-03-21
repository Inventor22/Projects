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

unordered_map<vector<T>, int> getNgramsForAllN(string f0, unsigned int nGramSize)
{
    int maxCount = 0; // will store the count of most frequent nGram
    int N = 0;        // will store the number of tokens read from file

    // First read all tokens from file into a vector
    vector<T> tokens;     // stores tokens read from file
    try {
        //read_tokens(f0, tokens, false); // reads tokens from file without EOS
        read_tokens(f0, tokens, true); // reads tokens from file with EOS
    }
    catch (FileReadException e) {
        e.Report();
        exit(EXIT_FAILURE);
    }

    N = tokens.size();    // that is the number of words to process

    // Initialize hash table storing n-grams. Each nGram is a vector<T>, used as hashtable key.
    // Associated with the key is the count of that n-gram
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

            if (maxCount < database[nGram])   // update maximum nGram count, if needed 
                maxCount = database[nGram];

        }
        /*//////////////////////////////////////////////////////////////////////////
        // now go over all nGrams in the database and print them out with their counts

        // prints summary of nGrams inserted
        cout << "\nMax nGram count " << maxCount;
        cout << "\nNumber of distinct nGrams " << database.size();
        cout << "\nTotal number of words read " << N;


        // now go over all nGrams in the database and print them out with their counts
        cout <<"\nPrinting out all the nGrams with their counts";

        for (auto i = database.begin(); i != database.end(); ++i) {
        vector<T> nGram = i->first;
        cout << "\n";

        for (unsigned int j = 0; j < nGram.size(); j++)
        cout << nGram[j] << " ";  // prints out next string in the current nGram

        cout << " " << i->second; // prints out the count of this nGram
        }*/
    }
    return database;
}

unordered_set<T> generateUnigrams(string f0)
{
    try {
        // First read all tokens from file into a vector
        vector<T> tokens;     // stores tokens read from file

        //read_tokens(f0, tokens, false); // reads tokens from file without EOS
        read_tokens(f0, tokens, true); // reads tokens from file with EOS

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

unordered_map<vector<T>, int> getNgrams(string f0, unsigned int n, int& numWords)
{
    int maxCount = 0; // will store the count of most frequent nGram
    int N = 0;        // will store the number of tokens read from file

    // First read all tokens from file into a vector
    vector<T> tokens;     // stores tokens read from files

    try {
        //read_tokens(f0, tokens, false); // reads tokens from file without EOS
        read_tokens(f0, tokens, true); // reads tokens from file 
        N = tokens.size();    // that is the number of words to process
    }
    catch (FileReadException e) {
        e.Report();
    }
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

            numWords++;
        }
    }

    return database;

}

int main(int argc, char** argv)
{
    //argv[1] = "Dickens.txt";
    //argv[2] = "MarxEngelsManifest.txt";
    //argv[1] = "testFile.txt";
    //argv[1] = "KafkaTrial.txt";
    //argv[2] = "2";

    string s0 = argv[1];
    unsigned int nGramSize = atoi(argv[2]);

    unordered_map<vector<T>, int> mll = getNgramsForAllN(s0, nGramSize);


    int numWords = 0;
    unordered_map<vector<T>, int> unigramsMap = getNgrams(s0, 1, numWords);

    int numUnigrams = unigramsMap.size();

    vector<T> unigrams;
    vector<double> probV;
    probV.reserve(unigrams.size());

    for (unordered_map<vector<T>, int>::iterator i = unigramsMap.begin(); i != unigramsMap.end(); ++i) {
        unigrams.push_back((i->first).at(0));
        probV.push_back(((double)i->second)/numWords);
    }

    int firstWordIndex = drawIndex(probV);

    vector<T> sentence;

    sentence.push_back(unigrams.at(firstWordIndex));

    if (nGramSize == 1) {
        while (sentence.at(sentence.size()-1).compare("<END>") != 0) {
            int indexOfNextWord = drawIndex(probV);
            string nextWord = unigrams.at(indexOfNextWord);
            sentence.push_back(nextWord);
        }
    } else {
        while (true) {
            vector<double> probs;
            probs.reserve(unigrams.size());
            if (sentence.size() < nGramSize) {
                for (unsigned int i = 0; i < unigrams.size(); i++) {
                    vector<T> next(sentence);
                    next.push_back(unigrams.at(i));
                    if (mll.count(next) && mll.count(sentence)) {
                        probs.push_back(((double)mll[next])/mll[sentence]);
                    } else {
                        probs.push_back(0);
                    }
                }
            } else {
                vector<T> lastSentence;
                for (unsigned int j = sentence.size()-nGramSize+1; j < sentence.size(); j++) {
                    lastSentence.push_back(sentence.at(j));
                }

                for (unsigned int i = 0; i < unigrams.size(); i++) {
                    vector<T> next(lastSentence);
                    next.push_back(unigrams.at(i));
                    //if (mll.count(next) == 0 || mll.count(lastSentence) == 0) {
                    //    int a = 5;
                    //}
                    if (mll.count(next) && mll.count(lastSentence)) {
                        probs.push_back(((double)mll[next])/mll[lastSentence]);
                    } else {
                        probs.push_back(0);
                    }
                }
            }

            int indexOfNextWord = drawIndex(probs);
            string nextWord = unigrams.at(indexOfNextWord);
            sentence.push_back(nextWord);

            if (sentence.at(sentence.size()-1).compare("<END>") == 0) {
                break;
            }
        }
    }

    for (unsigned int i = 0; i < sentence.size(); i++) {
        cout << sentence[i] << " ";
    }
    cout << endl;

    return 0;
}
