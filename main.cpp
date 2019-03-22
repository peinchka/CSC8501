// Author: Edward James Williams, Student Number: 170532613

#include <iostream>   //To enable screen input and output
#include <sstream>    //To convert string to int
#include <fstream>    //To enable file input and output
#include <stdexcept>  //To handle exceptions
#include <direct.h>   //To create directories
#include <ctime>      //To improve randomisation of numbers
#include <string>     //To enable the use of strings
#include <windows.h>  //To enable the use of Windows-specific types (e.g. DWORD)
#include <vector>     //To enable the (extensive) use of vectors
#include <io.h>       //To enable the use of the file access function
#include <chrono>	  //To enable the calculation of the duration of events

using namespace std;

struct Lock {
	int CLHN[3][4];  //Array to store CN, LN and HN data
};

void createKeyFolder() {
	string dir = ".\\Key files";
	DWORD x = GetFileAttributesA(dir.c_str());
	if (x == INVALID_FILE_ATTRIBUTES) {
		_mkdir(".\\Key files");
	}
}

void createMSFolder() {
	string dir = ".\\Multi-safe files";
	DWORD x = GetFileAttributesA(dir.c_str());
	if (x == INVALID_FILE_ATTRIBUTES) {
		_mkdir(".\\Multi-safe files");
	}
}

void createLSFolder() {
	string dir = ".\\Locked safe files";
	DWORD x = GetFileAttributesA(dir.c_str());
	if (x == INVALID_FILE_ATTRIBUTES) {
		_mkdir(".\\Locked safe files");
	}
}

void fixInput(string x, int& s, int lower, int upper, string message) {
	istringstream iss(x);
	s = 0;
	iss >> s;
	while ((s < lower) || (s > upper)) {
		cout << message;
		cin >> x;
		istringstream iss(x);
		iss >> s;
	}
}

void createFilename(string& file_name) {
	int i = 1;
	string test_file_name;
	while (i <= 9999999) {
		test_file_name = file_name + to_string(i) + ".txt";
		if (_access_s(test_file_name.c_str(), 0) != 0) {
			break;
		}
		i++;
	}
	file_name += to_string(i) + ".txt";
}

void hasher(int* num, const vector<int>& hash, int* out) {
	for (int i = 0; i < 4; i++) {
		out[i] = num[i] + hash[i];
		if (out[i] > 9) out[i] -= 10;
		if (out[i] < 0) out[i] += 10;
	}
}

bool checkNoRepeat(int* CN) {
	if (CN[0] == CN[1] || CN[0] == CN[2] || CN[0] == CN[3] || CN[1] == CN[2] || CN[1] == CN[3] || CN[2] == CN[3]) {
		return false;
	}
	return true;
}

bool checkSum(int* CNL, int* CNR) {
	int sumleft = CNL[0] + CNL[1] + CNL[2] + CNL[3];
	int sumright = CNR[0] + CNR[1] + CNR[2] + CNR[3];
	if (sumleft >= sumright) {
		return false;
	}
	return true;
}

void writeKeyFile(vector<vector<vector<int>>>& keys, int numoflocks) {
	string keyfile = ".\\Key files\\Key file ";
	createFilename(keyfile);
	ofstream keydata;
	keydata.open(keyfile.c_str());
	if (numoflocks != 5) {
		keydata << "LOCKS " << numoflocks << "\n";
	}
	keydata << "NS " << keys.size();
	for (int j = 0; j < (int)keys.size(); j++) {
		keydata << "\nROOT " << keys[j][3][0] << keys[j][3][1] << keys[j][3][2] << keys[j][3][3];
		keydata << "\nUHF ";
		for (int i = 0; i < 4; i++) {
			if (keys[j][0][i] < 0)
				keydata << keys[j][0][i];
			else
				keydata << '+' << keys[j][0][i];
			if (i != 3)
				keydata << ',';
		}
		keydata << "\nLHF ";
		for (int i = 0; i < 4; i++) {
			if (keys[j][1][i] < 0)
				keydata << keys[j][1][i];
			else
				keydata << '+' << keys[j][1][i];
			if (i != 3)
				keydata << ',';
		}
		keydata << "\nPHF ";
		for (int i = 0; i < 4; i++) {
			if (keys[j][2][i] < 0)
				keydata << keys[j][2][i];
			else
				keydata << '+' << keys[j][2][i];
			if (i != 3)
				keydata << ',';
		}
	}
	keydata.close();
}

void produceKey() {
	srand((unsigned int)time(NULL)); //Random seed is cast to unsigned integer to remedy type-mismatch warning
	int totalsum;
	int bestkey = 0;
	vector<int> hashnum(4, 0);
	vector<vector<int>> ULPHF;
	ULPHF.push_back(hashnum);
	ULPHF.push_back(hashnum);
	ULPHF.push_back(hashnum);
	ULPHF.push_back(hashnum);
	cout << "Enter the number of combination locks: ";
	int numoflocks;
	string stringlocks;
	cin >> stringlocks;
	fixInput(stringlocks, numoflocks, 1, 9, "Invalid input! Please enter an integer from 1 to 9: ");
	vector<int> sum(numoflocks, 0);
	vector<Lock> multilock;
	for (int i = 1; i <= numoflocks; i++) {
		Lock x;
		multilock.push_back(x);
	}
	cout << "Enter the number of combinations of hash function inputs for which to calculate all possible roots: ";
	int loops;
	string stringloops;
	cin >> stringloops;
	fixInput(stringloops, loops, 1, 100000000, "Invalid input! Please enter an integer from 1 to 100000000: ");
	cout << "\n";
	auto start = chrono::system_clock::now();
	int loopnum = 0;
	int test;
	while (loopnum < loops) {
		vector<vector<vector<int>>> hashrootsolution;
		loopnum += 1;
		int root[4];
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				ULPHF[i][j] = rand() % 10;
			}
		}
		while (ULPHF[1][0] == 0 && ULPHF[1][1] == 0 && ULPHF[1][2] == 0 && ULPHF[1][3] == 0) {  //LHF inputs can't be "0, 0, 0, 0", otherwise LN = CN, which is forbidden
			for (int j = 0; j < 4; j++) {
				ULPHF[1][j] = rand() % 10;
			}
		}
		test = 0;
		for (int r1 = 0; r1 < 10; r1++) {
			root[0] = r1;
			for (int r2 = 0; r2 < 10; r2++) {
				root[1] = r2;
				for (int r3 = 0; r3 < 10; r3++) {
					root[2] = r3;
					for (int r4 = 0; r4 < 10; r4++) {
						root[3] = r4;
						totalsum = 0;
						for (int i = 0; i < numoflocks; i++) {
							if (i == 0) {
								hasher(root, ULPHF[0], multilock[i].CLHN[0]);
							}
							else {
								hasher(multilock[i - 1].CLHN[2], ULPHF[0], multilock[i].CLHN[0]);
							}
							if (!checkNoRepeat(multilock[i].CLHN[0])) goto _nextroot;  //Checking that CN contains no repeating numbers
							sum[i] = multilock[i].CLHN[0][0] + multilock[i].CLHN[0][1] + multilock[i].CLHN[0][2] + multilock[i].CLHN[0][3];
							if (i > 0) {
								if (sum[i] <= sum[i - 1]) goto _nextroot;  //Checking that CN of LH lock is smaller than that of RH lock
							}
							hasher(multilock[i].CLHN[0], ULPHF[1], multilock[i].CLHN[1]);  //Calculating LN
							hasher(multilock[i].CLHN[1], ULPHF[2], multilock[i].CLHN[2]);  //Calculating HN
							totalsum += sum[i];
						}
						if (totalsum % 2 != 0) continue;  //Checking that sum of all unlock numbers on all locks is even

						if (test == 0) {
							cout << "\n";
							cout << "UHF " << ULPHF[0][0] << ", " << ULPHF[0][1] << ", " << ULPHF[0][2] << ", " << ULPHF[0][3] << "\n";
							cout << "LHF " << ULPHF[1][0] << ", " << ULPHF[1][1] << ", " << ULPHF[1][2] << ", " << ULPHF[1][3] << "\n";
							cout << "PHF " << ULPHF[2][0] << ", " << ULPHF[2][1] << ", " << ULPHF[2][2] << ", " << ULPHF[2][3] << "\n\n";
							test = 1;
						}

						cout << "ROOT " << root[0] << root[1] << root[2] << root[3] << "\n";
						for (int i = 0; i < numoflocks; i++) {
							cout << "CN" << i << ' ' << multilock[i].CLHN[0][0] << multilock[i].CLHN[0][1] << multilock[i].CLHN[0][2] << multilock[i].CLHN[0][3] << ", ";
							cout << "LN" << i << ' ' << multilock[i].CLHN[1][0] << multilock[i].CLHN[1][1] << multilock[i].CLHN[1][2] << multilock[i].CLHN[1][3] << ", ";
							cout << "HN" << i << ' ' << multilock[i].CLHN[2][0] << multilock[i].CLHN[2][1] << multilock[i].CLHN[2][2] << multilock[i].CLHN[2][3] << "\n";
						}
						cout << "Sum of CN numbers per lock = ";
						for (int i = 0; i < numoflocks; i++) {
							if (i != numoflocks - 1) {
								cout << sum[i] << " < ";
							}
							else {
								cout << sum[i] << "\n";
							}

						}
						cout << "Sum of CN numbers across all locks = " << totalsum << "\n\n";
						for (int i = 0; i < 4; i++) {
							ULPHF[3][i] = root[i];
						}
						hashrootsolution.push_back(ULPHF);
					_nextroot:;
					}
				}
			}
		}
		if (int(hashrootsolution.size()) > bestkey)
			bestkey = int(hashrootsolution.size());
		if (hashrootsolution.size() != 0)
			writeKeyFile(hashrootsolution, numoflocks);
	}
	cout << "Best solution has " << bestkey << " roots\n\n";
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds = end - start;
	cout << "Elapsed time: " << elapsed_seconds.count() << "s\n\n";
}

void writeMSFile(vector<vector<vector<int>>>& hashrootsolution, int numoflocks) {
	vector<Lock> multilock;
	for (int i = 1; i <= numoflocks; i++) {
		Lock x;
		multilock.push_back(x);
	}
	vector<vector<Lock>> multilocks;
	int totalsum;
	vector<int> sum(numoflocks, 0);
	bool valid;
	vector<bool> validity;
	int root[4];
	for (int x = 0; x < hashrootsolution.size(); x++) {
		for (int i = 0; i < 4; i++) {
			root[i] = hashrootsolution[x][3][i];
		}
		valid = true;
		totalsum = 0;
		for (int i = 0; i < numoflocks; i++) {
			if (i == 0) {
				hasher(root, hashrootsolution[x][0], multilock[i].CLHN[0]);
			}
			else {
				hasher(multilock[i - 1].CLHN[2], hashrootsolution[x][0], multilock[i].CLHN[0]);
			}
			if (!checkNoRepeat(multilock[i].CLHN[0])) valid = false;  //Check that CN contains no repeating numbers
			sum[i] = multilock[i].CLHN[0][0] + multilock[i].CLHN[0][1] + multilock[i].CLHN[0][2] + multilock[i].CLHN[0][3];
			if (i > 0) {
				if (sum[i] <= sum[i - 1]) valid = false;  //Check that CN of LH lock is smaller than that of RH lock
			}
			hasher(multilock[i].CLHN[0], hashrootsolution[x][1], multilock[i].CLHN[1]);  //Calculate LN
			hasher(multilock[i].CLHN[1], hashrootsolution[x][2], multilock[i].CLHN[2]);  //Calculate HN
			totalsum += sum[i];
		}
		if (totalsum % 2 != 0) valid = false;  //Check if sum of all unlock numbers on all locks is even
		multilocks.push_back(multilock);
		validity.push_back(valid);

		cout << "ROOT " << root[0] << root[1] << root[2] << root[3] << "\n";
		for (int i = 0; i < numoflocks; i++) {
			cout << "CN" << i << ' ' << multilock[i].CLHN[0][0] << multilock[i].CLHN[0][1] << multilock[i].CLHN[0][2] << multilock[i].CLHN[0][3] << ", ";
			cout << "LN" << i << ' ' << multilock[i].CLHN[1][0] << multilock[i].CLHN[1][1] << multilock[i].CLHN[1][2] << multilock[i].CLHN[1][3] << ", ";
			cout << "HN" << i << ' ' << multilock[i].CLHN[2][0] << multilock[i].CLHN[2][1] << multilock[i].CLHN[2][2] << multilock[i].CLHN[2][3] << "\n";
		}
		for (int i = 0; i < numoflocks; i++) {
			if (i != numoflocks - 1) {
				cout << sum[i] << " < ";
			}
			else {
				cout << sum[i] << "\n";
			}

		}
		cout << "Sum of CN numbers across all locks = " << totalsum << "\n\n";
	}

	ofstream msdata;
	string msfile;
	int i = 1;
	while (i < 9999999) {
		msfile = ".\\Multi-safe files\\Multi-safe file " + to_string(i) + ".txt";
		if (_access_s(msfile.c_str(), 0) != 0) {
			break;
		}
		i++;
	}
	int k = 0;
	msdata.open(msfile.c_str());
	for (int i = 0; i < (int)multilocks.size(); i++) {
		k++;
		msdata << "NS" << to_string(k) << ' ' << (validity[i] ? "VALID\n" : "NOT VALID\n");
		for (int j = 0; j < (int)multilocks[i].size(); j++) {
			msdata << "CN" << to_string(j) << ' ' << (multilocks[i][j]).CLHN[0][0] << (multilocks[i][j]).CLHN[0][1] << (multilocks[i][j]).CLHN[0][2] << (multilocks[i][j]).CLHN[0][3] << ", ";
			msdata << "LN" << to_string(j) << ' ' << (multilocks[i][j]).CLHN[1][0] << (multilocks[i][j]).CLHN[1][1] << (multilocks[i][j]).CLHN[1][2] << (multilocks[i][j]).CLHN[1][3] << ", ";
			msdata << "HN" << to_string(j) << ' ' << (multilocks[i][j]).CLHN[2][0] << (multilocks[i][j]).CLHN[2][1] << (multilocks[i][j]).CLHN[2][2] << (multilocks[i][j]).CLHN[2][3] << "\n";
		}
	}
	msdata.close();
}

void produceMSFromKey() throw (...) {
	int s;
	string x;
	string keyfile;
	cout << "Enter key file number for which to generate multi-safe file: ";
	cin >> x;
	fixInput(x, s, 1, 9999999, "Invalid input! Please enter an integer from 1 to 9999999: ");
	keyfile = ".\\Key files\\Key file " + to_string(s) + ".txt";
	auto start = chrono::system_clock::now();
	ifstream keydata;
	keydata.open(keyfile.c_str());
	if (keydata.fail())
		throw invalid_argument("This file does not exist: " + keyfile);
	vector<int> hashnum(4, 0);
	vector<vector<int>> ULPHF;
	ULPHF.push_back(hashnum);
	ULPHF.push_back(hashnum);
	ULPHF.push_back(hashnum);
	ULPHF.push_back(hashnum);

	int numoflocks = 5;
	string line;
	getline(keydata, line);
	string test = line.substr(0, 1);
	if (test == "L") {
		string numlocks = line.substr(6, line.length() - 6);
		istringstream iss(numlocks);
		iss >> numoflocks;
		getline(keydata, line);
	}
	string numofs = line.substr(3, line.length() - 3);
	int ns;
	istringstream iss(numofs);
	iss >> ns;
	vector<Lock> multilock;
	for (int i = 1; i <= numoflocks; i++) {
		Lock x;
		multilock.push_back(x);
	}
	vector<vector<Lock>> multilocks;
	int totalsum;
	vector<int> sum(numoflocks, 0);
	bool valid;
	vector<bool> validity;
	int root[4];
	for (int k = 0; k < ns; k++) {
		getline(keydata, line);
		for (int i = 0; i < 4; i++) {
			string roots = line.substr(5 + i, 1);
			istringstream iss(roots);
			iss >> root[i];
		}
		for (int j = 0; j < 3; j++) {
			getline(keydata, line);
			for (int i = 0; i < 4; i++) {
				string hfs = line.substr(4 + i * 3, 2);
				istringstream iss(hfs);
				iss >> ULPHF[j][i];
			}
		}
		valid = true;
		totalsum = 0;
		for (int i = 0; i < numoflocks; i++) {
			if (i == 0) {
				hasher(root, ULPHF[0], multilock[i].CLHN[0]);
			}
			else {
				hasher(multilock[i - 1].CLHN[2], ULPHF[0], multilock[i].CLHN[0]);
			}
			if (!checkNoRepeat(multilock[i].CLHN[0])) valid = false;  //Check that CN contains no repeating numbers
			sum[i] = multilock[i].CLHN[0][0] + multilock[i].CLHN[0][1] + multilock[i].CLHN[0][2] + multilock[i].CLHN[0][3];
			if (i > 0) {
				if (sum[i] <= sum[i - 1]) valid = false;  //Check that CN of LH lock is smaller than that of RH lock
			}
			hasher(multilock[i].CLHN[0], ULPHF[1], multilock[i].CLHN[1]);  //Calculate LN
			hasher(multilock[i].CLHN[1], ULPHF[2], multilock[i].CLHN[2]);  //Calculate HN
			totalsum += sum[i];
		}
		if (totalsum % 2 != 0) valid = false;  //Check if sum of all unlock numbers on all locks is even
		multilocks.push_back(multilock);
		validity.push_back(valid);

		cout << "ROOT " << root[0] << root[1] << root[2] << root[3] << "\n";
		for (int i = 0; i < numoflocks; i++) {
			cout << "CN" << i << ' ' << multilock[i].CLHN[0][0] << multilock[i].CLHN[0][1] << multilock[i].CLHN[0][2] << multilock[i].CLHN[0][3] << ", ";
			cout << "LN" << i << ' ' << multilock[i].CLHN[1][0] << multilock[i].CLHN[1][1] << multilock[i].CLHN[1][2] << multilock[i].CLHN[1][3] << ", ";
			cout << "HN" << i << ' ' << multilock[i].CLHN[2][0] << multilock[i].CLHN[2][1] << multilock[i].CLHN[2][2] << multilock[i].CLHN[2][3] << "\n";
		}
		for (int i = 0; i < numoflocks; i++) {
			if (i != numoflocks - 1) {
				cout << sum[i] << " < ";
			}
			else {
				cout << sum[i] << "\n";
			}

		}
		cout << "Sum of CN numbers across all locks = " << totalsum << "\n\n";
	}
	keydata.close();

	ofstream msdata;
	string msfile;
	int i = 1;
	while (i < 9999999) {
		msfile = ".\\Multi-safe files\\Multi-safe file " + to_string(i) + ".txt";
		if (_access_s(msfile.c_str(), 0) != 0) {
			break;
		}
		i++;
	}
	int k = 0;
	msdata.open(msfile.c_str());
	for (int i = 0; i < (int)multilocks.size(); i++) {
		k++;
		msdata << "NS" << to_string(k) << ' ' << (validity[i] ? "VALID\n" : "NOT VALID\n");
		for (int j = 0; j < (int)multilocks[i].size(); j++) {
			msdata << "CN" << to_string(j) << ' ' << (multilocks[i][j]).CLHN[0][0] << (multilocks[i][j]).CLHN[0][1] << (multilocks[i][j]).CLHN[0][2] << (multilocks[i][j]).CLHN[0][3] << ", ";
			msdata << "LN" << to_string(j) << ' ' << (multilocks[i][j]).CLHN[1][0] << (multilocks[i][j]).CLHN[1][1] << (multilocks[i][j]).CLHN[1][2] << (multilocks[i][j]).CLHN[1][3] << ", ";
			msdata << "HN" << to_string(j) << ' ' << (multilocks[i][j]).CLHN[2][0] << (multilocks[i][j]).CLHN[2][1] << (multilocks[i][j]).CLHN[2][2] << (multilocks[i][j]).CLHN[2][3] << "\n";
		}
	}
	msdata.close();
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds = end - start;
	cout << "Elapsed time: " << elapsed_seconds.count() << "s\n\n";
}

void extendKeyFile() throw (...) {
	int s;
	string x;
	string keyfile;
	cout << "Enter key file number for which to generate a completely expanded solution: ";
	cin >> x;
	fixInput(x, s, 1, 9999999, "Invalid input! Please enter an integer from 1 to 9999999: ");
	keyfile = ".\\Key files\\Key file " + to_string(s) + ".txt";
	auto start = chrono::system_clock::now();
	ifstream keydata;
	keydata.open(keyfile.c_str());
	if (keydata.fail())
		throw invalid_argument("This file does not exist: " + keyfile);
	vector<int> hashnum(4, 0);
	vector<vector<int>> ULPHF;
	ULPHF.push_back(hashnum);
	ULPHF.push_back(hashnum);
	ULPHF.push_back(hashnum);
	ULPHF.push_back(hashnum);
	vector<vector<vector<int>>> hashrootsolution;
	int numoflocks = 5;
	string line;
	getline(keydata, line);
	string test = line.substr(0, 1);
	if (test == "L") {
		string numlocks = line.substr(6, line.length() - 6);
		istringstream iss(numlocks);
		iss >> numoflocks;
		getline(keydata, line);
	}
	string numofs = line.substr(3, line.length() - 3);
	int ns;
	istringstream iss(numofs);
	iss >> ns;
	vector<int> root(4, 0);
	getline(keydata, line);
	for (int i = 0; i < 4; i++) {
		string sroots = line.substr(5 + i, 1);
		istringstream iss(sroots);
		iss >> root[i];
	}
	for (int j = 0; j < 3; j++) {
		getline(keydata, line);
		for (int i = 0; i < 4; i++) {
			string hfs = line.substr(4 + i * 3, 2);
			istringstream iss(hfs);
			iss >> ULPHF[j][i];
		}
	}
	keydata.close();

	vector<vector<int>> temp;
	temp.push_back(root);
	temp.push_back(root);
	temp.push_back(root);
	temp.push_back(root);
	for (int p4 = 0; p4 < 2; p4++) {
		for (int p3 = 0; p3 < 2; p3++) {
			for (int p2 = 0; p2 < 2; p2++) {
				for (int p1 = 0; p1 < 2; p1++) {
					for (int l4 = 0; l4 < 2; l4++) {
						for (int l3 = 0; l3 < 2; l3++) {
							for (int l2 = 0; l2 < 2; l2++) {
								for (int l1 = 0; l1 < 2; l1++) {
									for (int u4 = 0; u4 < 2; u4++) {
										for (int u3 = 0; u3 < 2; u3++) {
											for (int u2 = 0; u2 < 2; u2++) {
												for (int u1 = 0; u1 < 2; u1++) {
													temp = ULPHF;
													if ((u1 == 1) && (temp[0][0] != 0)) {
														temp[0][0] -= 10;
													}
													if ((u2 == 1) && (temp[0][1] != 0)) {
														temp[0][1] -= 10;
													}
													if ((u3 == 1) && (temp[0][2] != 0)) {
														temp[0][2] -= 10;
													}
													if ((u4 == 1) && (temp[0][3] != 0)) {
														temp[0][3] -= 10;
													}
													if ((l1 == 1) && (temp[1][0] != 0)) {
														temp[1][0] -= 10;
													}
													if ((l2 == 1) && (temp[1][1] != 0)) {
														temp[1][1] -= 10;
													}
													if ((l3 == 1) && (temp[1][2] != 0)) {
														temp[1][2] -= 10;
													}
													if ((l4 == 1) && (temp[1][3] != 0)) {
														temp[1][3] -= 10;
													}
													if ((p1 == 1) && (temp[2][0] != 0)) {
														temp[2][0] -= 10;
													}
													if ((p2 == 1) && (temp[2][1] != 0)) {
														temp[2][1] -= 10;
													}
													if ((p3 == 1) && (temp[2][2] != 0)) {
														temp[2][2] -= 10;
													}
													if ((p4 == 1) && (temp[2][3] != 0)) {
														temp[2][3] -= 10;
													}
													temp[3][0] = root[0];
													temp[3][1] = root[1];
													temp[3][2] = root[2];
													temp[3][3] = root[3];
													if (!((u1 == 1) && (temp[0][0] == 0)) && !((u2 == 1) && (temp[0][1] == 0)) && !((u3 == 1) && (temp[0][2] == 0)) && !((u4 == 1) && (temp[0][3] == 0)) &&
														!((l1 == 1) && (temp[1][0] == 0)) && !((l2 == 1) && (temp[1][1] == 0)) && !((l3 == 1) && (temp[1][2] == 0)) && !((l4 == 1) && (temp[1][3] == 0)) &&
														!((p1 == 1) && (temp[2][0] == 0)) && !((p2 == 1) && (temp[2][1] == 0)) && !((p3 == 1) && (temp[2][2] == 0)) && !((p4 == 1) && (temp[2][3] == 0))) {
														hashrootsolution.push_back(temp);
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	writeKeyFile(hashrootsolution, numoflocks);
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds = end - start;
	cout << "Elapsed time: " << elapsed_seconds.count() << "s\n\n";
}

void hasherLS(const vector<int>& num, const vector<int>& hash, vector<int>& out) {
	for (int i = 0; i < 4; i++) {
		out[i] = num[i] + hash[i];
		if (out[i] > 9) out[i] -= 10;
		if (out[i] < 0) out[i] += 10;
	}
}

void produceLS() throw (...) {
	int s;
	string x;
	string keyfile;
	cout << "Enter key file number for which to generate locked safe file: ";
	cin >> x;
	fixInput(x, s, 1, 9999999, "Invalid input! Please enter an integer from 1 to 9999999: ");
	keyfile = ".\\Key files\\Key file " + to_string(s) + ".txt";
	auto start = chrono::system_clock::now();
	ifstream keydata;
	keydata.open(keyfile.c_str());
	if (keydata.fail())
		throw invalid_argument("This file does not exist: " + keyfile);
	vector<int> hashnum(4, 0);
	vector<vector<int>> ULPHF;
	ULPHF.push_back(hashnum);
	ULPHF.push_back(hashnum);
	ULPHF.push_back(hashnum);
	int numoflocks = 5;
	string line;
	getline(keydata, line);
	string test = line.substr(0, 1);
	if (test == "L") {
		string numlocks = line.substr(6, line.length() - 6);
		istringstream iss(numlocks);
		iss >> numoflocks;
		getline(keydata, line);
	}
	string numofs = line.substr(3, line.length() - 3);
	int ns;
	istringstream iss(numofs);
	iss >> ns;
	vector<int> root(4, 0);
	vector<vector<int>> roots;
	for (int k = 0; k < ns; k++) {
		getline(keydata, line);
		for (int i = 0; i < 4; i++) {
			string roots = line.substr(5 + i, 1);
			istringstream iss(roots);
			iss >> root[i];
		}
		roots.push_back(root);
		for (int j = 0; j < 3; j++) {
			getline(keydata, line);
			if (k == 0) {  //Only need to acquire hash values from key file once
				for (int i = 0; i < 4; i++) {
					string hfs = line.substr(4 + i * 3, 2);
					istringstream iss(hfs);
					iss >> ULPHF[j][i];
				}
			}
		}
	}
	keydata.close();
	vector<int> sumofUHFLHF(4, 0);
	for (int j = 0; j < 4; j++) {
		sumofUHFLHF[j] = ULPHF[0][j] + ULPHF[1][j];
		if (sumofUHFLHF[j] < -10) {
			sumofUHFLHF[j] += 20;
		}
		else if (sumofUHFLHF[j] < 0) {
			sumofUHFLHF[j] += 10;
		}
		else if (sumofUHFLHF[j] > 9) {
			sumofUHFLHF[j] -= 10;
		}
	}
	vector<int> LN(4, 0);
	vector<int> temp(4, 0);
	ofstream lsdata;
	string lsfile;
	int i = 1;
	while (i < 9999999) {
		lsfile = ".\\Locked safe files\\Locked safe file " + to_string(i) + ".txt";
		if (_access_s(lsfile.c_str(), 0) != 0) {
			break;
		}
		i++;
	}
	lsdata.open(lsfile.c_str());
	if (numoflocks != 5) {
		lsdata << "LOCKS " << numoflocks << "\n";
	}
	lsdata << "NL " << ns << "\n";
	for (int i = 0; i < ns; i++) {
		lsdata << "ROOT: " << roots[i][0] << ' ' << roots[i][1] << ' ' << roots[i][2] << ' ' << roots[i][3] << "\n";
		for (int j = 0; j < numoflocks; j++) {
			if (j == 0)
				hasherLS(roots[i], sumofUHFLHF, LN);
			else
				hasherLS(temp, sumofUHFLHF, LN);
			lsdata << "LN" << j << ": " << LN[0] << ' ' << LN[1] << ' ' << LN[2] << ' ' << LN[3] << "\n";
			hasherLS(LN, ULPHF[2], temp);
		}
		lsdata << "\n";
	}
	lsdata.close();
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds = end - start;
	cout << "Elapsed time: " << elapsed_seconds.count() << "s\n\n";
}

bool UHFLHF(vector<vector<int>>& ULPHF, int* root, const int& numoflocks) {
	bool valid = true;
	vector<Lock> multilock;
	for (int i = 1; i <= numoflocks; i++) {
		Lock x;
		multilock.push_back(x);
	}
	vector<int> sum(numoflocks, 0);
	int totalsum = 0;
	for (int i = 0; i < numoflocks; i++) {
		if (i == 0) {
			hasher(root, ULPHF[0], multilock[i].CLHN[0]);
		}
		else {
			hasher(multilock[i - 1].CLHN[2], ULPHF[0], multilock[i].CLHN[0]);
		}
		if (!checkNoRepeat(multilock[i].CLHN[0])) {  //Check that CN contains no repeating numbers
			valid = false;
			goto _endoftest;
		}
		sum[i] = multilock[i].CLHN[0][0] + multilock[i].CLHN[0][1] + multilock[i].CLHN[0][2] + multilock[i].CLHN[0][3];
		if (i > 0) {
			if (sum[i] <= sum[i - 1]) {  //Check that CN of LH lock is smaller than that of RH lock
				valid = false;
				goto _endoftest;
			}
		}
		hasher(multilock[i].CLHN[0], ULPHF[1], multilock[i].CLHN[1]);  //Calculate LN
		hasher(multilock[i].CLHN[1], ULPHF[2], multilock[i].CLHN[2]);  //Calculate HN
		totalsum += sum[i];
	}
	if (totalsum % 2 != 0) valid = false;  //Check if sum of all unlock numbers on all locks is even
_endoftest:
	return valid;
}

void getUHFLHFCombo(int* sumUHFandLHF, vector<vector<int>>& totaltestUHF, vector<vector<int>>& totaltestLHF) {
	vector<int> testUHF(10, 0);
	vector<int> testLHF(10, 0);
	for (int i = 0; i < 4; i++) {
		switch (sumUHFandLHF[i])
		{
		case 0:
			testUHF = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };  //The sum of any element in testUHF with its corresponding
			testLHF = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };  //element in testLHF is 0 for case 0, 1 for case 1, etc.
			break;
		case 1:
			testUHF = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
			testLHF = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
		case 2:
			testUHF = { 2, 3, 4, 5, 6, 7, 8, 9, 0, 1 };
			testLHF = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
			break;
		case 3:
			testUHF = { 3, 4, 5, 6, 7, 8, 9, 0, 1, 2 };
			testLHF = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
			break;
		case 4:
			testUHF = { 4, 5, 6, 7, 8, 9, 0, 1, 2, 3 };
			testLHF = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
			break;
		case 5:
			testUHF = { 5, 6, 7, 8, 9, 0, 1, 2, 3, 4 };
			testLHF = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
			break;
		case 6:
			testUHF = { 6, 7, 8, 9, 0, 1, 2, 3, 4, 5 };
			testLHF = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
			break;
		case 7:
			testUHF = { 7, 8, 9, 0, 1, 2, 3, 4, 5, 6 };
			testLHF = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
			break;
		case 8:
			testUHF = { 8, 9, 0, 1, 2, 3, 4, 5, 6, 7 };
			testLHF = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
			break;
		case 9:
			testUHF = { 9, 0, 1, 2, 3, 4, 5, 6, 7, 8 };
			testLHF = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
			break;
		}
		totaltestUHF.push_back(testUHF);
		totaltestLHF.push_back(testLHF);
	}
}

void produceMSFromLock() throw (...) {
	int s;
	string x;
	string lsfile;
	cout << "Enter locked safe file number from which to generate key file and multi-safe file: ";
	cin >> x;
	fixInput(x, s, 1, 9999999, "Invalid input! Please enter an integer from 1 to 9999999: ");
	lsfile = ".\\Locked safe files\\Locked safe file " + to_string(s) + ".txt.";
	auto start = chrono::system_clock::now();
	ifstream lsdata;
	lsdata.open(lsfile.c_str());
	if (lsdata.fail())
		throw invalid_argument("This file does not exist: " + lsfile);
	vector<int> R(4, 0);
	vector<vector<int>> root;
	int numoflocks = 5;
	string line;
	getline(lsdata, line);
	string test = line.substr(0, 1);
	if (test == "L") {
		string numlocks = line.substr(6, line.length() - 6);
		istringstream iss(numlocks);
		iss >> numoflocks;
		if (numoflocks == 1) {
			lsdata.close();
			throw runtime_error(lsfile);
		}
		getline(lsdata, line);
	}
	string numofs = line.substr(3, line.length() - 3);
	int ns;
	istringstream iss(numofs);
	iss >> ns;
	vector<int> L1(4, 0);
	vector<vector<int>> L2;
	vector<vector<vector<int>>> LN;
	for (int j = 0; j < numoflocks; j++) {
		L2.push_back(L1);
	}
	for (int i = 0; i < 4; i++) {
		LN.push_back(L2);
	}
	for (int k = 0; k < ns; k++) {
		root.push_back(R);
		getline(lsdata, line);
		for (int i = 0; i < 4; i++) {
			string roots = line.substr(6 + 2 * i, 1);
			istringstream iss(roots);
			iss >> root[k][i];
		}
		for (int j = 0; j < numoflocks; j++) {
			L2.push_back(L1);
			getline(lsdata, line);
			for (int i = 0; i < 4; i++) {
				LN.push_back(L2);
				string LNS = line.substr(5 + 2 * i, 1);
				istringstream iss(LNS);
				iss >> LN[k][j][i];
			}
		}
		getline(lsdata, line);
	}
	lsdata.close();

	vector<int> hashnum(4, 0);
	vector<vector<int>> ULPHF;
	ULPHF.push_back(hashnum);
	ULPHF.push_back(hashnum);
	ULPHF.push_back(hashnum);
	ULPHF.push_back(hashnum);
	vector<vector<vector<int>>> hashrootsolution;

	/**
	*	Algebraically:
	*	ROOT + UHF + LHF = LN0
	*	=> UHF + LHF = LN0 - ROOT      ---(1)
	*
	*	LN0 + PHF + UHF + LHF = LN1
	*	=> PHF = LN1 - LN0 - (UHF + LHF)
	*		   = LN1 - LN0 - (LN0 - ROOT)     [using (1) from above]
	*	   PHF = LN1 - 2 * LN0 + ROOT  ---(2)
	*/

	for (int m = 0; m < ns; m++) {
		for (int i = 0; i < 4; i++) {
			ULPHF[2][i] = LN[m][1][i] - 2 * LN[m][0][i] + root[m][i];  //Using (2) derived above, meaning that the PHF values are already fixed by the LN0, LN1 and root values
			if (ULPHF[2][i] < -10) {
				ULPHF[2][i] += 20;
			}
			else if (ULPHF[2][i] < 0) {
				ULPHF[2][i] += 10;
			}
			else if (ULPHF[2][i] > 9) {
				ULPHF[2][i] -= 10;
			}
		}
		int sumUHFandLHF[4];
		for (int i = 0; i < 4; i++) {
			sumUHFandLHF[i] = LN[m][0][i] - root[m][i];  //Using (1) derived above, meaning the sum of each UHF and LHF digit pair is constrained by the corresponding LN and root digits
			if (sumUHFandLHF[i] < 0) {
				sumUHFandLHF[i] += 10;
			}
		}
		vector<vector<int>> totaltestUHF;
		vector<vector<int>> totaltestLHF;
		getUHFLHFCombo(sumUHFandLHF, totaltestUHF, totaltestLHF);
		for (int i = 0; i < 10; i++) {  //These nested for loops iterate through all 10,000 possible combinations of UHF and LHF values
			for (int j = 0; j < 10; j++) {
				for (int k = 0; k < 10; k++) {
					for (int l = 0; l < 10; l++) {
						ULPHF[0][0] = totaltestUHF[0][i];
						ULPHF[0][1] = totaltestUHF[1][j];
						ULPHF[0][2] = totaltestUHF[2][k];
						ULPHF[0][3] = totaltestUHF[3][l];
						ULPHF[1][0] = totaltestLHF[0][i];
						ULPHF[1][1] = totaltestLHF[1][j];
						ULPHF[1][2] = totaltestLHF[2][k];
						ULPHF[1][3] = totaltestLHF[3][l];
						int temp2[4];
						for (int z = 0; z < 4; z++) {
							temp2[z] = root[m][z];
						}
						if (UHFLHF(ULPHF, temp2, numoflocks)) {
							for (int i = 0; i < 4; i++) {
								ULPHF[3][i] = root[m][i];
							}
							hashrootsolution.push_back(ULPHF);
							cout << "UHF " << ULPHF[0][0] << ' ' << ULPHF[0][1] << ' ' << ULPHF[0][2] << ' ' << ULPHF[0][3] << "\n";
							cout << "LHF " << ULPHF[1][0] << ' ' << ULPHF[1][1] << ' ' << ULPHF[1][2] << ' ' << ULPHF[1][3] << "\n";
							cout << "PHF " << ULPHF[2][0] << ' ' << ULPHF[2][1] << ' ' << ULPHF[2][2] << ' ' << ULPHF[2][3] << "\n\n";
						}
					}
				}
			}
		}
	}
	if (hashrootsolution.size() != 0) {
		writeKeyFile(hashrootsolution, numoflocks);
		writeMSFile(hashrootsolution, numoflocks);
	}
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds = end - start;
	cout << "Elapsed time: " << elapsed_seconds.count() << "s\n\n";
}

int main() {
	createKeyFolder();
	createMSFolder();
	createLSFolder();
	string x;
	int s;
	do {
		cout << "                                                 The Safe Problem\n";
		cout << "********************************************************************************************************************\n";
		cout << "Please select an option:\n";
		cout << "====================================================================================================================\n";
		cout << "1. Produce key file\n";
		cout << "2. Produce multi-safe file from key file\n";
		cout << "3. Extend key file to include all possible valid combinations of positive and negative integers for UHF, LHF and PHF\n";
		cout << "4. Produce locked safe file\n";
		cout << "5. Produce key file and multi-safe file from locked safe file\n";
		cout << "6. Terminate the program\n";
		cout << "********************************************************************************************************************\n";
		cin >> x;
		fixInput(x, s, 1, 6, "Invalid input! Please enter an integer from the above menu: ");

		switch (s)
		{
		case 1:
			system("cls");
			produceKey();
			system("pause");
			system("cls");
			break;
		case 2:
			system("cls");
			try {
				produceMSFromKey();
			}
			catch (const invalid_argument& iae) {
				cout << "Unable to read data. " << iae.what() << "\n";
			}
			system("pause");
			system("cls");
			break;
		case 3:
			system("cls");
			try {
				extendKeyFile();
			}
			catch (const invalid_argument& iae) {
				cout << "Unable to read data. " << iae.what() << "\n";
			}
			system("pause");
			system("cls");
			break;
		case 4:
			system("cls");
			try {
				produceLS();
			}
			catch (const invalid_argument& iae) {
				cout << "Unable to read data. " << iae.what() << "\n";
			}
			system("pause");
			system("cls");
			break;
		case 5:
			system("cls");
			try {
				produceMSFromLock();
			}
			catch (const invalid_argument& iae) {
				cout << "Unable to read data. " << iae.what() << "\n";

			}
			catch (const runtime_error & rte) {
				cout << "There is only one lock in this locked safe file, therefore it cannot be used: " << rte.what() << "\n";
			}
			system("pause");
			system("cls");
			break;
		case 6:
			break;
		}  //No default case is required due to the above fixInput function
	} while (s != 6);
	return 0;
}