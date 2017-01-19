#ifndef XML_READER_HPP
#define XML_READER_HPP

#include <string>

#define MAX_FILE_SIZE       ( 1024 * 1024 )
#define DEFAULT_ROOT_NAME   "ROOT"

class XMLReader
{
public:

    // Enum used for returning values from functions
    enum class Status : unsigned int
    {
        Ok = 0,
        InvalidPath,
        FileTooBig,
        FileEmpty,
        ReadingError,
        NoRootNode,
        MissingParameter,
        MissingValue,
    };

    // Prints message corresponding to given status code
    static std::string PrintStatus( Status status );

    // Constructor
    XMLReader();

    // Destructor
    ~XMLReader();

    // Loads xml file's content
    Status LoadFile( /*in*/ const std::string& filePath, /*in*/ const std::string& rootName = DEFAULT_ROOT_NAME );

    // Return current root node
    std::string GetRootNode();

    // Set root node
    Status SetRootNode( /*in*/ const std::string& rootName );

    // Obtains value of parameter which name is passed as argument
    Status GetParameter( /*in*/ const std::string& name, /*out*/ bool& value ) const;
    Status GetParameter( /*in*/ const std::string& name, /*out*/ char& value ) const;
    Status GetParameter( /*in*/ const std::string& name, /*out*/ unsigned char& value ) const;
    Status GetParameter( /*in*/ const std::string& name, /*out*/ short& value ) const;
    Status GetParameter( /*in*/ const std::string& name, /*out*/ unsigned short& value ) const;
    Status GetParameter( /*in*/ const std::string& name, /*out*/ int& value ) const;
    Status GetParameter( /*in*/ const std::string& name, /*out*/ unsigned int& value ) const;
    Status GetParameter( /*in*/ const std::string& name, /*out*/ long long& value ) const;
    Status GetParameter( /*in*/ const std::string& name, /*out*/ unsigned long long& value ) const;
    Status GetParameter( /*in*/ const std::string& name, /*out*/ float& value ) const;
    Status GetParameter( /*in*/ const std::string& name, /*out*/ double& value ) const;
    Status GetParameter( /*in*/ const std::string& name, /*out*/ std::string& value ) const;

    // Clears saved contents and root node
    void Clear();

private:
    Status InnerGetParameter( /*in*/ const std::string& name, /*out*/ std::string& value ) const;

    template<typename T>
    Status InnerGetParameter( /*in*/ const std::string& name, /*in*/ const std::string& format, /*out*/ T& value ) const;

private:
    std::string m_fileContent;
    std::string m_rootName;
    std::string m_rootContent;
};

#endif // XML_READER_HPP