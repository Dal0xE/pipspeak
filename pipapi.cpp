#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdlib.h>

#if defined _WIN32 || defined _WIN64
#define CMD ""
#endif
#if defined LINUX || defined __linux__
#define CMD "aplay "
#endif
#if defined __APPLE__
#define CMD "afplay "
#endif
#if defined __FreeBSD__
#define CMD "aplay "

#include "pipapi.hpp"
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

namespace pip {
    int calculatedTone(int mood) {
        int a = 5 - mood;
        return (int) ((double) a * 1.2);
    }

/**
 * This is the API for pipSpeak. It can be used to create unqiue personalities
 * for your robot, and is used particularly to generate intelligent-sounding
 * communications. Usage of each function is probably documented somewhere.
 * 
 * For Linux and FreeBSD, ALSA is currently required in order to use
 * DirectPipStreams. This is because there is no standardized way to play sound
 * files from the command line, and these operating systems do not come with
 * one out-of-the-box.
 * 
 * Raspberry Pi pretty much cannot use DirectPipStreams out-of-the-box.
 * It is possible to add a preprocessor definition to change this, but doing
 * so in the main release would currently require making dangerous assumtpions.
 * Instead, we recommend just using an AdvancedPipStream or PipStream and
 * having them commit the sound vectors to a file and playing them manually.
 * Remember: sound files will ALWAYS be stored in out.wav
 * 
 * For help with determining mood, remember that mood is on a 10-point scale.
 * 5 is neutral, and anything over that is positive sentiment, whereas anything
 * under it is negative.
 * This applies only to Advanced augmented PipStreams. Everything else can be
 * done with high levels of customization.
 * 
 * Important note: pipSpeak is undergoing a near total rewrite. Everything you
 * see here is subject to change as we make pipSpeak better. One of our key
 * focuses is changing how we handle playing audio once it has been processed.
 * This should significantly extend the platforms we support, and make
 * development far simpler and more convenient. The second is changing the
 * way we generate sound, and will allow for the creation of a new engine
 * ecosystem within pipSpeak.
 */

//Global variables
    static double two_pi = 6.283185307179586476925286766559;

//All class declarations should be moved to the header file for production

    void PipStream::calculateStream(double seconds) {
        wavstream.clear();
        vector<double> subspace;
        int length = (int) (hz * seconds);
        if (numerical) {
            for (auto x:distlist) {
                frequency = 440.0 * pow(1.059463094359, x);
                subspace.push_back((two_pi * frequency) / hz);
            }
        }
        else {
            for (int x = 0; x < sheetmusic.length(); x++) {
                frequency = 440.0 * pow(1.059463094359, (int) sheetmusic[x] - 57);
                subspace.push_back((two_pi * frequency) / hz);
            }
        }
        for (auto x:subspace) {
            for (int x = 0; x <= length / subspace.size(); x++) {
                wavstream.push_back((int) (sin(x * y) * 0x7FFF));
            }
        }
    }
    void PipStream::set(vector<int> l, double seconds) {
        numerical = true;
        distlist = l;
        calculateStream();
    }
    void PipStream::set(string wave, double seconds) {
        numerical = false;
        sheetmusic = wave;
        calculateStream();
    }
    vector<int> PipStream::get() {
        return wavstream;
    }
    void PipStream::empty() {
        wavstream.clear();
    }
    void PipStream::writeToFile(string dest="out.wav") {
        ofstream f(dest, ios::binary);
        //write headers
        f << "RIFF----WAVEfmt ";
        write_word(f, 16, 4); //There is no extension data
        write_word(f, 1, 2); // PCM
        write_word(f, 1, 2); // Monaural file
        write_word(f, hz, 4); //Hz (samples/second)
        write_word(f, 176400, 4); // (sample_rate * bitsPerSample * channels) / 8
        write_word(f, 4, 2); // data block size
        write_word(f, 16, 2); // number of bits/sample

        size_t dataChunkPos = f.tellp();
        f << "data----";

        for (auto x:wavstream) {
            write_word(f, x, 2);
        }
        size_t fileLength = f.tellp();
        f.seekp(dataChunkPos + 4);
        write_word(f, file_length - data_chunk_pos + 8);
        f.seekp(4);
        write_word(f, file_length - 8, 4);
    }
    void DirectPipStream::writeToFile(string dest="out.wav") {
        ofstream f(dest, ios::binary);
        //write headers
        f << "RIFF----WAVEfmt ";
        write_word(f, 16, 4); //There is no extension data
        write_word(f, 1, 2); // PCM
        write_word(f, 1, 2); // Monaural file
        write_word(f, hz, 4); //Hz (samples/second)
        write_word(f, 176400, 4); // (sample_rate * bitsPerSample * channels) / 8
        write_word(f, 4, 2); // data block size
        write_word(f, 16, 2); // number of bits/sample

        size_t dataChunkPos = f.tellp();
        f << "data----";

        for (auto x:wavstream) {
            write_word(f, x, 2);
        }
        size_t fileLength = f.tellp();
        f.seekp(dataChunkPos + 4);
        write_word(f, file_length - data_chunk_pos + 8);
        f.seekp(4);
        write_word(f, file_length - 8, 4);
    }
    void AdvancedPipStream::writeToFile(string dest="out.wav") {
        ofstream f(dest, ios::binary);
        //write headers
        f << "RIFF----WAVEfmt ";
        write_word(f, 16, 4); //There is no extension data
        write_word(f, 1, 2); // PCM
        write_word(f, 1, 2); // Monaural file
        write_word(f, hz, 4); //Hz (samples/second)
        write_word(f, 176400, 4); // (sample_rate * bitsPerSample * channels) / 8
        write_word(f, 4, 2); // data block size
        write_word(f, 16, 2); // number of bits/sample

        size_t dataChunkPos = f.tellp();
        f << "data----";

        for (auto x:wavstream) {
            write_word(f, x, 2);
        }
        size_t fileLength = f.tellp();
        f.seekp(dataChunkPos + 4);
        write_word(f, file_length - data_chunk_pos + 8);
        f.seekp(4);
        write_word(f, file_length - 8, 4);
    }
    void DirectPipStream::calculateStream(double seconds) {
        wavstream.clear();
        vector<double> subspace;
        int length = (int) (hz * seconds);
        if (numerical) {
            for (auto x:distlist) {
                frequency = 440.0 * pow(1.059463094359, x);
                subspace.push_back((two_pi * frequency) / hz);
            }
        }
        else {
            for (int x = 0; x < sheetmusic.length(); x++) {
                frequency = 440.0 * pow(1.059463094359, (int) sheetmusic[x] - 57);
                subspace.push_back((two_pi * frequency) / hz);
            }
        }
        for (auto x:subspace) {
            for (int x = 0; x <= length / subspace.size(); x++) {
                wavstream.push_back((int) (sin(x * y) * 0x7FFF));
            }
        }
        writeToFile();
        system(CMD + "out.wav");
    }
    void DirectPipStream::set(vector<int> l, double seconds) {
        numerical = true;
        distlist = l;
        calculateStream(seconds);
    }
    void DirectPipStream::set(string wave, double seconds) {
        numerical = false;
        sheetmusic = wave;
        calculateStream(seconds);
    }
    void AdvancedPipStream::setDepth(int l) {
        depth = l;
        calculateStream();
    }
    void AdvancedPipStream::setPersonality(int n) {
        personality = n;
        calculateStream();
    }
    vector<int> AdvancedPipStream::get() {
        return wavstream;
    }
    void AdvancedPipStream::calculateStream(double seconds) {
        wavstream.clear();
        vector<double> subspace;
        int length = (int) (hz * seconds);
        for (auto x:distlist) {
            frequency = 440.0 * pow(1.059463094359, x);
            subspace.push_back((two_pi * frequency) / hz);
        }
        for (auto x:subspace) {
            for (int x = 0; x <= length / subspace.size(); x++) {
                wavstream.push_back((int) (sin(x * y) * 0x7FFF));
            }
        }
    }
    void PipStream::setSmoothening(bool a) {
        smooth = a;
    }
    void PipStream::toggleSmoothening() {
        smooth ^= true;
    }
    void DirectPipStream::setSmoothening(bool a) {
        smooth = a;
    }
    void DirectPipStream::toggleSmoothening() {
        smooth ^= true;
    }
    void AdvancedPipStream::setSmoothening(bool a) {
        smooth = a;
    }
    void AdvancedPipStream::toggleSmoothening() {
        smooth ^= true;
    }
    void AdvancedPipStream::calculateStreamWithSentiment(int mood, double seconds) {
        //We're not calculating the stream, just the set of note offsets
        distlist.clear();
        int jumps = personality + (mood == 0) ? 1 - personality : mood;
        int jstart = rand() % (subspaceSize / 3);
    }
    AdvancedPipStream::AdvancedPipStream() {
        smooth = true;
        personality = 2;
        depth = 0;
        loquaciousness = 3.0;
        subframeSize = 12;
    }
    AdvancedPipStream::AdvancedPipStream(int per, int dep, double loq) {
        smooth = true;
        personality = per;
        depth = dep;
        loquaciousness = loq;
        subframeSize = (int) (loq * 4);
    }
    AdvancedPipStream::AdvancedPipStream(int per, int dep, double loq, bool smoothen) {
        smooth = smoothen;
        personality = per;
        depth = dep;
        loquaciousness = loq;
        subframeSize = (int) (loq * 4);
    }
    AdvancedPipStream::AdvancedPipStream(bool smoothen) {
        smooth = smoothen;
        personality = 2;
        depth = 0;
        loquaciousness = 3.0;
        subframeSize = 12;
    }
    AdvancedPipStream::mute() {
        loquaciousness = 0;
    }
}
/**
 * Things that still need doing:
 * All sound sequences should be calculated as deviations from a baseline. The depth of voice is then
 * implemented by adding the depth to every element before calculating frequencies.

 * Possibility for the future: train a neural network to generate highs/lows for every sequence.
 * Output should be considerably more lifelike than current compute-from-scratch methods.
 */