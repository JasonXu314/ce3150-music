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

		Piece piece = parsePiece(file);

		stringstream out;
		out << "#ifndef " << name << "_h\n"
			<< "#define " << name << "_h\n\n"
			<< "#include <avr/io.h>\n"
			<< "#include <avr/interrupt.h>\n\n"
			<< "#include \"Note.h\"\n"
			<< "void delay(int duration, int prescaler);\n\n"
			<< "const struct Note " << name << "Notes[] = {\n";
		unordered_map<Note, int> noteMap = generateNoteMap(piece, out);

		stringstream fnBody;

		int length = 0, bar = 1;
		double barValue = 0;

		for (const Note& note : piece.notes) {
			double freq = pitchMap[note.pitch], duration = 60 / piece.bpm * note.value, iterations = duration * freq;

			if (barValue == 0) {
				fnBody << "// Meas. " << bar << endl;
			}

			fnBody << "\t\tplay(" << noteMap[note] << ", " << (int)iterations << ", " << (int)(note.pitch == "r") << ");\n";
			length++;
			barValue += note.value;

			if (barValue >= piece.timeN) {
				barValue -= piece.timeN;
				bar++;
			}
		}

		out << "};\n\n"
			<< "void play(int note, int iterations, int rest) {\n"
			<< "\tfor (int i = 0; i < iterations; i++) {\n"
			<< "\t\tif (!rest) PORTE ^= 0x10;\n"
			<< "\t\tdelay(" << name << "Notes[note].period / 2, " << name << "Notes[note].prescaler);\n"
			<< "\t\tif (!rest) PORTE ^= 0x10;\n"
			<< "\t\tdelay(" << name << "Notes[note].period / 2, " << name << "Notes[note].prescaler);\n"
			<< "\t}\n"
			<< "}\n\n"
			<< "void playMusic() {\n"
			// << "\tfor (int i = 0; i < " << length << "; i++) {\n"
			<< fnBody.str()
			// << "\t}\n"
			<< "}\n\n"
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