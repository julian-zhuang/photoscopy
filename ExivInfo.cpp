#include "ExivInfo.h"
#include <QDateTime>

ExivInfo::ExivInfo(std::string RawFilePath)
{
    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);

    m_Image = Exiv2::ImageFactory::open(RawFilePath);
    if (m_Image.get() == nullptr){
        m_Valid = false;
        return;
    }
    m_Valid = true;

    m_Image->readMetadata();
    Exiv2::ExifData &exifData = m_Image->exifData();
    m_ExivInfo = &exifData;
    if (exifData.empty() == true){
        m_Empty = true;
    }else{
        m_Empty = false;
    }
}


bool ExivInfo::IsValid()
{
    return m_Valid;
}

bool ExivInfo::IsEmpty()
{
    return m_Empty;
}

std::string ExivInfo::GetMIMEType()
{
    std::string Str = "";
    GetExifKey("Exif.Image.Make", Str);
    return Str;
}

bool ExivInfo::GetSize(int &width, int &Height)
{
    std::string Value;

    if (GetExifKey("Exif.Photo.PixelXDimension", Value) == false){
        width = std::stoi(Value);
        return false;
    }
    if (GetExifKey("Exif.Photo.PixelYDimension", Value) == false){
        Height = std::stoi(Value);
        return false;
    }
    return true;
}

std::string ExivInfo::GetCameraMake()
{
    std::string Value;
    GetExifKey("Exif.Image.Make", Value);
    return Value;
}

std::string ExivInfo::GetCameraModel()
{
    std::string Value;
    GetExifKey("Exif.Image.Model", Value);
    return Value;
}

long int ExivInfo::GetTimestamp()
{
    std::string Value;
    long int TimeStmap = 0;
    if (GetExifKey("Exif.Image.DateTime", Value) == true){
        TimeStmap = QDateTime::fromString(QString(Value.c_str()), "yyyy:MM:dd hh:mm:ss").toTime_t();
    }
    return TimeStmap;
}

int ExivInfo::GetExposure()
{
    std::string Value;
    long int TimeStmap = 0;
    if (GetExifKey("Exif.Photo.ExposureTime", Value) == true){
        TimeStmap = std::stoi(Value);
    }
    return TimeStmap;
}

float ExivInfo::GetAperture()
{
    std::string Value;
    float Aperture = 0.0;
    if (GetExifKey("Exif.Photo.FNumber", Value) == true){
        Aperture = std::stof(Value);
    }
    return Aperture;
}

int ExivInfo::GetISOSpeed()
{
    std::string Value;
    int ISOSpeed = 0;
    if (GetExifKey("Exif.Photo.ISOSpeedRatings", Value) == true){
        ISOSpeed = std::stoi(Value);
    }
    return ISOSpeed;
}

std::string ExivInfo::GetCopyright()
{
    std::string Str = "";
    GetExifKey("Exif.Image.Copyright", Str);
    return Str;
}

int ExivInfo::GetAllInfo(std::map<std::string, std::string> &Map)
{
    Map.clear();
    for (Exiv2::ExifData::iterator tmp = m_ExivInfo->begin(); tmp != m_ExivInfo->end(); tmp++) {
        Map[tmp->key()] = tmp->value().toString();
    }
    return Map.size();
}

bool ExivInfo::GetExifKey(std::string key, std::string &String)
{
    Exiv2::ExifKey tmp = Exiv2::ExifKey(key);
    Exiv2::ExifData::iterator pos = m_ExivInfo->findKey(tmp);
    if (pos == m_ExivInfo->end()) {
        return false;
    }
    String = pos->value().toString();
    return true;
}


