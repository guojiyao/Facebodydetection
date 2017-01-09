#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H

#include <string>
#include <iostream>
#include <vector>

class ArgumentParser
{
public:
    ArgumentParser(int argc, char *argv[]);

    bool isValid() const;
    bool getPreproc() const;
    bool getEar() const;
    bool getNose() const;
    bool getFlip() const;
    int getWidth() const;
    float getHeight() const;
    float getWast() const;
    int getFPixelX() const;
    int getSPixelX() const;
    std::string getFrontPath() const;
    std::string getSidePath() const;

    std::string help()
    {
       std::string h =
               "-f|--front <ImgPath> "
               "-e|--ear t/f "
               "-n|--nose t/f"
               "-fl|--flip t/f"
               "[-s|--side <ImgPath>] [-p]"
               "[-w --width <width to resize all images to before processing>]"
               "-h --height <height to resize after segmentation>"
               "-fp --fpixelx <width in pixels to resize frontal images for joints detection>"
               "-sp --spixelx <width in pixels to resize sideview image for joints detection>"
               "-wa --wast <wast variable>";
       return h;
    }

private:
    bool parse();

 private:
    bool m_preproc;
    std::string m_frontalPath;
    std::string m_sidePath;
    int m_width;

    float m_height;
    int m_fPixelX;
    int m_sPixelX;
    float m_wast;

    bool m_isValid;
    bool m_earDetect;
    bool m_noseDetect;
    bool m_FlipImage;
    int mArgc;
    std::vector<std::string> mArgv;
public:
    static const std::string s_invalidPath;// = "";
    static const int s_invalidNumericalValue;// = -1;
};

#endif // ARGUMENTPARSER_H
