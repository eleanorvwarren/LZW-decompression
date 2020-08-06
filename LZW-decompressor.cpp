//  LZW decompressor

#include <iostream>
#include <fstream>
#include <cstddef>
#include <vector>
#include <bitset>

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



//MAIN
int main() {

	string filename;
	cout << "What file would you like to decompress?";
	cin >> filename;
	fstream file;
	file.open(filename, fstream::in); // open file


	if (file.is_open()) { // file has opened successfully
    	cout << "Compressed file successfully opened.\n" << "Decompressing...\n";

    	char c;
    	string binary;
		while (file.get(c)) { // read each character at a time and add the corresponding 8-bit binary to an array
    		binary += bitset<8>(decimal_to_binary(int(c))).to_string();
    	}
    	file.close(); //close the file

		// read binary in 12-bit chunks
    	cout << binary << "\n";



  	} else { // file couldn't be opened
    cout << "Error opening file to decompress";
  	}
	

// if its a multiple of 3 bytes then its normal, but if its something*3 + 2 bytes then the final 2 bytes are 1 code padded into 16 bits
    
    
    return 0;
}