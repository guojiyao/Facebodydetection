#include "ArgumentParser.h"
#include <cstdlib>

const std::string ArgumentParser::s_invalidPath = "";
const int ArgumentParser::s_invalidNumericalValue = -1;



ArgumentParser::ArgumentParser(int argc, char *argv[])
    : m_preproc(false)
    , m_frontalPath(s_invalidPath)
    , m_sidePath(s_invalidPath)
    , m_width(s_invalidNumericalValue)
    , m_height(s_invalidNumericalValue)
    , m_fPixelX(s_invalidNumericalValue)
    , m_sPixelX(s_invalidNumericalValue)
    , m_wast(s_invalidNumericalValue)
    , m_isValid(false)
    , mArgc(argc)
    , m_earDetect(false)
    , m_noseDetect(false)
    , m_FlipImage(false)
{
    for (int i = 0; i < argc; ++i){
        mArgv.push_back(argv[i]);
        //std::cout << mArgv[i] << "    " ;
    }
    m_isValid = parse();
}

bool ArgumentParser::parse()
{
    for(int i = 0; i < mArgc; ++ i) {
        if(mArgv[i] == std::string("-f") || mArgv[i] == std::string("--front")) {
            if(i + 1 < mArgc) {
                m_frontalPath = mArgv[i + 1];
            } else {
                return false;
            }
        } else if(mArgv[i] == std::string("-e") || mArgv[i] == std::string("--ear")) {
            if(i + 1 < mArgc) {
                if (mArgv[i + 1]== "t") m_earDetect = true;
            } else {
                return false;
            }
        } else if(mArgv[i] == std::string("-n") || mArgv[i] == std::string("--nose")) {
            if(i + 1 < mArgc) {
                if (mArgv[i + 1]== "t") m_noseDetect = true;
            } else {
                return false;
            }
        } else if(mArgv[i] == std::string("-fl") || mArgv[i] == std::string("--flip")) {
            if(i + 1 < mArgc) {
                if (mArgv[i + 1]== "t") m_FlipImage = true;
            } else {
                return false;
            }
        } else if(mArgv[i] == std::string("-s") || mArgv[i] == std::string("--side")) {
            if(i + 1 < mArgc) {
                m_sidePath = mArgv[i + 1];
            } else {
                return false;
            }
        } else if(mArgv[i] == std::string("-p") || mArgv[i] == std::string("--pre-process") ) {
            m_preproc = true;
        } else if(mArgv[i] == std::string("-w") || mArgv[i] == std::string("-width") ) {
            if(i + 1 < mArgc) {
                m_width = atoi(mArgv[i + 1].c_str()); /// TO DO implement to_string for linux
            } else {
                return false;
            }
        } else if(mArgv[i] == std::string("-h") || mArgv[i] == std::string("--height")) {
            if(i + 1 < mArgc) {
                m_height = atof(mArgv[i + 1].c_str());
            } else {
                return false;
            }
        } else if(mArgv[i] == std::string("-fp") || mArgv[i] == std::string("--fpixelx")) {
            if(i + 1 < mArgc) {
                m_fPixelX = atoi(mArgv[i + 1].c_str());
            } else {
                return false;
            }
        } else if(mArgv[i] == std::string("-sp") || mArgv[i] == std::string("--spixelx")) {
            if(i + 1 < mArgc) {
                m_sPixelX = atoi(mArgv[i + 1].c_str());
            } else {
                return false;
            }
        }
        else if(mArgv[i] == std::string("-wa") || mArgv[i] == std::string("--wast")) {
            if(i + 1 < mArgc) {
                m_wast = atof(mArgv[i + 1].c_str());
            } else {
                return false;
            }
        }
    }
    return true;
}

bool ArgumentParser::getPreproc() const
{
    return m_preproc;
}

bool ArgumentParser::getEar() const
{
    return m_earDetect;
}

bool ArgumentParser::getNose() const
{
    return m_noseDetect;
}

bool ArgumentParser::getFlip() const
{
    return m_FlipImage;
}


bool ArgumentParser::isValid() const
{
    return m_isValid;
}


int ArgumentParser::getWidth() const
{
    return m_width;
}

std::string ArgumentParser::getFrontPath() const
{
    return m_frontalPath;
}

std::string ArgumentParser::getSidePath() const
{
    return m_sidePath;
}

float ArgumentParser::getHeight() const
{
    return m_height;
}

float ArgumentParser::getWast() const
{
   return m_wast;
}

int ArgumentParser::getFPixelX() const
{
    return m_fPixelX;
}

int ArgumentParser::getSPixelX() const
{
    return m_sPixelX;
}
