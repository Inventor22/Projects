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

typedef string T;   // Currently tests word language model.

vector<unordered_map<vector<T>, int>> getNgramsForAllN(string f0, unsigned int nGramSize, bool withEOS, unsigned int& numTokens)
{
    // First read all tokens from file into a vector
    vector<T> tokens;     // stores tokens read from file
    try {
        //read_tokens(f0, tokens, false); // reads tokens from file without EOS
        read_tokens(f0, tokens, withEOS); // reads tokens from file with EOS
    }
    catch (FileReadException e) {
        e.Report();
        exit(EXIT_FAILURE);
    }

    numTokens = tokens.size();

    vector<unordered_map<vector<T>, int>> database;

    for (unsigned int n = 1; n <= nGramSize; n++) {
        unordered_map<vector<T>, int> nGramDatabase;
        for (unsigned int i = 0; i <= tokens.size() - n; i++) {
            vector<T> nGram(n);   // for temporarily storing tokens to go into next n-gram

            // Take next n tokens read from the input file
            for (unsigned int j = 0; j < n; j++) // put next n tokens into vector temp
                nGram[j] = tokens[i+j];

            if (nGramDatabase.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
                nGramDatabase[nGram] = 1;
            else // nGram is already in the database, increase its count by 1
                nGramDatabase[nGram] = nGramDatabase[nGram] + 1;
        }
        database.push_back(nGramDatabase);
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

unordered_map<vector<T>, int> getNgrams(string f0, unsigned int n, unsigned int& numWords, bool useEosMarker)
{
    int maxCount = 0; // will store the count of most frequent nGram
    int N = 0;        // will store the number of tokens read from file

    // First read all tokens from file into a vector
    vector<T> tokens;     // stores tokens read from files

    try {
        //read_tokens(f0, tokens, false); // reads tokens from file without EOS
        read_tokens(f0, tokens, useEosMarker); // reads tokens from file 
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

unordered_map<vector<T>, int> getNgramsFromVector(vector<T> tokens, unsigned int nGramSize)
{
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

int main(int argc, char** argv)
{
    string textModel = argv[1];
    string sentenceFile = argv[2];
    unsigned int nGramSize = atoi(argv[3]);
    double param = atof(argv[4]);
    bool useAddDelta = (bool)atoi(argv[5]);

    int threshold = (int)param;
    double delta = 0;

    if (useAddDelta) {
        delta = param;
    }

    bool useEosMarker  = false;
    bool keepEosMarker = false;

    unsigned int numWords = 0;

    vector<unordered_map<vector<T>, int>> nGrams = getNgramsForAllN(textModel, nGramSize, useEosMarker, numWords);

    int vocabularySize = 2 * nGrams[1].size(); // number of unigrams * 2

    vector<vector<T>> sentences = getSentences(sentenceFile, keepEosMarker);

    if (useAddDelta) {

        for (unsigned int i = 0; i < sentences.size(); i++) {
            double prob = 0;
            unordered_map<vector<T>, int> sentenceNgrams = getNgramsFromVector(sentences[i], nGramSize);
            for (unordered_map<vector<T>, int>::iterator itr = sentenceNgrams.begin(); itr != sentenceNgrams.end(); itr++) {
                for (unsigned int j = 1; j < nGramSize; j++) {
                    if (nGrams[j].count(itr->first)) {
                        prob +=
                            log(
                                (nGrams[j][itr->first] + delta) /
                                (numWords + delta*pow(vocabularySize, j))
                            );
                    } else {
                        prob += log(delta/(numWords + delta*pow(vocabularySize, j)));
                    }
                }
            }
            printf("%.2lf\n", prob);
        }

    } else { // use Good-Turing

        double totalPossibleNgrams = pow(vocabularySize, nGramSize);

        unordered_map<vector<T>, int> nGramBag;
        for (unsigned int i = 0; i < nGrams.size(); i++) {
            nGramBag.insert(nGrams[i].begin(), nGrams[i].end());
            //for (unordered_map<vector<T>, int>::iterator j = nGrams[i].begin(); j != nGrams[i].end(); ++j) {
            //    GoodTuringParam g;
            //    g.count = j->second;
            //    g.prob = 0.0;
            //    nGramBag.emplace(j->first, g);
            //}
        }
        int max = 0;
        for (auto i = nGramBag.begin(); i != nGramBag.end(); i++) {
            if (i->second > max) {
                max = i->second;
            }
        }
        max++;

        int* Nr = new int[max];
        double* prob = new double[max];
        for (int i = 0; i < max; i++) {
            Nr[i] = 0;
            prob[i] = 0.0;
        }
        for (auto i = nGramBag.begin(); i != nGramBag.end(); i++) {
            Nr[i->second]++;
        }
        Nr[0] = (totalPossibleNgrams - nGramBag.size());
        for (int r = 0; r <= threshold; r++) {
            if (Nr[r] == 0) {
                cerr << "All values must be >0" << endl;
                exit(EXIT_FAILURE);
            }
            prob[r] = (r+1)*Nr[r+1]/(numWords*Nr[r]);
        }
        for (int i = threshold+1; i < max; i++) {
            prob[i] = Nr[i]/numWords;
        }

        double weightOfSeenNgrams = nGramBag.size() / (double)totalPossibleNgrams;
        double probOfSeenNgrams = 0;
        for (auto i = nGramBag.begin(); i != nGramBag.end(); i++) {
            probOfSeenNgrams += prob[i->second];
        }

        double normalizationFactor1 = weightOfSeenNgrams / probOfSeenNgrams;
        for (int i = 1; i < max; i++) {
            prob[i] *= normalizationFactor1;
        }
        double sum = 0;
        for (int i = 1; i < max; i++) {
            sum += prob[i];
        }
        for (int i = 1; i < max; i++) {
            prob[i] /= sum;
        }

        for (unsigned int i = 0; i < sentences.size(); i++) {
            double sentenceProb = 0;
            unordered_map<vector<T>, int> sentenceNgrams = getNgramsFromVector(sentences[i], nGramSize);
            for (auto itr = sentenceNgrams.begin(); itr != sentenceNgrams.end(); itr++) {
                sentenceProb += log(prob[itr->second]);
            }
            printf("%.2lf\n", prob);
        }
        // if ngram rate < threshold (r), use good-turing estimate of probability
        // if ngram rate >= threshold, use ML estimate
        // Nr = frequency of n-gram that occurs r times
    }

    //getchar();

    return 0;
}

