#ifndef FLOODGUIDE_H
#define FLOODGUIDE_H

#include <string>

// Keyword-scoring knowledge base for FloodBot (flood safety, helplines, health, camps...).
// Returns the best matching answer, or "" if nothing matches. "@CAMPS" means "show the live camp list".
std::string guideAnswer(const std::string& question);

extern const char* GUIDE_FALLBACK;   // what FloodBot says when nothing matches

#endif
