#include <iostream> 
#include <fstream> 
#include <string>
#include<iostream>
#include <math.h>

using namespace std;
/*
--- Please use C++ 11 when you Compile in Venus  ---g++ -std=gnu++11 Histogram.cpp -o histogram.out
*/
class image {
private:
	int numRows;
	int numCols;
	int minVal;
	int maxVal;
	int **imageAry; // 2D Array, size = numRows+2 by numCols+2.
	int **CCAry;// 2D Array, size = numRows+2 by numCols+2.	
	
public:
	friend class connectCC;
	friend class ChainCode;
	image(char **File) {
		fstream finput;
		finput.open(File[1]);
		if (!finput.is_open()) {
			throw "Error ! Open Input File Failed, try again...";
		}
		if (!finput.eof() && finput.peek() != EOF) {
			finput >> numRows;
			finput >> numCols;
			finput >> minVal;
			finput >> maxVal;
		}
		else {
			finput.close();
			throw "Error ! Input File is empty, try again...";
		}
		finput.close();
		prepareOutput(File[3],File[4]);

		imageAry = new int*[numRows + 2];
		CCAry = new int*[numRows + 2];
		for (int i = 0; i < numRows + 2; i++) {
			imageAry[i] = new int[numCols + 2];
			CCAry[i] = new int[numCols + 2];
			for (int j = 0; j < numCols + 2; j++) {
				imageAry[i][j] = 0;
				CCAry[i][j] = 0;
			}
		}

		loadImage(File[1]);
		prepareOutput(File[3],File[4]);

	}
	~image() {
		for (int i = 0; i < numRows + 2; i++) {
			delete[] imageAry[i];
			delete[] CCAry[i];
		}
		delete[] imageAry;
		delete[] CCAry;
	}

	void zeroFramed() {
	}

	void loadImage(string inFile) {
		//Read the original image
		fstream finput;
		finput.open(inFile);
		if (!finput.is_open()) {
			throw "Error ! Open Input File Failed, try again...";
		}
		if (!finput.eof() && finput.peek() != EOF) {
			string temp;
			getline(finput, temp); // skip the image head
			int gVal = 0;
			for (int i = 1; i < numRows + 1; i++) {
				for (int j = 1; j < numCols + 1; j++) {
					if (!finput.eof() && finput.peek() != EOF) {
						finput >> gVal;
					}
					else { break; }
					imageAry[i][j] = gVal;
				}
			}
		}
		else {
			finput.close();
			throw "Error ! Input File is empty, try again...";
		}
		finput.close();
		cout << "Finish Loading Original Image." << endl;
	}

	void prepareOutput(string out1, string out2) {
		fstream foutput1;
		fstream foutput2;
		foutput1.open(out1, ios::out);
		foutput2.open(out2, ios::out);
		foutput1 << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
		foutput2 << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
		foutput1.close();
		foutput2.close();
	}
};

class connectCC {
private:
	int Label;
	int numbpixels;
	int minRow;
	int minCol;
	int maxRow;
	int maxCol;
public:
	friend class ChainCode;
	connectCC(int lb, int num, int minr, int minc, int maxr, int maxc) {
		Label = lb;
		numbpixels = num;
		minRow = minr;
		minCol = minc;
		maxRow = maxr;
		maxCol = maxc;
	}
	
	void loadCC(image* img)	//(int ** CCAry,int ** imgAry)
	{
		for (int i = minRow; i <= maxRow; i++) {
			for (int j = minCol; j <= maxCol; j++) {
				if (img->imageAry[i][j] == Label) {
					img->CCAry[i][j] = img->imageAry[i][j];	// take labeled part out of the image
				}
			}
		}
	}

	void PrettyPrity(image* img) {
		// Debugging print
		cout << "#Label:" << Label << " #Pixels:" << numbpixels << " minRow:" << minRow << " minCol:" << minCol << " maxRow:" << maxRow << " maxCol:" << maxCol << endl;
		for (int i = 1; i < img->numRows + 1; i++) {
			for (int j = 1; j < img->numCols + 1; j++) {
				if (img->CCAry[i][j] > 0) {
					if (img->CCAry[i][j] < 10) {
						cout << img->CCAry[i][j] << "  ";
					}
					else {
						cout << img->CCAry[i][j] << " ";
					};
				}
				else {
					cout << "   ";
				}
			}
			cout << endl;
		}
	}
};



class ChainCode {
private:
	class point {
	private:
		int row;
		int col;
	public:
		point(int i, int j) {
			row = i;
			col = j;
		}
		point() {
			row = 0;
			col = 0;
		}
		void setPoint(int i, int j) {
			row = i;
			col = j;
		}
		int getR() { return row; };
		int getC() { return col; };
	};

	point pt;
	point nbCoord[8];//neighborCoord[]
	point currentP;
	point nextP;
	int lastQ;
	//int nextQ;
	int nextDirTable[8];
	int nextDir;
	int PchainDir;

public:
	ChainCode() {
		lastQ = 4;
		nextDirTable[0] = 6;
		nextDirTable[1] = 0;
		nextDirTable[2] = 0;
		nextDirTable[3] = 2;
		nextDirTable[4] = 2;
		nextDirTable[5] = 4;
		nextDirTable[6] = 4;
		nextDirTable[7] = 6;
	}
	~ChainCode() {

	}

	void loadNeighborsCoord() {
		nbCoord[0].setPoint(currentP.getR(), currentP.getC()+1);
		nbCoord[1].setPoint(currentP.getR()-1, currentP.getC()+1);
		nbCoord[2].setPoint(currentP.getR()-1, currentP.getC());
		nbCoord[3].setPoint(currentP.getR()-1, currentP.getC()-1);
		nbCoord[4].setPoint(currentP.getR(), currentP.getC()-1);
		nbCoord[5].setPoint(currentP.getR()+1, currentP.getC()-1);
		nbCoord[6].setPoint(currentP.getR()+1, currentP.getC());
		nbCoord[7].setPoint(currentP.getR()+1, currentP.getC()+1);
	}

	int findNextP(point* currP, int nextQ, point* nextP, image * img, int label) {
		loadNeighborsCoord();	
		//scan currentP's 8 neighbors within nighborCoord[] array from nextQ direction (mod 8)
		for (int i = nextQ; i < nextQ+8; i++) { 
			if (abs(img->CCAry[nbCoord[i%8].getR()][nbCoord[i%8].getC()]) == label) {
				//Set nextP
				nextP->setPoint(nbCoord[i % 8].getR(), nbCoord[i % 8].getC());
				return i % 8;
			}
		}
	}

	void prettyPrint() {
	}

	void getChainCode(connectCC* CC, image* img, string out1, string out2) {
		//getChainCode Setp1
		int minRow, minCol, maxRow, maxCol, Label, pixels;
		minRow = CC->minRow;
		minCol = CC->minCol;
		maxRow = CC->maxRow;
		maxCol = CC->maxCol;
		Label = CC->Label;
		pixels = CC->numbpixels;
		//open output file
		fstream foutput1;
		fstream foutput2;
		foutput1.open(out1, ios::out | ios::app);
		foutput2.open(out2, ios::out | ios::app);
		cout << Label;
		foutput1 << Label ; // output label
		foutput2 << Label ; // output label

		//getChainCode Step2
		if (pixels == 1) {	// if the CC only has 1 pixels , directly output it .
			cout << " " << minRow << " " << minCol << endl << endl;
			foutput1 << " " << minRow << " " << minCol << endl;
			foutput2 << " " << minRow << " " << minCol << endl;
			return;
		}
		for (int i = minRow; i <= maxRow; i++) {
			for (int j = minCol; j <= maxCol; j++) {		
				//getChainCode Step3
				if (img->CCAry[i][j] > 0 && img->CCAry[i][j] == Label) {
					pt.setPoint(i, j);	// record the start point
					currentP.setPoint(i, j);
					lastQ = 4;	
					
					//output
					cout << " " << i << " " << j << endl;
					foutput1 << " " << i << " " << j ; // output start point
					foutput2 << " " << i << " " << j << endl; // output start point
					int printCounter = 0;
					
					while(true){
						//getChainCode Step4 (Ensure lastQ keeps in range 0-7)
						lastQ = ++lastQ % 8;
						//getChainCode Step5 (Get nextP and flip the point)
						PchainDir = findNextP(&currentP, lastQ , &nextP, img, Label);
						img->CCAry[currentP.getR()][currentP.getC()] = -1 * img->CCAry[currentP.getR()][currentP.getC()];
						//getChainCode Step6
						printCounter++;
						cout << PchainDir << " ";
						foutput1 << " " << PchainDir ;	
						foutput2 << PchainDir << " ";
						if (printCounter % 20 == 0) {
							cout << endl;
							foutput2 << endl;
						}
						//getChainCode Step7 (Ensure nextDirTable[] keeps in range 0-7)
						lastQ = nextDirTable[(PchainDir+8-1)%8];
						currentP.setPoint(nextP.getR(), nextP.getC());
						//check if meet the start point
						if ((currentP.getC() == pt.getC()) && (currentP.getR() == pt.getR())) break;
					}
					cout << endl << endl;
					foutput1 << endl;
					foutput2 << endl;
					return;

				}// end of IF (step3)
			}// end of for loop (j)
		}// end of for loop (i)
		foutput1.close();
		foutput2.close();
	}// end of getChainCode function
};

int main(int argc, char *argv[])
{
	try {
		if (argc != 5) {
			cout << "Parameters missing... ( Run as : ChainCode <input file1> <input file2> <output file1> <output file2> ).";
			return 0;
		}
		//Main Step1
		image img(argv);
		ChainCode ChainCode;

		//Read the properties image
		fstream finput;
		finput.open(argv[2]);
		if (!finput.is_open()) {
			throw "Error ! Open Input File Failed, try again...";
		}
		if (!finput.eof() && finput.peek() != EOF) {
			string temp;
			getline(finput, temp); // skip the image head
			int numOfCC = 0;
			int lb = 0, num = 0, minr = 0, minc = 0, maxr = 0, maxc = 0;
			finput >> numOfCC;
			for (int i = 1; i <= numOfCC; i++) {			
				finput >> lb;
				finput >> num;
				finput >> minr;
				finput >> minc;
				finput >> maxr;
				finput >> maxc;
				//Main Step2
				connectCC CC(lb, num, minr, minc, maxr, maxc);	
				CC.loadCC(&img);
				ChainCode.getChainCode(&CC, &img, argv[3], argv[4]);
			}
		}
		else {
			finput.close();
			throw "Error ! Input File is empty, try again...";
		}
		finput.close();
	}
	catch (const char* msg) {
		cerr << msg << endl;
		return 0;
	}
	return 0;
};


