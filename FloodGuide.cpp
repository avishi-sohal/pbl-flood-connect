#include "FloodGuide.h"
#include <cctype>
using namespace std;

// FloodBot knowledge base. Each entry: comma-separated keywords, then the answer.
// A keyword ending in $ must match a whole word (so "hi$" does not match "high").
// Entries earlier in the table win ties, so safety topics come before greetings.
// The answer "@CAMPS" is replaced by the live relief-camp list in ChatBot.cpp.
struct GuideEntry { const char* keys; const char* answer; };

static const GuideEntry GUIDE[] = {
    {"trapped,stuck,drowning,rescue me,help me,in danger,water rising,rising water,water is coming,water entering,need rescue,save us,save me,dying,cannot get out",
     "If you are in immediate danger, call 112 (or 1070, the Uttarakhand State Disaster Control Room) right now and give your exact location. While you wait: move to the highest safe place (upper floor or roof), do not walk or swim through moving water, take your phone, torch, drinking water and medicines, and signal with a torch or a bright cloth. Then report your emergency in FloodConnect so the rescue team can see it."},
    {"report,register,submit,complaint,raise emergency,add emergency,how do i ask for rescue,request rescue",
     "To report an emergency here, choose Report an Emergency (menu 1 in the program, the Reports tab in the web version) and enter your name, exact location, type of emergency, severity (HIGH if life is at risk), a short description and a 10-digit contact number. You get a report ID: keep it and ask me 'status of report <ID>' any time. For anything life-threatening also call 112."},
    {"camp,camps,shelter,relief camp,where to go,safe place,place to stay,beds,bed free,accommodation",
     "@CAMPS"},
    {"helpline,emergency number,phone number,contact number,control room,ambulance,police,fire brigade,112,1070,1077,108,toll free,who to call,whom to call,call",
     "Emergency numbers (India): 112 national emergency, 1070 Uttarakhand State Disaster Control Room, 1077 District Control Room, 108 ambulance, 100 police, 101 fire."},
    {"evacuate,evacuation,leave home,leave house,should i leave,leave the area,order to leave",
     "If authorities order an evacuation, leave immediately. Take your ready bag, switch off electricity and gas, lock the house, tell family and neighbours where you are going, use the safest high-ground route and head to the nearest relief camp (ask me 'relief camps'). Do not return until it is declared safe."},
    {"carry,bring,pack,emergency kit,go bag,ready bag,essentials,what to take,what should i take",
     "Take: drinking water, dry food, your medicines (a week's supply if you can), phone with power bank, torch, a change of clothes, a blanket, ID and important papers in a waterproof bag, some cash and baby supplies if needed. Carry only what you can hold; leave heavy items behind."},
    {"prepare,preparation,before flood,get ready,be ready,plan ahead,precaution,precautions",
     "Prepare now: know the nearest relief camp and the safest route to higher ground, keep a ready bag (water, food, medicines, torch, power bank, documents in a waterproof cover), save the numbers 112, 1070, 1077 and 108, keep your phone charged, move valuables to an upper floor and agree on a meeting point with your family."},
    {"during flood,what to do,what should i do,flood safety,safety,tips,advice,survive,stay safe,precautions during",
     "During a flood: move to higher ground early; never walk, swim or drive through flood water (about 15 cm of moving water can knock an adult off their feet); stay away from rivers, drains and fallen power lines; switch off electricity at the main switch only if you can reach it from dry ground; follow official alerts; and call 112 or 1070 if you are in danger."},
    {"after flood,after the flood,return home,going back,go back home,clean up,cleanup,damage,flood is over,water has gone",
     "After a flood: return only when authorities say it is safe, wear boots and gloves, do not touch wet electrical items until an electrician checks them, throw away food that touched flood water, boil or treat drinking water, photograph the damage for claims, and watch out for snakes and weak walls or floors."},
    {"electric,electricity,current,power line,shock,switch,wire,socket,short circuit,transformer",
     "Water and electricity are deadly together. Switch off the main power only if you can reach the switch while standing on dry ground. Never touch wet switches, wires or appliances, and stay far away from fallen power lines (report them to the electricity department or 112). Do not use wet appliances until an electrician has checked them."},
    {"drive,driving,car,vehicle,bike,scooter,cross the road,cross the river,bridge,wade,walk through,causeway",
     "Do not drive or walk through flood water. It can hide damaged roads, open drains and strong currents, and a vehicle can stall or float in about 30 cm of water. Turn around and take another route. If your vehicle is caught in rising water, get out and move to higher ground, but only if the water around you is not fast moving."},
    {"drink,drinking water,safe water,water safe,safe to drink,boil,purify,tap water,chlorine,contaminated,water is dirty,clean water",
     "Drink only safe water: boil it at a rolling boil for at least 1 minute, or use chlorine tablets as directed, or use sealed bottled water. Store it in clean, covered containers. Do not drink flood water, and avoid tap water until it is declared safe."},
    {"food,eat,cook,cooking,fridge,leftover,leftovers,ration,hungry,hunger,packets",
     "Eat only sealed or freshly cooked food. Throw away food that touched flood water, food from a fridge that lost power for more than 4 hours, and swollen or damaged cans. Wash hands with soap and safe water before eating. In a relief camp, ask the camp staff for food packets."},
    {"disease,diarrhoea,diarrhea,loose motion,cholera,typhoid,dengue,malaria,leptospirosis,infection,fever,hygiene,mosquito,vomiting,rash",
     "Floods spread infections such as diarrhoea, typhoid, cholera, leptospirosis, dengue and malaria. Wash hands with soap and safe water, drink boiled or treated water, keep wounds clean and covered, avoid wading in dirty water, and use mosquito nets or repellent. See a doctor or the camp medical post for fever, vomiting, loose motions or a red, swollen wound. In an emergency call 108."},
    {"first aid,bleeding,injury,injured,hurt,burn,fracture,broken bone,faint,unconscious,cpr,not breathing,hypothermia,shivering,wound,cut",
     "Basic first aid: for bleeding, press firmly on the wound with a clean cloth; for burns, cool with clean running water for 10 to 20 minutes; keep a suspected broken bone still; if someone is unconscious and not breathing, call 108 and start chest compressions if you know how. I am not a doctor, so for any serious injury call 108 immediately."},
    {"snake,snakes,bite,bitten,sting,scorpion",
     "For a snake bite: keep the person calm and still, keep the bitten limb at or below heart level, remove rings and tight items, and get to a hospital or call 108 quickly. Do not cut the wound, suck the venom, apply ice or tie a tight tourniquet. Floods push snakes into homes, so shake out shoes and clothes before using them."},
    {"child,children,kid,kids,baby,infant,elderly,old people,old age,grandparents,disabled,disability,wheelchair,pregnant,pregnancy",
     "Children, elderly people, pregnant women and people with disabilities should be moved to safety first. Keep them warm and dry, carry their medicines and special items, and write their needs in your report description so the rescue team and camp staff can plan for them."},
    {"pet,pets,dog,cat,cow,cattle,buffalo,animal,animals,livestock,goat",
     "Take pets with you if you can, with a leash or carrier, food and water. Many relief camps cannot take animals, so ask the District Control Room (1077) about animal shelters. Move cattle to higher ground early, and do not risk your life to save animals."},
    {"medicine,medicines,medication,insulin,prescription,diabetes,diabetic,blood pressure,tablet,tablets,dialysis,oxygen",
     "Keep your regular medicines (and a copy of the prescription) in a waterproof bag, ideally a week's supply. Tell the camp medical staff about any condition or allergy. Medicines that need to stay cool, like insulin, should go in a cool bag; ask the camp medical post for help. For urgent medical help call 108."},
    {"document,documents,aadhaar,certificate,valuables,important papers,insurance,compensation,claim,relief money,ex gratia",
     "Keep Aadhaar, ID, property papers, insurance and bank documents in a sealed waterproof bag, and keep photos or scans on your phone. For relief or compensation after a flood, contact the District Control Room (1077) or your tehsil office, and keep photos of the damage."},
    {"landslide,mudslide,hill,hills,cloudburst,slope,debris,boulder,rockfall",
     "Hill areas can see landslides and cloudbursts with heavy rain. Move away from slopes, river banks and drains. Leave if you hear rumbling or see mud flowing or tilting trees and poles, and move sideways away from the path, not downhill. Do not cross roads covered by debris. Call 112 or 1070."},
    {"phone,mobile,battery,charge,charging,network,signal,power bank,sms,whatsapp,torch",
     "To save phone battery: lower the brightness, turn on battery saver, close apps, use SMS instead of calls when the network is weak, and keep a power bank. Share your location with family and keep emergency numbers saved."},
    {"forecast,weather,warning,alert,alerts,imd,rain,rainfall,red alert,orange alert,yellow alert,will it flood,dam,river level",
     "I cannot see live weather or river levels. Follow official alerts from the India Meteorological Department (IMD), the Uttarakhand State Disaster Management Authority and your district administration. If an evacuation is ordered, leave immediately."},
    {"when will,how long,eta,how soon,wait,waiting,arrive,rescue team,taking so long,no one came,nobody came",
     "I cannot promise an arrival time. Reports are handled by priority: HIGH (life at risk) first, then MEDIUM, then LOW, and by arrival order within the same level. Keep your exact location and a working phone number ready, stay somewhere high and safe, and call 112 if things get worse. Ask me 'status of report <ID>' to check your place in the queue."},
    {"priority,how does it work,how are cases,queue,prioritize,prioritise,how do you decide",
     "FloodConnect ranks every pending report: HIGH severity (life at risk) first, then MEDIUM, then LOW; reports of the same severity are handled in the order they arrived. The rescue team then finds the shortest open road to the location and matches the person to the nearest camp that has what they need."},
    {"volunteer,donate,donation,help others,want to help,contribute,relief work,ngo",
     "Thank you for wanting to help. Please do not enter flood water yourself. Contact the District Control Room (1077) or your local relief committee to register as a volunteer, donate supplies or join organised relief work."},
    {"missing,lost,missing person,find my family,relative,cannot find,separated,family member",
     "If someone is missing, call 112 or the police (100) and the District Control Room (1077) with the person's name, age, clothing and last known location. Register with the relief camp staff too, as camps keep lists of the people who arrive."},
    {"who are you,what are you,your name,are you real,are you human,are you ai,are you a bot",
     "I am FloodBot, FloodConnect's assistant. I answer from a built-in knowledge base of flood safety advice and from live report and relief-camp data. I am not a person or a doctor, so call 112 in an emergency."},
    {"what can you,can you help,how can you,help$,menu,options,topics,commands",
     "You can ask me about: how to report an emergency, relief camps and free beds, helpline numbers, what to do before, during and after a flood, evacuation, what to pack, electricity, driving, safe drinking water, food, health and first aid, snake bites, landslides, pets, children and the elderly, medicines, documents, missing people, or 'status of report <ID>'."},
    {"thank,thanks,thx,thank you,appreciate,great,helpful",
     "You are welcome. Stay safe, and call 112 if you are ever in danger."},
    {"hi$,hii$,hello,hey$,namaste,good morning,good evening,good afternoon,good night",
     "Hello! I am FloodBot. Ask me about flood safety, relief camps, how to report an emergency, helpline numbers, drinking water, first aid, or the status of your report."},
    {"bye,goodbye,see you,take care",
     "Stay safe. Goodbye!"}
};
static const int GUIDE_SIZE = (int)(sizeof(GUIDE) / sizeof(GUIDE[0]));

const char* GUIDE_FALLBACK =
    "I am not sure about that one. I can help with: reporting an emergency, relief camps and beds, helpline numbers, flood safety before, during and after a flood, evacuation, electricity, driving, drinking water, food, health, first aid, snake bites, landslides, pets, children and the elderly, medicines, documents, missing people, and the status of your report. If you are in danger, call 112 now.";

// lower case, every non letter/digit becomes a space, words separated by one space, padded with spaces
static string normalize(const string& text) {
    string out = " ";
    for (size_t i = 0; i < text.size(); i++) {
        char ch = (char)tolower((unsigned char)text[i]);
        if (isalnum((unsigned char)ch)) out += ch;
        else if (out[out.size() - 1] != ' ') out += ' ';
    }
    if (out[out.size() - 1] != ' ') out += ' ';
    return out;
}

// score = number of words in every keyword that appears in the question
string guideAnswer(const string& question) {
    string q = normalize(question);
    int bestScore = 0;
    int best = -1;
    for (int e = 0; e < GUIDE_SIZE; e++) {
        string keys = GUIDE[e].keys;
        int score = 0;
        size_t start = 0;
        while (start <= keys.size()) {
            size_t comma = keys.find(',', start);
            if (comma == string::npos) comma = keys.size();
            string kw = keys.substr(start, comma - start);
            start = comma + 1;
            if (kw.empty()) continue;
            bool whole = kw[kw.size() - 1] == '$';
            if (whole) kw = kw.substr(0, kw.size() - 1);
            string pattern = " " + kw + (whole ? " " : "");
            if (q.find(pattern) != string::npos) {
                int words = 1;
                for (size_t i = 0; i < kw.size(); i++) if (kw[i] == ' ') words++;
                score += words;
            }
        }
        if (score > bestScore) { bestScore = score; best = e; }
    }
    return best < 0 ? string("") : string(GUIDE[best].answer);
}
