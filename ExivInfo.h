#ifndef EXIVINFO_H
#define EXIVINFO_H

#include <map>
#include <iostream>
#include <string>

#include <exiv2/exiv2.hpp>

class ExivInfo
{

public:
    ExivInfo(std::string RawFilePath);
    bool IsValid();
    bool IsEmpty();
    int GetExposure();
    int GetISOSpeed();
    long GetTimestamp();
    float GetAperture();
    std::string GetMIMEType();
    std::string GetCopyright();
    std::string GetCameraMake();
    std::string GetCameraModel();
    bool GetSize(int &width, int &Height);
    bool GetExifKey(std::string key, std::string &String);
    int  GetAllInfo(std::map<std::string, std::string> &Map);

private:
    bool m_Valid;
    bool m_Empty;
    Exiv2::Image::AutoPtr m_Image;
    Exiv2::ExifData *m_ExivInfo;
    std::map<std::string, std::string> ExivAllInfo;
};

#endif // EXIVINFO_H
