#ifndef SECTIONREPOSITORY_H
#define SECTIONREPOSITORY_H

#include "section.h"
#include <vector>
#include <optional>

class SectionRepository {
public:
    SectionRepository();
    
    bool addSection(const Section& section);
    bool updateSection(const Section& section);
    bool deleteSection(int id);
    std::vector<Section> getAllSections();
    std::optional<Section> getSectionById(int id);
};

#endif // SECTIONREPOSITORY_H
