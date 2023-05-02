#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#define P_FREQ 16000000

struct Note {
	std::string pitch;
	std::string original;
	double value;
};

struct Piece {
	double bpm;
	int timeN;
	int timeD;
	std::vector<Note> notes;
};

struct PeriodData {
	double period;
	int prescaler;
};

extern std::map<std::string, double> pitchMap;

Piece parsePiece(const std::string& file);

void parse(const std::string& line, int timeD, std::vector<Note>& notes);

std::vector<std::string> split(const std::string& str, char delim);

double noteValue(const std::string& note, int timeD, bool slurred);

PeriodData calculatePeriod(double freq);

#endif