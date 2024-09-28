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

int main() {
    // Valid amino acid characters
    string aa = "ACDEFGHIKLMNPQRSTVWY";

    // Data structures to store length and frequency information
    unordered_map<char, int> length;
    map<string, int> freq;

    // File handling to read the CSV file
    ifstream file("protein_data3.csv");  // Open the CSV file
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
        
        // Read and ignore the first column (ID) 
        getline(ss, id, ',');  
        
        // Read the second column (sequence) and remove quotes
        getline(ss, seq, ',');  
        seq.erase(remove(seq.begin(), seq.end(), '\"'), seq.end()); // Remove quotes
        seq.erase(remove(seq.begin(), seq.end(), ' '), seq.end()); // Remove spaces if any

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
