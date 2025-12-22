#ifndef SECTIONREPOSITORY_H
#define SECTIONREPOSITORY_H

#include "section.h"
#include <vector>

class SectionRepository {
public:
    SectionRepository();
    
    bool addSection(const Section& section);
    bool deleteSection(int id);
    std::vector<Section> getAllSections();
};

#endif // SECTIONREPOSITORY_H
