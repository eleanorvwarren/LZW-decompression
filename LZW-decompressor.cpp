//  LZW decompressor

#include <iostream>
#include <fstream>
#include <cstddef>
#include <cmath>
#include <vector>
#include <bitset>
#include <string>
#include <map>

using namespace std;

//FUNCTIONS
//converts decimal to binary
string decimal_to_binary(int dec) {
    string b;
    while(dec != 0) {
        if (dec%2 == 0) {
            b += "0";
        } else {
            b += "1";
        }
        dec /= 2;
    }
    reverse(b.begin(),b.end());
    return b;
}
//converts binary to decimal
int binary_to_decimal(string bin) {
    int d = 0;
    for (int i = 0; i < bin.length(); i++) {
        if (bin.at(bin.length()-1-i) == *"0") {
            d += 0*(pow(2,i));
        } else if (bin.at(bin.length()-1-i) == *"1") {
            d += 1*(pow(2,i));
        }
    }
    return d;
}

// clears additional entries from dictionary
void clear_Dict(map<unsigned int,string> &d, bool &s) {
	d.clear();
    s = true;
    for (int unsigned j = 0 ; j < 256 ; j++ ){
        d[j] = string(1,j);
    }
}


//MAIN
int main() {

	string filename;
	cout << "You are using Eleanor's magical LZW decompression algorithm. What file would you like to decompress? (with .z format)";
	cin >> filename;
	ifstream file_in;
	file_in.open(filename, fstream::in); // open file


	if (file_in.is_open()) { // file has opened successfully
    	cout << "Compressed file successfully opened.\n" << "Decompressing...\n";

    	char c;
    	string binary = "";
		while (file_in.get(c)) { // read each character at a time and add the corresponding 8-bit binary to a string
            if (int(c)<0) { // accounts for issues with int(c) for extended ASCII characters
                binary += bitset<8>(decimal_to_binary(int(c)+256)).to_string();
            } else {
                binary += bitset<8>(decimal_to_binary(int(c))).to_string();
            }
    	}

    	file_in.close(); //close the file

		// split binary into distinct codes
        string end_16 = "";
    	if (binary.size()%12 == 4) { // check if the last code will be padded into 16-bits
            end_16 = binary.substr(binary.length() - 16, string::npos); // separate out the last 16 bits
            binary.erase(binary.end()-16, binary.end()); 
    	} 
        vector<string> chunks; // vector to store separated 12(or 16) bit chunks of binary
        while(binary.length() > 0) {
            string nextChunk = binary.substr(0,12);
            chunks.push_back(nextChunk);
            binary = binary.substr(12, binary.length());
        }
        if (end_16 != "") chunks.push_back(end_16); // add the last 16-bit chunk if necessary

        // convert all binary chunks to decimal codes
        vector<int> codes;
        for (int i = 0; i < chunks.size(); i++) {
            codes.push_back(binary_to_decimal(chunks[i]));
        }

        for (int i=0; i<codes.size(); i++) {
            cout << codes[i] << "\n";
        }




        // DECOMPRESSION PROCESS
        ofstream file_out;
        file_out.open (filename.substr(0,filename.length()-2)+"_decompressed.txt"); // the decompressed file to output


        // Dictionary initialised with 0-255 ASCII characters
        map<unsigned int,string> Dict;
        for (int unsigned i = 0 ; i < 256 ; i++ ){
            Dict[i] = string(1,i);
        }
        int dict_entries = 256;



        string prev_output; // variable to store the last/current output in order to create new dictionary entries
        bool start = true; // bool to say whether decompression should 'start again' ie ignore previous outputs

        for (int i = 0; i < codes.size(); i++) {

			if (Dict.size() == 4096) { // Dictionary is full so reset it
				clear_Dict(Dict, start);
				dict_entries = 256;
            }

            // output current code as a string to the file
            if (Dict.count(codes[i]) == 1) { // code in our reconstructed Dictionary
                file_out << Dict[codes[i]];
            } else if (!start && Dict.count(codes[i]) == 0){ // code not yet in 'Dict' ie encoder only just added it to their dictionary
                // know prev code in extended dictionary of form 'cS' c=char S=string, and this code must be 'cSc'
                Dict[codes[i]] = Dict[codes[i-1]] + Dict[codes[i-1]][0]; // add code to 'Dict'
                dict_entries++;
                file_out << Dict[codes[i]];
                }

            // define new 'Dict' entry based on current and previous codes
			if (Dict.size() == 4096) { // Dictionary is full so reset it
				clear_Dict(Dict, start);
				dict_entries = 256;
            }
			if (!start) {
                Dict[dict_entries] = prev_output + Dict[codes[i]][0];
                dict_entries++;
                file_out << Dict[dict_entries];
            }

            prev_output = Dict[codes[i]];
            start = false;

        }
        file_out.close();


  	} else { // file couldn't be opened
    cout << "Error opening file to decompress";
  	}
    return 0;
}