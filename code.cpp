#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <fstream>   // For file I/O
#include <sstream>   // For string stream to parse CSV lines

using namespace std;

// Function to solve the frequency of repeated amino acids in the protein sequence
void solve(string protein, unordered_map<char, int>& length, map<string, int>& freq) {
    int n = protein.size();

    for (int i = 0; i < n; i++) {
        char curr = protein[i];
        int len = 0;
        string repeat = "";
        while (i < n && curr == protein[i]) {
            repeat += protein[i];
            i++;
            len++;
        }
        freq[repeat]++;
        i--;
    }
    return;
}

// Function to check if the sequence only contains valid amino acids
bool checkSequence(string protein, string aa) {
    for (int i = 0; i < protein.size(); i++) {
        bool ans = false;
        for (int j = 0; j < aa.size(); j++) {
            if (protein[i] == aa[j]) {
                ans = true;
            }
        }
        if (!ans) return false;
    }
    return true;
}

int main() {
    // Valid amino acid characters
    string aa = "ACDEFGHIKLMNPQRSTVWY";

    // Data structures to store length and frequency information
    unordered_map<char, int> length;
    map<string, int> freq;

    // File handling to read the CSV file
    ifstream file("protein_data.csv");  // Open the CSV file
    if (!file.is_open()) {
        cout << "Error: Could not open the file!" << endl;
        return 1;
    }

    // To store the sequences
    vector<string> sequences;
    string line, protein;

    // Skip the first line (header) if present
    getline(file, line);  // Assuming the first line is the header

    // Read the sequences from the CSV file
    while (getline(file, line)) {
        stringstream ss(line);
        string id, seq;
        getline(ss, id, ',');   // First column is ID (ignore)
        getline(ss, seq, ',');  // Second column is the protein sequence
        sequences.push_back(seq);
    }
    file.close();  // Close the file after reading

    int t = sequences.size();  // Total number of protein sequences
    int curr = 0;
    bool print = true;

    // Loop through each protein sequence
    while (curr < t) {
        protein = sequences[curr];
        curr++;

        // Validate the protein sequence
        if (checkSequence(protein, aa) == false) {
            cout << "Wrong Protein Sequence Inserted. Program Terminated." << endl;
            print = false;
            break;
        }

        // Analyze the protein sequence
        solve(protein, length, freq);
    }

    // If all sequences are valid, print the frequency of repeated subsequences
    if (print == true) {
        for (auto i : freq) {
            string aa = i.first;

            // Ignore single-character repeats
            if (aa.size() == 1) continue;

            cout << aa << ": " << freq[aa] << " times repeated" << endl;
        }
    }

    return 0;
}
