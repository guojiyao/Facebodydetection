#ifdef BodySegmentator_UNIT_TEST
#include <iostream>
#include "BodySegmentator.h"
#include "argumentparser.h"


int main(int argc, char *argv[])
{
    ArgumentParser p(argc, argv);
    if(!p.isValid()) {
        std::cout << "Input arguments are Invalid"<< std::endl << p.help() << std::endl;
        return -1;// TO DO add Enums for each error code
    }
    if(p.getFrontPath() == ArgumentParser::s_invalidPath) {
        std::cout << "Frontal path is not specified" << std::endl;
        return -2;
    }


   BodySegmentator seg;
   int retCode = seg.run(p);

  return 0;
}
#endif
