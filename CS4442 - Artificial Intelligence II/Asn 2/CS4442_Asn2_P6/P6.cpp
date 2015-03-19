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

//typedef string T;   // Currently tests string language model.
typedef char T;   // Uncomment this and comment above for char language model.

unordered_map<vector<T>, int> getNgramsForAllN(string f0, unsigned int nGramSize, bool latin_only, unsigned int& numTokens)
{

    // First read all tokens from file into a vector
    vector<T> tokens;     // stores tokens read from file
    try {
        //read_tokens(f0, tokens, false); // reads tokens from file without EOS
        read_tokens(f0, tokens, latin_only); // reads tokens from file with EOS
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

unordered_map<vector<T>, int> getNgrams(string f0, unsigned int n, int& numWords, bool useEosMarker)
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

int main(int argc, char** argv)
{
    //argv[1] = "2";
    //argv[2] = "0.05";
    //argv[3] = "50";

    unsigned int nGramSize = atoi(argv[1]);
    double delta = atof(argv[2]);
    int senLength = atoi(argv[3]);

    //int vocabularySize = 256;
    int vocabularySize = 26;
    //bool latin_only = false;
    bool latin_only = true;
    string langStr[] = {"english", "danish", "french", "sweedish", "italian", "latin"};
    int numLanguages = sizeof(langStr)/sizeof(langStr[0]);
    int cm[6][6] = {{0}};

    unsigned int N[6] = {0};
    vector<unordered_map<vector<char>, int>> clm;
    clm.reserve(numLanguages);
    for (int i = 0; i < numLanguages; i++) {
        unordered_map<vector<T>, int> langNGrams = getNgramsForAllN(langStr[i]+"1.txt", nGramSize, latin_only, N[i]);
        clm.push_back(langNGrams);
    }

    for (int i = 0; i < numLanguages; i++) {
        // parse sentence
        // generate n grams from sentence
        // calculate probability of each n-gram in sentence using P_add_delta(ngram) = (count(n-gram)+delta)/(N+delta*|V|^n)
        // take log of probability and add to sum
        vector<T> tokensTest;
        read_tokens(langStr[i]+"2.txt", tokensTest, false);
        vector<vector<T>> sentences;
        unsigned int j = 0;
        while (j+senLength < tokensTest.size()) {
            vector<T> sentence;
            for (unsigned int k = j; k < j+senLength; k++) {
                sentence.push_back(tokensTest.at(k));
            }
            sentences.push_back(sentence);
            j += senLength;
        }

        for (unsigned int j = 0; j < sentences.size(); j++) {
            vector<T> sentence = sentences[j];
            vector<unordered_map<vector<T>, int>> nGrams; // maps for each 1-gram, 2-gram, etc.

            for (unsigned int n = 1; n <= nGramSize; n++) {
                unordered_map<vector<T>, int> nGramsOfn;
                for (unsigned int i = 0; i <= sentence.size() - n; i++) {
                    vector<T> nGram(n);   // for temporarily storing tokens to go into next n-gram

                    // Take next n tokens read from the input file
                    for (unsigned int j = 0; j < n; j++) // put next n tokens into vector temp
                        nGram[j] = sentence[i+j];

                    if (nGramsOfn.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
                        nGramsOfn[nGram] = 1;
                    else // nGram is already in the database, increase its count by 1
                        nGramsOfn[nGram] = nGramsOfn[nGram] + 1;
                }
                nGrams.push_back(nGramsOfn);
            }

            double probability[6] = {0};
            double bestProb = -DBL_MAX;
            int langIndex = 0;

            for (int lang = 0; lang < numLanguages; lang++) {
                for (unsigned int nn = 0; nn < nGrams.size(); nn++) {
                    unordered_map<vector<T>, int> nGramsOfn = nGrams[nn];
                    for (unordered_map<vector<T>, int>::iterator itr = nGramsOfn.begin(); itr != nGramsOfn.end(); ++itr) {
                        //P_add_delta(ngram) = (count(n-gram)+delta)/(N+delta*|V|^n)
                        if (clm.at(lang).count(itr->first)) {
                            probability[lang] +=
                                log(
                                (clm.at(lang)[itr->first] + delta)
                                /
                                (N[lang] + delta*pow(vocabularySize, nn))
                                );
                        } else {
                            probability[lang] +=
                                log(
                                (delta)
                                /
                                (N[lang] + delta*pow(vocabularySize, nn))
                                );
                        }
                    }
                }
            }
            bestProb = probability[langIndex];
            for (int lang = 1; lang < numLanguages; lang++) {
                if (probability[lang] > bestProb) {
                    bestProb = probability[lang];
                    langIndex = lang;
                }
            }

            cm[i][langIndex]++;


            //cout << "P(";
            //std::copy(sentence.begin(), sentence.end(), std::ostream_iterator<char>(std::cout, " ")); 
            //cout << ") = " << bestProb << " [" << langStr[langIndex] << "]" << endl;
        }
    }

    double totalSum = 0;
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 6; col++) {
            totalSum += cm[row][col];
        }
    }
    double diagonal = 0;
    for (int i = 0; i < 6; i++) {
        diagonal += cm[i][i];
    }
    printf("%.2f\n", 100*(totalSum - diagonal)/totalSum);

    // print confusion matrix.
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 6; col++) {
            cout << cm[row][col] << "\t";
        }
        cout << endl;
    }

    /*
    Padd1(ab) = C(ab)+1/(N+vocabularySize^2)
    */

    //getchar();

    return 0;
}

unordered_map<vector<T>, int> getNgramsForAllN(string f0, unsigned int nGramSize, bool latin_only, unsigned int& numTokens)
{

    // First read all tokens from file into a vector
    vector<T> tokens;     // stores tokens read from file
    try {
        //read_tokens(f0, tokens, false); // reads tokens from file without EOS
        read_tokens(f0, tokens, latin_only); // reads tokens from file with EOS
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

unordered_map<vector<T>, int> getNgrams(string f0, unsigned int n, int& numWords, bool useEosMarker)
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

int main(int argc, char** argv)
{
    //argv[1] = "textTrain.txt";
    //argv[2] = "textCheck.txt";
    //argv[3] = "dictionary.txt";
    //argv[4] = "3"; // n
    //argv[5] = "3"; // thresholod
    //argv[6] = "0.05" // delta
    //argv[7] = "1"; // model

    string textTrain  = argv[1];
    string textCheck  = argv[2];
    string dictionary = argv[3];
    unsigned int nGramSize = atoi(argv[4]);
    int threshold     = atoi(argv[5]);
    double delta      = atof(argv[6]);
    int model         = atoi(argv[7]);

    if (model == 0) {
        exit(EXIT_FAILURE);
    }

    unsigned int nGramSize = atoi(argv[1]);
    double delta = atof(argv[2]);
    int senLength = atoi(argv[3]);

    int vocabularySize = 256;
    bool latin_only = false;

    unsigned int N = 0;
    unordered_map<vector<char>, int> clm = getNgramsForAllN(textTrain, nGramSize, latin_only, N);

    for (int i = 0; i < numLanguages; i++) {
        // parse sentence
        // generate n grams from sentence
        // calculate probability of each n-gram in sentence using P_add_delta(ngram) = (count(n-gram)+delta)/(N+delta*|V|^n)
        // take log of probability and add to sum
        vector<T> tokensTest;
        read_tokens(langStr[i]+"2.txt", tokensTest, false);
        vector<vector<T>> sentences;
        unsigned int j = 0;
        while (j+senLength < tokensTest.size()) {
            vector<T> sentence;
            for (unsigned int k = j; k < j+senLength; k++) {
                sentence.push_back(tokensTest.at(k));
            }
            sentences.push_back(sentence);
            j += senLength;
        }

        for (unsigned int j = 0; j < sentences.size(); j++) {
            vector<T> sentence = sentences[j];
            vector<unordered_map<vector<T>, int>> nGrams; // maps for each 1-gram, 2-gram, etc.

            for (unsigned int n = 1; n <= nGramSize; n++) {
                unordered_map<vector<T>, int> nGramsOfn;
                for (unsigned int i = 0; i <= sentence.size() - n; i++) {
                    vector<T> nGram(n);   // for temporarily storing tokens to go into next n-gram

                    // Take next n tokens read from the input file
                    for (unsigned int j = 0; j < n; j++) // put next n tokens into vector temp
                        nGram[j] = sentence[i+j];

                    if (nGramsOfn.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
                        nGramsOfn[nGram] = 1;
                    else // nGram is already in the database, increase its count by 1
                        nGramsOfn[nGram] = nGramsOfn[nGram] + 1;
                }
                nGrams.push_back(nGramsOfn);
            }

            double probability[6] = {0};
            double bestProb = -DBL_MAX;
            int langIndex = 0;

            for (int lang = 0; lang < numLanguages; lang++) {
                for (unsigned int nn = 0; nn < nGrams.size(); nn++) {
                    unordered_map<vector<T>, int> nGramsOfn = nGrams[nn];
                    for (unordered_map<vector<T>, int>::iterator itr = nGramsOfn.begin(); itr != nGramsOfn.end(); ++itr) {
                        //P_add_delta(ngram) = (count(n-gram)+delta)/(N+delta*|V|^n)
                        if (clm.at(lang).count(itr->first)) {
                            probability[lang] +=
                                log(
                                (clm.at(lang)[itr->first] + delta)
                                /
                                (N[lang] + delta*pow(vocabularySize, nn))
                                );
                        } else {
                            probability[lang] +=
                                log(
                                (delta)
                                /
                                (N[lang] + delta*pow(vocabularySize, nn))
                                );
                        }
                    }
                }
            }
            bestProb = probability[langIndex];
            for (int lang = 1; lang < numLanguages; lang++) {
                if (probability[lang] > bestProb) {
                    bestProb = probability[lang];
                    langIndex = lang;
                }
            }

            cm[i][langIndex]++;


            //cout << "P(";
            //std::copy(sentence.begin(), sentence.end(), std::ostream_iterator<char>(std::cout, " ")); 
            //cout << ") = " << bestProb << " [" << langStr[langIndex] << "]" << endl;
        }
    }

    double totalSum = 0;
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 6; col++) {
            totalSum += cm[row][col];
        }
    }
    double diagonal = 0;
    for (int i = 0; i < 6; i++) {
        diagonal += cm[i][i];
    }
    printf("%.2f\n", 100*(totalSum - diagonal)/totalSum);

    // print confusion matrix.
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 6; col++) {
            cout << cm[row][col] << "\t";
        }
        cout << endl;
    }

    /*
    Padd1(ab) = C(ab)+1/(N+vocabularySize^2)
    */

    //getchar();

    return 0;
}
