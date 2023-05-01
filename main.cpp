#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "Note.h"

#define P_FREQ 16000000

using namespace std;

map<string, double> pitchMap{{"b3", 246.91}, {"c4", 261.63},  {"c#4", 277.18}, {"db4", 277.18}, {"d4", 293.66}, {"d#4", 311.13}, {"eb4", 311.13},
							 {"e4", 329.63}, {"f4", 349.23},  {"f#4", 369.99}, {"gb4", 369.99}, {"g4", 392.00}, {"g#4", 415.30}, {"ab4", 415.30},
							 {"a4", 440.00}, {"a#4", 466.16}, {"bb4", 466.16}, {"b4", 493.88},	{"c5", 523.25}, {"c#5", 554.37}, {"db5", 554.37},
							 {"d5", 587.33}, {"d#5", 622.25}, {"eb5", 622.25}, {"e5", 659.25}};

double noteValue(const string& note, int timeD);
double calculatePeriod(double freq);

int main(int argc, char** argv) {
	if (argc > 1) {
		string file = argv[1], name = "music";

		if (argc > 2) {
			name = argv[2];
		}

		ifstream in(file);
		int bpm, timeN, timeD;

		in >> bpm >> timeN >> timeD;

		stringstream out;
		out << "#ifndef " << name << "_h\n"
			<< "#define " << name << "_h\n\n"
			<< "#include \"Note.h\"\n\n"
			<< "const struct Note " << name << "Notes[] = {\n";

		int length = 0;
		double prevDuration = 0;
		string pitch, note, tied;
		while (in >> pitch >> note >> tied) {
			double freq = pitchMap[pitch], value = noteValue(note, timeD), duration = bpm / 60 * value;

			if (tied == "t") {
				prevDuration += duration;
			} else {
				double period = calculatePeriod(freq), iterations = (prevDuration + duration) * freq;

				out << "\t{ " << (int)period << ", " << (int)iterations << " },\n";
				prevDuration = 0;
				length++;
			}
		}

		out << "};\n\n"
			<< "int " << name << "Length = " << length << ";\n\n"
			<< "#endif" << endl;

		ofstream of(name + ".h");
		of << out.str();
		in.close();
		of.close();
	} else {
		cerr << "Usage: " << argv[0] << " <input> [name]" << endl;
		return 1;
	}

	return 0;
}

double noteValue(const string& note, int timeD) {
	if (note == "w") {
		return timeD;
	} else if (note == "h") {
		return (double)timeD / 2;
	} else if (note == "q") {
		return (double)timeD / 4;
	} else if (note == "e") {
		return (double)timeD / 8;
	} else if (note == "s") {
		return (double)timeD / 16;
	} else {
		throw runtime_error("Unexpected note " + note);
	}
}

double calculatePeriod(double freq) { return (P_FREQ / freq) / 256; }