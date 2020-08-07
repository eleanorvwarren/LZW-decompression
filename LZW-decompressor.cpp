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
    while(dec!=0) {
        if (dec%2==0) {
            b+="0";
        } else {
            b+="1";
        }
        dec/=2;
    }
    return b;
}
//converts binary to decimal
int binary_to_decimal(string bin) {
    int d = 0;
    for (int i = 0; i < bin.length(); i++) {
        if (bin.at(bin.length()-1-i)==*"0") {
            d += 0*(pow(2,i));
        } else if (bin.at(bin.length()-1-i)==*"1") {
            d += 1*(pow(2,i));
        }
    }
    return d;
}



//MAIN
int main() {

	string filename;
	//cout << "What file would you like to decompress?";
	//cin >> filename;
	ifstream file_in;
	file_in.open("LzwInputData/compressedfile1.z", fstream::in); // open file


	if (file_in.is_open()) { // file has opened successfully
    	cout << "Compressed file successfully opened.\n" << "Decompressing...\n";

    	char c;
    	string binary = "";
        int num_of_chars = 0;
		while (file_in.get(c)) { // read each character at a time and add the corresponding 8-bit binary to an array
    		binary += bitset<8>(decimal_to_binary(int(c))).to_string();
            num_of_chars ++;
    	}
    	file_in.close(); //close the file
        cout << "number of chars read in is " << num_of_chars << "\n";
        cout << "length of binary is " << binary.length() << "\n";
        cout << binary << "\n";

		// split binary into distinct codes
        string end_16 = "";
    	if (binary.size()%12 == 4) { // check if the last code will be padded into 16-bits
            end_16 = binary.substr(binary.length() - 16, string::npos); // separate out the last 16 bits
            binary.erase(binary.end()-16, binary.end()); 
            //cout << "new length is " << binary.length();
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
        for (int i = 0; i<chunks.size(); i++) {
            codes.push_back(binary_to_decimal(chunks[i]));
        }

        for (int i=0; i<chunks.size(); i++) {
            cout << chunks[i] << " ";
        }
        cout << "\n";
        for (int i=0; i<codes.size(); i++) {
            cout << codes[i] << " ";
        }




        // DECOMPRESSION PROCESS
        ofstream file_out;
        file_out.open ("LzwInputData/decompressedfile1.txt"); // the decompressed file to output

        // extended dictionary
        map<int,string> Dict;
        int dict_entries = 0;

        string output; // variable to store the last output to the file
        string prev_output;
        for (int i = 0; i < codes.size(); i++) {

            // output current code as a string to the file
            output = "";
            if (codes[i] <= 255) { // in initial dictionary (ASCII)
                output += static_cast<char>(codes[i]);
            } else { // in extended dictionary created by encoder
                if (Dict.count(codes[i]) == 1) { // code in our reconstructed extended dictionary 'Dict'
                    output += Dict[codes[i]];
                } else if (i>0 && Dict.count(codes[i]) == 0){ // code not yet in 'Dict' ie encoder only just added it to their dictionary
                    // know prev code in extended dictionary of form 'cS' c=char S=string
                    // this code must be 'cSc'
                    Dict[codes[i]] = Dict[codes[i-1]] + Dict[codes[i-1]][0]; // add code to 'Dict'
                    dict_entries++;
                    output += Dict[codes[i]];
                }
            }
            
            file_out << output;
            prev_output = output;

            // define new 'Dict' entry based on current and previous codes
            if (i>0) {
                Dict[dict_entries+256] = prev_output + output[0];
                dict_entries++;
            }



        }
        file_out.close();






  	} else { // file couldn't be opened
    cout << "Error opening file to decompress";
  	}
    return 0;
}