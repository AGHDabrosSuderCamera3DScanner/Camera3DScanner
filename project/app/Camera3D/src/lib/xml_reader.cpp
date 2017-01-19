#include "xml_reader.hpp"
#include <cstdio>
#include <memory>

std::string XMLReader::PrintStatus( XMLReader::Status status )
{
    const std::string DEFAULT_MESSAGE = "Unknown XML error";

    switch( status )
    {
        case XMLReader::Status::Ok:                 return "Ok";
        case XMLReader::Status::InvalidPath:        return "Invalid path";
        case XMLReader::Status::FileTooBig:         return "File is too big";
        case XMLReader::Status::FileEmpty:          return "File is empty";
        case XMLReader::Status::ReadingError:       return "Reading error occured";
        case XMLReader::Status::NoRootNode:         return "No root node found";
        case XMLReader::Status::MissingParameter:   return "Parameter is missing";
        case XMLReader::Status::MissingValue:       return "Value is missing";
        default:                                    return DEFAULT_MESSAGE;
    }
    return DEFAULT_MESSAGE;
}

XMLReader::XMLReader()
    : m_fileContent( "" )
    , m_rootName( "" )
    , m_rootContent( "" )
{
}

XMLReader::~XMLReader()
{
}

XMLReader::Status XMLReader::LoadFile( /*in*/ const std::string& filePath, /*in*/ const std::string& rootName )
{
    FILE * pFile = nullptr;
    size_t fileSize = 0;
    std::shared_ptr<char> buffer( new char[ MAX_FILE_SIZE ] );

    // Open file
    pFile = fopen( filePath.c_str(), "rb" );
    if( pFile == nullptr )
    {
        return XMLReader::Status::InvalidPath;
    }

    // Get file size
    fseek( pFile, 0, SEEK_END );
    fileSize = ftell( pFile );
    fseek( pFile, 0, SEEK_SET );

    // Check whether file is too big
    if( fileSize > MAX_FILE_SIZE )
    {
        fclose( pFile );
        return XMLReader::Status::FileTooBig;
    }

    // Check if file is empty
    if( fileSize == 0 )
    {
        fclose( pFile );
        return XMLReader::Status::FileEmpty;
    }

    // Read file's content
    if( fread( buffer.get(), 1, fileSize, pFile ) < fileSize )
    {
        fclose( pFile );
        return XMLReader::Status::ReadingError;
    }

    // Close file
    fclose( pFile );

    // Save file content
    m_fileContent.assign( buffer.get(), fileSize );

    // Set root node   
    XMLReader::Status result = SetRootNode( rootName.empty() ? DEFAULT_ROOT_NAME : rootName );
    if( result != XMLReader::Status::Ok )
    {
        return result;
    }

    return XMLReader::Status::Ok;
}

std::string XMLReader::GetRootNode()
{
    return m_rootName;
}

XMLReader::Status XMLReader::SetRootNode( /*in*/ const std::string& rootName )
{
    // Find root node
    size_t begin = m_fileContent.find( "<" + rootName + ">" );
    size_t end = m_fileContent.find( "</" + rootName + ">" );
    if( begin == std::string::npos || end == std::string::npos )
    {
        return XMLReader::Status::NoRootNode;
    }

    // Save new root node
    m_rootName = rootName;

    // Get content of root node
    m_rootContent = m_fileContent.substr( begin, end - begin );

    return XMLReader::Status::Ok;
}

XMLReader::Status XMLReader::GetParameter( const std::string & name, bool & value ) const
{
    std::string innerValue = "";

    // Get parameter's value as string
    XMLReader::Status result = InnerGetParameter( name, innerValue );
    if( result != XMLReader::Status::Ok )
    {
        return result;
    }

    // Parse parameter's boolean value 'true'
    if( innerValue.compare( "TRUE" ) == 0 ||
        innerValue.compare( "true" ) == 0 ||
        innerValue.compare( "1" ) == 0 )
    {
        value = true;
        return XMLReader::Status::Ok;
    }

    // Parse parameter's boolean value 'false'
    if( innerValue.compare( "FALSE" ) == 0 ||
        innerValue.compare( "false" ) == 0 ||
        innerValue.compare( "0" ) == 0 )
    {
        value = false;
        return XMLReader::Status::Ok;
    }

    return XMLReader::Status::MissingValue;
}

XMLReader::Status XMLReader::GetParameter( /*in*/ const std::string& name, /*out*/ char& value ) const
{
    return InnerGetParameter<char>( name, "%hhd", value );
}

XMLReader::Status XMLReader::GetParameter( /*in*/ const std::string& name, /*out*/ unsigned char& value ) const
{
    return InnerGetParameter<unsigned char>( name, "%hhu", value );
}

XMLReader::Status XMLReader::GetParameter( /*in*/ const std::string& name, /*out*/ short& value ) const
{
    return InnerGetParameter<short>( name, "%hd", value );
}

XMLReader::Status XMLReader::GetParameter( /*in*/ const std::string& name, /*out*/ unsigned short& value ) const
{
    return InnerGetParameter<unsigned short>( name, "%hu", value );
}

XMLReader::Status XMLReader::GetParameter( /*in*/ const std::string& name, /*out*/ int& value ) const
{
    return InnerGetParameter<int>( name, "%d", value );
}

XMLReader::Status XMLReader::GetParameter( /*in*/ const std::string& name, /*out*/ unsigned int& value ) const
{
    return InnerGetParameter<unsigned int>( name, "%u", value );
}

XMLReader::Status XMLReader::GetParameter( /*in*/ const std::string& name, /*out*/ long long& value ) const
{
    return InnerGetParameter<long long>( name, "%lld", value );
}

XMLReader::Status XMLReader::GetParameter( /*in*/ const std::string& name, /*out*/ unsigned long long& value ) const
{
    return InnerGetParameter<unsigned long long>( name, "%llu", value );
}

XMLReader::Status XMLReader::GetParameter( /*in*/ const std::string& name, /*out*/ float& value ) const
{
    return InnerGetParameter<float>( name, "%f", value );
}

XMLReader::Status XMLReader::GetParameter( /*in*/ const std::string& name, /*out*/ double& value ) const
{
    return InnerGetParameter<double>( name, "%lf", value );
}

XMLReader::Status XMLReader::GetParameter( /*in*/ const std::string& name, /*out*/ std::string& value ) const
{
    return InnerGetParameter( name, value );
}

void XMLReader::Clear()
{
    m_fileContent = "";
    m_rootName = "";
    m_rootContent = "";
}

XMLReader::Status XMLReader::InnerGetParameter( /*in*/ const std::string& name, /*out*/ std::string& value ) const
{
    // Find parameter
    size_t begin = m_rootContent.find( "<" + name + ">" );
    size_t end = m_rootContent.find( "</" + name + ">" );
    if( begin == std::string::npos || end == std::string::npos )
    {
        return XMLReader::Status::MissingParameter;
    }

    // Get parameter's value
    value = m_rootContent.substr( begin + 2 + name.size(), end - begin - 2 - name.size() );
    return XMLReader::Status::Ok;
}

template<typename T>
XMLReader::Status XMLReader::InnerGetParameter( /*in*/ const std::string& name, /*in*/ const std::string& format, /*out*/ T& value ) const
{
    std::string innerValue = "";
    T finalValue = 0;

    // Get parameter's value as string
    XMLReader::Status result = InnerGetParameter( name, innerValue );
    if( result != XMLReader::Status::Ok )
    {
        return result;
    }

    // Parse parameter's value
    if( sscanf( innerValue.c_str(), format.c_str(), &finalValue ) < 1 )
    {
        return XMLReader::Status::MissingValue;
    }

    value = finalValue;
    return XMLReader::Status::Ok;
}
