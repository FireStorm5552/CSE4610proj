#include "filesys.h"


Filesys::Filesys(string diskname, int numberofblocks, int blocksize): Sdisk(diskname, numberofblocks, blocksize){
	rootsize = getblocksize()/12;
	fatsize = getnumberofblocks() * 5/getblocksize() + 1;
	string buffer;
	getblock(1,buffer);
	if(buffer[0] == '#'){
		buildfs();
		fssynch();
	}
	else readfs();
}

int Filesys::fsclose(){
	return 1;
}

int Filesys::newfile(string file){
	//create empty file
	//-1 if not exist
	//0 no room left
	//1 okay
	for (int i = 0; i < filename.size(); i++){
		if(filename[i] == file){
			cout << "File already exists.";
			return -1;
			}
		if (filename[i] == "XXXXXX") {
			filename[i] = file;
			firstblock[i] = 0;
			fssynch();
			cout << "Created new file: " << file <<".\n";
			return 1;
		}
	}
	cout << "No space left.\n";
	return 0;
}
int Filesys::rmfile(string file){
	return 1;
}

int Filesys::getfirstblock(string file){
	for (int i = 0; i < filename.size(); i++){
		if(filename[i] == file){
			return firstblock[i];
		}
	}	
	return -1;

}

int Filesys::addblock(string file, string buffer){
	int allocate = fat[0];
	for (int i = 0; i < 15; i++){
	cout << fat[i] << " ";
	}
	cout << endl;
	if (allocate <= 0){
		cout << "Error: Out of space on disk" << fat[0] <<endl;
		return 0;
	}

	int blockid = getfirstblock(file);
	cout << blockid;
	if(blockid == -1){
		cout << "File does not exist." << endl;
		return 0;
	}
	else if(blockid == 0){
		for (int i; i < filename.size(); i++){
			if(filename[i] == file){
				firstblock[i] = allocate;
				cout << filename[i] << " " << firstblock[i] << endl;
				break;
			}
		}
	}
	else{
		while (fat[blockid] != 0){
			blockid = fat[blockid];
		}
		fat[blockid] = allocate;
		}
		fat[0] = fat[fat[0]];
		fat[allocate] = 0;
		fssynch();
		putblock(allocate,buffer);
		return allocate;
	}
	
int Filesys::delblock(string file, int blocknumber){
	int b = getfirstblock(file);
	if (b == blocknumber){
		for (int i = 0; i < filename.size(); i++) {
        		if (filename[i] == file){
        			firstblock[i] = fat[firstblock[i]];
        			break;
			}
		}
	}
	else{
		while (fat[b] != blocknumber){
			b = fat[b];
		}
		fat[b] = fat[blocknumber];
	}
	fat[blocknumber] = fat[0];
	fat[0] = blocknumber;
	return 1;
}

int Filesys::readblock(string file, int blocknumber, string& buffer){
	if(fbcheck(file,blocknumber)){
		getblock(blocknumber,buffer);
		return 1;
	}
	else return 0;
}

int Filesys::fbcheck(string file,int blocknumber) {
	int b = getfirstblock(file);
	if (b < 0) return 0;
	while (b != 0){
		if (b == blocknumber) return 1;
    		else b = fat[b];
	}
	return 0;
}

int Filesys::writeblock(string file, int blocknumber, string buffer){
	if(fbcheck(file,blocknumber)){
		putblock(blocknumber,buffer);
		return 1;
	}
	else return 0;
}

int Filesys::nextblock(string file, int blocknumber){
	if (fbcheck(file,blocknumber))
		return fat[blocknumber];
	else return -1;
	}
vector<string> Filesys::ls()
{ vector<string> flist;
  for (int i=0; i<filename.size(); i++)
      {
        if (filename[i] != "XXXXX")
           {
             flist.push_back(filename[i]);
           }
      }
  return flist;
}

int Filesys::buildfs(){
	for(int i = 0; i < rootsize; i++){
		filename.push_back("XXXXXX");
		firstblock.push_back(0);
	}
	fat.push_back(2+fatsize);
	fat.push_back(-1);
	for(int i = 0; i < fatsize; i++){
		fat.push_back(-1);
	}
	for(int i = 2 + fatsize; i < getnumberofblocks();i++){
		fat.push_back(i+1);
	}
	fat.at(fat.size()-1) = 0;
	return 1;
}

int Filesys::readfs(){
	string buffer1, buffer2;  
	getblock(1, buffer1);    
  	for (int i = 0; i < fatsize; i++){ 
    		string b;
    		getblock(i+2, b); 
    		buffer2 += b;    
  	}
	cout << buffer1 << endl << buffer2 << endl;
  	istringstream instream1;
  	istringstream instream2;
  	instream1.str(buffer1);
  	instream2.str(buffer2);

  	filename.clear();
  	firstblock.clear();
  	for (int i = 0; i < rootsize; i++){
    	string f;
    	int b;
    	instream1 >> f >> b;
    	filename.push_back(f);
    	firstblock.push_back(b);
  	}

  	for (int i = 0; i < getnumberofblocks(); i++){
    		int k;
    		instream2 >> k;
    		fat.push_back(k);
  	}
  	cout << "File system read in.\n";
  	return 1;
}

int Filesys::fssynch(){
	//synch file system
	ostringstream outstream1;
	for(int i = 0; i < firstblock.size(); i++){
		outstream1 << filename[i] << " " << firstblock[i] << " ";
	}
	ostringstream outstream2;
	for(int i = 0; i < fat.size(); i++){
		outstream2 << fat[i] << " ";
	}
	string buffer1 = outstream1.str();
	string buffer2 = outstream2.str();
	vector<string> blocks1 = block(buffer1, getblocksize());
	vector<string> blocks2 = block(buffer2, getblocksize());
	putblock(1,blocks1.at(0));
	for(int i = 0; i < blocks2.size(); i++){
		putblock(fatsize+ i + 2, blocks2[i]);
	}
	return 1;

}

vector<string> block(string buffer, int b) {
// blocks the buffer into a list of blocks of size b
	vector<string> blocks;
	int numberofblocks=0;

	if (buffer.length() % b == 0) { 
		numberofblocks= buffer.length()/b;
   	}
	else { 
		numberofblocks= buffer.length()/b +1;
   	}

	string tempblock;
	for (int i=0; i<numberofblocks; i++) {
		tempblock=buffer.substr(b*i,b);
      		blocks.push_back(tempblock);
  	}

	int lastblock=blocks.size()-1;

	for (int i=blocks[lastblock].length(); i<b; i++) { 
		blocks[lastblock]+="#";
    	}
	return blocks;

}
