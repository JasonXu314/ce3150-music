#include "utils.h"

using namespace std;

map<string, double> pitchMap{{"r", 440.00},	   {"e2", 82.41},	 {"f2", 87.31},	   {"f#2", 92.50},	{"gb2", 92.50},	  {"g2", 98.00},	{"g#2", 103.83},
							 {"ab2", 103.83},  {"a2", 110.00},	 {"a#2", 116.54},  {"bb2", 116.54}, {"b2", 123.47},	  {"c3", 130.81},	{"c#3", 138.59},
							 {"db3", 138.59},  {"d3", 146.83},	 {"d#3", 155.56},  {"eb3", 155.56}, {"e3", 164.81},	  {"f3", 174.61},	{"f#3", 185.00},
							 {"gb3", 185.00},  {"g3", 196.00},	 {"g#3", 207.65},  {"ab3", 207.65}, {"a3", 220.00},	  {"a#3", 233.08},	{"bb3", 233.08},
							 {"b3", 246.91},   {"c4", 261.63},	 {"c#4", 277.18},  {"db4", 277.18}, {"d4", 293.66},	  {"d#4", 311.13},	{"eb4", 311.13},
							 {"e4", 329.63},   {"f4", 349.23},	 {"f#4", 369.99},  {"gb4", 369.99}, {"g4", 392.00},	  {"g#4", 415.30},	{"ab4", 415.30},
							 {"a4", 440.00},   {"a#4", 466.16},	 {"bb4", 466.16},  {"b4", 493.88},	{"c5", 523.25},	  {"c#5", 554.37},	{"db5", 554.37},
							 {"d5", 587.33},   {"d#5", 622.25},	 {"eb5", 622.25},  {"e5", 659.25},	{"f5", 698.46},	  {"f#5", 739.99},	{"gb5", 739.99},
							 {"g5", 783.99},   {"g#5", 830.61},	 {"ab5", 830.61},  {"a5", 880.00},	{"a#5", 932.33},  {"bb5", 932.33},	{"b5", 987.77},
							 {"c6", 1046.50},  {"c#6", 1108.73}, {"db6", 1108.73}, {"d6", 1174.66}, {"d#6", 1244.51}, {"eb6", 1244.51}, {"e6", 1318.51},
							 {"f6", 1396.91},  {"f#6", 1479.98}, {"gb6", 1479.98}, {"g6", 1567.98}, {"g#6", 1661.22}, {"ab6", 1661.22}, {"a6", 1760.00},
							 {"a#6", 1864.66}, {"bb6", 1864.66}, {"b6", 1975.53},  {"c7", 2093.00}, {"c#7", 2217.46}, {"db7", 2217.46}, {"d7", 2349.32},
							 {"d#7", 2489.02}, {"eb7", 2489.02}, {"e7", 2637.02},  {"f7", 2793.83}, {"f#7", 2959.96}, {"gb7", 2959.96}, {"g7", 3135.96}};

bool operator==(const Note& a, const Note& b) { return (a.pitch + " " + to_string(a.value)) == (b.pitch + " " + to_string(b.value)); }

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
	} else if (note == "ds") {
		out = (double)timeD / 16 * 1.5;
	} else if (note == "s") {
		out = (double)timeD / 16;
	} else if (note == "t") {
		out = (double)timeD / 32;
	} else {
		throw runtime_error("Unexpected note " + note);
	}

	return slurred ? out : max(out * 0.95, out - (double)timeD / 16);
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

unordered_map<Note, int> generateNoteMap(const Piece& piece, stringstream& out) {
	unordered_map<Note, int> noteMap;
	int idx = 0;

	for (auto& note : piece.notes) {
		if (!noteMap.count(note)) {
			noteMap.emplace(note, idx++);

			double freq = pitchMap[note.pitch];
			PeriodData period = calculatePeriod(freq);
			out << "\t{ " << (int)period.period << ", " << period.prescaler << " }, // " << note.original << " " << note.pitch << "\n";
		}
	}

	return noteMap;
}