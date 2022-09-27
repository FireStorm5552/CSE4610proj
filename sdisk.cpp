#include <fstream>
#include <string>
#include <iostream>
#include "sdisk.h"

using namespace std;

Sdisk::Sdisk(string diskname, int numberofblocks, int blocksize){
this->diskname = diskname;
this->numberofblocks = numberofblocks;
this->blocksize = blocksize;
ifstream indisk;
indisk.open(diskname.c_str());
if (indisk.fail()){
	indisk.close();
	ofstream outdisk;
	outdisk.open(diskname.c_str());
	for (int i = 0; i < numberofblocks * blocksize; i++){
		outdisk.put('#');
	}
}
else{
	int counter = 0;
	char c;
	
	indisk.get(c);
	while(indisk.good()){
		counter ++;
		indisk.get(c);
	}
	if(counter == numberofblocks*blocksize){
	cout << "Disk is formatted correctly" << endl;
	return;
	}
}
}

int Sdisk::getblock(int blocknumber, string& buffer){
	fstream iofile;
	iofile.open(diskname.c_str(), ios::in | ios::out);
	buffer.clear();
	iofile.seekg(blocknumber*blocksize);
	char c;
	iofile.get(c);
	for(int i = 0; i < blocksize; i++){
		buffer.push_back(c);
		iofile.get(c);
		}
	iofile.close();
	return 1;
}

int Sdisk::putblock(int blocknumber, string buffer){
	fstream iofile;
	iofile.open(diskname.c_str(), ios::in | ios::out);
	if( blocknumber < 0 | blocknumber >= numberofblocks){
		cout << "error" << endl;
		return 0;
		}
	iofile.seekp(blocksize*blocknumber);
	for(int i = 0; i < blocksize; i++){
		iofile.put(buffer.at(i));
		}
	iofile.close();
	return 1;
}

int Sdisk::getnumberofblocks(){
	return numberofblocks;
}

int Sdisk::getblocksize(){
	return blocksize;
}
