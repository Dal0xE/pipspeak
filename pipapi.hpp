#ifndef PIPSPEAK
#define PIPSPEAK

using namespace std;

namespace pip {
    class AdvancedPipStream {
        public:
        vector<int> get();
        void setPersonality();
        void setContext();
        void setDepth(int l);
        void writeToFile(string dest);
        void speak(int mood);
        void toggleSmoothening();
        void setSmoothening(bool a);
        void mute();
        AdvancedPipStream();
        AdvancedPipStream(int per, int dep, double loq);
        AdvancedPipStream(int per, int dep, double loq, bool smoothen);
        AdvancedPipStream(bool smoothen);
        private:
        bool smooth;
        vector<int> wavstream, distlist;
        int personality, depth, subframeSize;
        double loquaciousness;
        void calculateStream(double seconds);
        void calculateStreamWithSentiment();
    }
    class PipStream {
        public:
        vector<int> get;
        void empty();
        void set(vector<int> l, double seconds);
        void set(string wave, double seconds);
        void writeToFile();
        void toggleSmoothening();
        void setSmoothening(bool a);
        private:
        bool smooth = true, numerical = false;
        vector<int> wavstream, distlist;
        string sheetmusic;
        double hz = 44100;
        void calculateStream(double seconds);
    }
    class DirectPipStream {
        public:
        void set(vector<int> l, double seconds);
        void set(string wave, double seconds);
        void toggleSmoothening();
        void setSmoothening(bool a);
        private:
        bool smooth = true, numerical = false;
        vector<int> wavstream, distlist;
        string sheetmusic;
        double hz = 44100;
        void calculateStream(double seconds);
        void writeToFile();
    }
}
#include "pipapi.cpp"
#endif