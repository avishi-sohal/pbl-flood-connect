#include "ChatBot.h"
#include "Utils.h"
#include <iostream>
#include <sstream>
using namespace std;

static string num(int n) {
    ostringstream os;
    os << n;
    return os.str();
}

static string lower(string s) {
    for (size_t i = 0; i < s.size(); i++)
        s[i] = (char)tolower((unsigned char)s[i]);
    return s;
}

ChatBot::ChatBot(const ReportManager& r, const CampManager& c, const PriorityQueue& q)
    : reports(r), camps(c), queue(q) {}

bool ChatBot::has(const string& text, const string& word) {
    return text.find(word) != string::npos;
}

string ChatBot::highPriorityAnswer() const {
    return "There are " + num(reports.countBySeverity("HIGH")) + " HIGH priority report(s) in total. "
         + "Active reports (Pending + In Progress): " + num(reports.countActive()) + ".";
}

string ChatBot::nextCaseAnswer() const {
    EmergencyReport r;
    if (!queue.peek(r)) return "The priority queue is empty. No emergency is waiting.";
    return "Next to be rescued: Report #" + num(r.getId()) + ", " + r.getVictimName()
         + " at " + r.getLocation() + " (" + r.getEmergencyType() + ", " + r.getSeverity()
         + "). " + num(queue.getSize()) + " emergencies are waiting in the queue.";
}

string ChatBot::summaryAnswer() const {
    return "Total reports: " + num(reports.getTotal())
         + " | Active: " + num(reports.countActive())
         + " | Rescued: " + num(reports.countByStatus("Rescued"))
         + " | Closed: " + num(reports.countByStatus("Closed"))
         + " | Waiting in queue: " + num(queue.getSize()) + ".";
}

string ChatBot::campAnswer() const {
    return "There are " + num(camps.getCampCount()) + " relief camp(s) with "
         + num(camps.getTotalAvailableBeds()) + " bed(s) available in total.";
}

string ChatBot::statusAnswer() const {
    return "Pending: " + num(reports.countByStatus("Pending"))
         + ", In Progress: " + num(reports.countByStatus("In Progress"))
         + ", Rescued: " + num(reports.countByStatus("Rescued"))
         + ", Closed: " + num(reports.countByStatus("Closed")) + ".";
}

string ChatBot::safetyAnswer() const {
    return "Flood safety: move to higher ground, avoid walking or driving through flood water, "
           "switch off electricity at the main switch, keep drinking water and medicines with you, "
           "and call the emergency helpline 112 or 1070 (Uttarakhand disaster control).";
}

string ChatBot::helpAnswer() const {
    return "You can ask me things like: 'who is next', 'how many high priority cases', "
           "'give me a summary', 'status of reports', 'how many beds are free', 'flood safety tips'. "
           "Type 'exit' to leave the chat.";
}

// Order matters: the most specific keywords are checked first.
string ChatBot::reply(const string& question) const {
    string q = lower(question);

    if (has(q, "hello") || has(q, "hi ") || q == "hi" || has(q, "hey"))
        return "Hello! I am FloodBot. Ask me about reports, the queue or relief camps. Type 'help' for ideas.";
    if (has(q, "help") || has(q, "what can you"))
        return helpAnswer();
    if (has(q, "next") || has(q, "first") || has(q, "urgent") || has(q, "rescue first") || has(q, "who"))
        return nextCaseAnswer();
    if (has(q, "high") || has(q, "critical") || has(q, "severe"))
        return highPriorityAnswer();
    if (has(q, "bed") || has(q, "camp") || has(q, "shelter"))
        return campAnswer();
    if (has(q, "status") || has(q, "pending") || has(q, "progress") || has(q, "rescued") || has(q, "closed"))
        return statusAnswer();
    if (has(q, "queue") || has(q, "waiting"))
        return "There are " + num(queue.getSize()) + " emergencies waiting in the priority queue. " + nextCaseAnswer();
    if (has(q, "summary") || has(q, "total") || has(q, "how many") || has(q, "overview") || has(q, "dashboard"))
        return summaryAnswer();
    if (has(q, "safe") || has(q, "tip") || has(q, "advice") || has(q, "flood"))
        return safetyAnswer();

    return "Sorry, I did not understand that. Type 'help' to see what I can answer.";
}

void ChatBot::run() const {
    printHeader("FLOODBOT ASSISTANT");
    cout << "Type your question (or 'exit' to go back).\n\n";
    while (true) {
        string q = readLine("You     : ");
        if (q.empty()) continue;
        string l = lower(q);
        if (l == "exit" || l == "quit" || l == "back") break;
        cout << "FloodBot: " << reply(q) << "\n\n";
    }
}
