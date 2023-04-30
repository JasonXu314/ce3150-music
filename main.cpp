#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "Note.h"

#define P_FREQ 16000000

using namespace std;

map<string, double> pitchMap{{"c4", 261.63}, {"d4", 293.66}, {"e4", 329.63}, {"f4", 349.23}, {"g4", 392.00},
							 {"a4", 440.00}, {"b4", 493.88}, {"c5", 523.25}, {"d5", 587.33}, {"e5", 659.25}};

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