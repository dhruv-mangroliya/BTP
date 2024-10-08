#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <fstream>   // For file I/O
#include <sstream>   // For string stream to parse CSV lines
#include <xlsxwriter.h>   // Library to write Excel files
#include <filesystem>  // For directory traversal (C++17 feature)

using namespace std;
namespace fs = std::filesystem;

// Function to solve the frequency of repeated amino acids in the protein sequence
void solve(string protein, map<string, int>& freq) {
    int n = protein.size();

    for (int i = 0; i < n; i++) {
        char curr = protein[i];
        string repeat = "";
        while (i < n && curr == protein[i]) {
            repeat += protein[i];
            i++;
        }
        if (repeat.size() > 1) {  // Only consider repeats of length > 1
            freq[repeat]++;
        }
        i--;  // Move back to the last character of the current repeat
    }
}

int main() {
    // Initialize Excel workbook
    lxw_workbook  *workbook  = workbook_new("protein_homorepeat_results.xlsx");
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);

    // Write headers: Entry ID, Protein Name, and Homorepeats (common for all CSV files)
    int row = 0;  // Start at row 0 for the first CSV
    int col = 1;  // Starting column (column 0 is for protein names)
    int csv_counter = 1; // Counter to keep track of CSV files processed

    // Iterate over all CSV files in the same directory
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        if (entry.path().extension() == ".csv") {
            string filename = entry.path().filename().string(); // Get only the filename without path
            cout << "Processing file: " << filename << endl;

            // File handling to read the CSV file
            ifstream file(entry.path());  // Open the CSV file using the path
            if (!file.is_open()) {
                cout << "Error: Could not open the file: " << filename << endl;
                continue;
            }

            // To store the sequences and corresponding protein names
            vector<tuple<string, string, string>> sequences;  // {entry_id, protein_name, sequence}
            string line;

            // Skip the first line (header) if present
            getline(file, line);  // Assuming the first line is the header

            // Read the sequences from the CSV file
            while (getline(file, line)) {
                stringstream ss(line);
                string id, name, seq;

                // Read the first column (ID)
                getline(ss, id, ',');  
                // Read the second column (Protein name)
                getline(ss, name, ',');  
                // Read the third column (sequence) and remove quotes
                getline(ss, seq, ',');  
                seq.erase(remove(seq.begin(), seq.end(), '"'), seq.end()); // Remove quotes
                seq.erase(remove(seq.begin(), seq.end(), ' '), seq.end()); // Remove spaces if any

                sequences.push_back(make_tuple(id, name, seq));  // Store entry ID with name and sequence
            }

            file.close();  // Close the file after reading

            // Map to store unique homorepeats across all proteins in this file
            set<string> homorepeats;
            for (const auto& tuple : sequences) {
                const string& protein = get<2>(tuple);  // Get sequence
                map<string, int> freq;
                solve(protein, freq);  // Analyze the protein sequence

                for (const auto& i : freq) {
                    homorepeats.insert(i.first);  // Insert unique homorepeats
                }
            }

            // Write headers for this CSV file (Entry ID, Protein Name, and Homorepeats)
            worksheet_write_string(worksheet, row, 0, ("Results from " + filename).c_str(), NULL);
            row++;
            worksheet_write_string(worksheet, row, 0, "Entry ID", NULL);
            worksheet_write_string(worksheet, row, 1, "Protein Name", NULL);
            col = 2;  // Start from the third column for homorepeats
            for (const string& repeat : homorepeats) {
                worksheet_write_string(worksheet, row, col++, repeat.c_str(), NULL);
            }

            // Write each protein and its corresponding repeat counts
            row++;  // Move to the next row for data
            for (const auto& tuple : sequences) {
                const string& entry_id = get<0>(tuple);  // Get entry ID
                const string& protein_name = get<1>(tuple);  // Get protein name
                const string& protein = get<2>(tuple);      // Get sequence

                // Map to store frequency of amino acid repeats
                map<string, int> freq;
                solve(protein, freq);  // Analyze the protein sequence

                // Write entry ID and protein name
                worksheet_write_string(worksheet, row, 0, entry_id.c_str(), NULL);
                worksheet_write_string(worksheet, row, 1, protein_name.c_str(), NULL);

                // Write homorepeat frequencies for this protein
                col = 2;  // Reset column for homorepeats
                for (const string& repeat : homorepeats) {
                    if (freq.find(repeat) != freq.end()) {
                        worksheet_write_number(worksheet, row, col, freq[repeat], NULL);
                    } else {
                        worksheet_write_number(worksheet, row, col, 0, NULL);  // No repeats of this type
                    }
                    col++;
                }

                row++;  // Move to the next row for the next protein
            }

            // Add a few empty rows to separate this result from the next file
            row += 3;  // You can adjust the number of blank rows
            csv_counter++;  // Increment the file counter
        }
    }

    // Close the workbook and save
    workbook_close(workbook);

    cout << "All CSV files processed. Excel results saved as 'protein_homorepeat_results.xlsx'." << endl;

    return 0;
}
