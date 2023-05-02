#include "utils.h"

using namespace std;

map<string, double> pitchMap{{"r", 440.00},	  {"b3", 246.91}, {"c4", 261.63},  {"c#4", 277.18}, {"db4", 277.18}, {"d4", 293.66},  {"d#4", 311.13},
							 {"eb4", 311.13}, {"e4", 329.63}, {"f4", 349.23},  {"f#4", 369.99}, {"gb4", 369.99}, {"g4", 392.00},  {"g#4", 415.30},
							 {"ab4", 415.30}, {"a4", 440.00}, {"a#4", 466.16}, {"bb4", 466.16}, {"b4", 493.88},	 {"c5", 523.25},  {"c#5", 554.37},
							 {"db5", 554.37}, {"d5", 587.33}, {"d#5", 622.25}, {"eb5", 622.25}, {"e5", 659.25},	 {"f5", 698.46},  {"f#5", 739.99},
							 {"gb5", 739.99}, {"g5", 783.99}, {"g#5", 830.61}, {"ab5", 830.61}, {"a5", 880.00},	 {"a#5", 932.33}, {"bb5", 932.33},
							 {"b5", 987.77},  {"c6", 1046.50}};

Piece parsePiece(const string& file) {
	ifstream in(file);
	string line;

	getline(in, line);
	double bpm = stod(line);
	getline(in, line);
	vector<string> timeSignature = split(line, ' ');
	int timeN = stoi(timeSignature[0]), timeD = stoi(timeSignature[1]);

	vector<Note> notes;
	while (getline(in, line)) {
		if (line == "" || line[0] == '#') {
			continue;
		}
		parse(line, timeD, notes);
	}

	return Piece{bpm, timeN, timeD, notes};
}

void parse(const string& line, int timeD, vector<Note>& notes) {
	vector<string> parts = split(line, ' ');
	bool slurred = parts[2] == "t";
	double value = noteValue(parts[1], timeD, slurred);

	notes.push_back(Note{parts[0], parts[1], value});
	if (!slurred) {
		notes.push_back(Note{"r", "inserted rest", noteValue(parts[1], timeD, true) - value});
	}
}

vector<string> split(const string& str, char delim) {
	vector<string> out;
	string substr;

	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] == delim) {
			out.push_back(substr);
			substr = "";
		} else {
			substr += str[i];
		}
	}

	out.push_back(substr);
	return out;
}

double noteValue(const string& note, int timeD, bool slurred) {
	double out = 0;

	if (note == "dw") {
		out = timeD * 1.5;
	} else if (note == "w") {
		out = timeD;
	} else if (note == "dh") {
		out = timeD * 0.75;
	} else if (note == "h") {
		out = (double)timeD / 2;
	} else if (note == "dq") {
		out = timeD * 0.375;
	} else if (note == "q") {
		out = (double)timeD / 4;
	} else if (note == "de") {
		out = timeD * 0.1875;
	} else if (note == "e") {
		out = (double)timeD / 8;
	} else if (note == "s") {
		out = (double)timeD / 16;
	} else {
		throw runtime_error("Unexpected note " + note);
	}

	return slurred ? out : max(out * 0.8, out - (double)timeD / 16);
}

PeriodData calculatePeriod(double freq) {
	if (P_FREQ / freq < 512) {
		return PeriodData{P_FREQ / freq, 0x01};
	} else if ((P_FREQ / freq) / 8 < 512) {
		return PeriodData{(P_FREQ / freq) / 8, 0x02};
	} else if ((P_FREQ / freq) / 32 < 512) {
		return PeriodData{(P_FREQ / freq) / 32, 0x03};
	} else if ((P_FREQ / freq) / 64 < 512) {
		return PeriodData{(P_FREQ / freq) / 64, 0x04};
	} else if ((P_FREQ / freq) / 128 < 512) {
		return PeriodData{(P_FREQ / freq) / 128, 0x05};
	} else if ((P_FREQ / freq) / 256 < 512) {
		return PeriodData{(P_FREQ / freq) / 256, 0x06};
	} else if ((P_FREQ / freq) / 1024 < 512) {
		return PeriodData{(P_FREQ / freq) / 1024, 0x07};
	} else {
		throw runtime_error("No prescaler works for frequency");
	}
}