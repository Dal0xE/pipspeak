#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

namespace little_endian_io
{
  template <typename Word>
  std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
  {
    for (; size; --size, value >>= 8)
      outs.put( static_cast <char> (value & 0xFF) );
    return outs;
  }
}
using namespace little_endian_io;

/**
 * This is the command-line version of the pipSpeak robotics communication
 * utility. It exists mainly as a method for testing/fine-tuning the
 * personality of your robot. Use as you wish.
 * 
 * For those blissfully unaware, this takes a string of ascii randomness
 * and converts it into lovely, lovely robot speak. Randomness should be
 * fine-tuned to create unique personalities.
 * 
 * Sound-guide:
 * pipSpeak accepts a string of ascii characters to represent notes.
 * Middle-C is 0, and everything else is measured in half-steps from 0.
 * The integrated edition of pipSpeak can smoothen audio as desired, but
 * the command line version will not, as it is not the main focus of the
 * project.
 */

int main(int argc, char *argv[]) {

    if (!(argc == 3)) {
        cerr << "Usage: pipspeak [duration (seconds)] [data]" << endl;
        return 1;
    }

    string l = (string) argv[1];
    string d = (string) argv[2];

    ofstream f("out.wav", ios::binary);
    //write headers
    f << "RIFF----WAVEfmt ";
    write_word(f, 16, 4); //There is no extension data
    write_word(f, 1, 2); // PCM
    write_word(f, 2, 2); // Stereo file
    write_word(f, 44100, 4); //Hz (samples/second)
    write_word(f, 176400, 4); // (sample_rate * bitsPerSample * channels) / 8
    write_word(f, 4, 2); // data block size
    write_word(f, 16, 2); // number of bits/sample

    //data chunk header
    size_t data_chunk_pos = f.tellp();
    f << "data----"; //we'll fill in the size later

    //audio samples; we need to calculate these from input
    //to do this, we'll use a simple sine wave

    const double two_pi = 6.283185307179586476925286766559;
    const double max_amplitude = 32760; //volume

    stringstream ss; //We'll need this for extracting args

    double hz = 44100;
    double frequency = 261.626; //initialized to middle-C for no reason
    double seconds; ss << l; ss >> seconds;

    ss.str(""); //clear the stream
    ss.clear(); //clear error flags (just in case)

    int N = hz * seconds; //total frames

    std::vector<int> subspace;

    //You're not expected to understand this part, but it helps if you do
    for (int n = 0; n < d.length(); n++) {
        //generate frame subspace
        frequency = 440.0 * pow(1.059463094359, (int) argv[2][n] - 53); // calculate frequency
        double value = sin((two_pi * frequency) / hz); // This doesn't work. Not sure why. Just doesn't.
        subspace.push_back((int) (max_amplitude * value));
    }

    //Now we need to extrapolate the subspace over the actual sample space
    int a = N / subspace.size();
    for (auto x:subspace) {
        for (int y = 0; y < a; y++) {
            write_word(f, x, 2); write_word(f, x , 2); //stereo audio
        }
    }
    size_t file_length = f.tellp();

    //fix data chunk header
    f.seekp(data_chunk_pos + 4);
    write_word(f, file_length - data_chunk_pos + 8);

    //fix the file header
    f.seekp(0 + 4);
    write_word(f, file_length - 8, 4);

    return 0;
}