#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "utils.h"

using namespace std;

int main(int argc, char** argv) {
	if (argc > 1) {
		string file = argv[1], name = "music";

		if (argc > 2) {
			name = argv[2];
		}

		stringstream out;
		out << "#ifndef " << name << "_h\n"
			<< "#define " << name << "_h\n\n"
			<< "#include \"Note.h\"\n\n"
			<< "const struct Note " << name << "Notes[] = {\n";

		int length = 0, bar = 1;
		double barValue = 0;

		Piece piece = parsePiece(file);
		for (const Note& note : piece.notes) {
			double freq = pitchMap[note.pitch], duration = 60 / piece.bpm * note.value, iterations = duration * freq;
			PeriodData period = calculatePeriod(freq);

			if (barValue == 0) {
				out << "// Meas. " << bar << endl;
			}

			out << "\t{ " << (int)period.period << ", " << period.prescaler << ", " << (int)iterations << ", " << (int)(note.pitch == "r") << " }, // "
				<< note.original << " " << note.pitch << "\n";
			length++;
			barValue += note.value;

			if (barValue >= piece.timeN) {
				barValue -= piece.timeN;
				bar++;
			}
		}

		out << "};\n\n"
			<< "int " << name << "Length = " << length << ";\n\n"
			<< "#endif" << endl;

		ofstream of(name + ".h");
		of << out.str();
		of.close();
	} else {
		cerr << "Usage: " << argv[0] << " <input> [name]" << endl;
		return 1;
	}

	return 0;
}