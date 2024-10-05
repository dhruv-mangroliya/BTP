#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <fstream>   // For file I/O
#include <sstream>   // For string stream to parse CSV lines
#include <hpdf.h>    // Haru PDF Library

using namespace std;

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

void writeToPDF(HPDF_Page& page, const string& entry_id, const string& protein_name, const map<string, int>& freq, float& text_pos_y) {
    // Write protein entry ID and name to PDF
    string header = "Entry: " + entry_id + ", Protein: " + protein_name;
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, text_pos_y, header.c_str());
    HPDF_Page_EndText(page);
    text_pos_y -= 20;

    // Write repeated sequences
    for (const auto& i : freq) {
        string aa = i.first;
        string text = aa + ": " + to_string(i.second) + " times repeated";
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, 50, text_pos_y, text.c_str());
        HPDF_Page_EndText(page);
        text_pos_y -= 20;
    }

    // Add a separator
    HPDF_Page_BeginText(page);
    HPDF_Page_TextOut(page, 50, text_pos_y, "-----------------------");
    HPDF_Page_EndText(page);
    text_pos_y -= 30;
}

int main() {
    string filename;
    cout << "Enter the CSV filename (including path if not in current directory): ";
    getline(cin, filename);  // Use getline to allow for spaces in the filename

    // File handling to read the CSV file
    ifstream file(filename);  // Open the CSV file
    if (!file.is_open()) {
        cout << "Error: Could not open the file!" << endl;
        return 1;
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

    // Initialize PDF document
    HPDF_Doc pdf = HPDF_New(NULL, NULL);
    if (!pdf) {
        cout << "Error: Cannot create PDF file." << endl;
        return 1;
    }

    // Add a new page to the PDF document
    HPDF_Page page = HPDF_AddPage(pdf);
    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

    // Set font and text size
    HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);
    HPDF_Page_SetFontAndSize(page, font, 12);

    float page_height = HPDF_Page_GetHeight(page);
    float text_pos_y = page_height - 40;  // Start writing from the top of the page

    // Loop through sequences and add content to the PDF
    for (const auto& tuple : sequences) {
        const string& entry_id = get<0>(tuple);  // Get entry ID
        const string& protein_name = get<1>(tuple);  // Get protein name
        const string& protein = get<2>(tuple);      // Get sequence

        // Map to store frequency of amino acid repeats
        map<string, int> freq;
        solve(protein, freq);  // Analyze the protein sequence

        writeToPDF(page, entry_id, protein_name, freq, text_pos_y);

        // Check if we need a new page
        if (text_pos_y < 50) {
            page = HPDF_AddPage(pdf);
            HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
            HPDF_Page_SetFontAndSize(page, font, 12);
            text_pos_y = page_height - 40;  // Reset position for new page
        }
    }

    // Save the PDF to a file
    HPDF_SaveToFile(pdf, "protein_repeat_results.pdf");

    // Clean up and close PDF
    HPDF_Free(pdf);

    cout << "PDF results saved as 'protein_repeat_results.pdf'. You can download it now." << endl;

    return 0;
}

