#ifndef CHATBOT_H
#define CHATBOT_H

#include <string>
#include "EmergencyReport.h"
#include "PriorityQueue.h"
#include "ReliefCamp.h"
using namespace std;

// MODULE 6: FloodBot, a small assistant that answers questions using the REAL data.
// It reads (never changes) the report list, the camps and the priority queue.
// How it works: the question is turned to lower case, then we look for keywords.
class ChatBot {
private:
    const ReportManager& reports;
    const CampManager& camps;
    const PriorityQueue& queue;

    static bool has(const string& text, const string& word);   // does text contain word?
    string highPriorityAnswer() const;
    string nextCaseAnswer() const;
    string summaryAnswer() const;
    string campAnswer() const;
    string statusAnswer() const;
    string helpAnswer() const;
    string safetyAnswer() const;

public:
    ChatBot(const ReportManager& r, const CampManager& c, const PriorityQueue& q);
    string reply(const string& question) const;   // returns the bot's answer
    void run() const;                             // chat loop (type "exit" to leave)
};

#endif
