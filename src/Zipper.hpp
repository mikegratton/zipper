//-----------------------------------------------------------------------------
// Copyright (c) 2022 Quentin Quadrat <lecrapouille@gmail.com>
// https://github.com/Lecrapouille/zipper distributed under MIT License.
// Based on https://github.com/sebastiandev/zipper/tree/v2.x.y distributed under
// MIT License. Copyright (c) 2015 -- 2022 Sebastian <devsebas@gmail.com>
//-----------------------------------------------------------------------------

#ifndef ZIPPER_ZIPPER_HPP
#  define ZIPPER_ZIPPER_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <ctime>
#include <system_error>

namespace zipper {

// *************************************************************************
//! \brief Zip archive compressor.
// *************************************************************************
class Zipper
{
public:

    // -------------------------------------------------------------------------
    //! \brief Archive flags.
    // -------------------------------------------------------------------------
    enum openFlags
    {
        //! \brief Overwrite existing file.zip
        Overwrite,
        //! \brief Append to existing file.zip
        Append
    };

    // -------------------------------------------------------------------------
    //! \brief Compression options for files.
    // -------------------------------------------------------------------------
    enum zipFlags
    {
        //! \brief Minizip options/params: -0  Store only
        Store = 0x00,
        //! \brief Minizip options/params: -1  Compress faster
        Faster = 0x01,
        //! \brief Minizip options/params: -5  Compress medium
        Medium = 0x05,
        //! \brief Minizip options/params: -9  Compress better
        Better = 0x09,
        //! \brief ???
        SaveHierarchy = 0x40
    };

    // -------------------------------------------------------------------------
    //! \brief Regular zip compression (inside a disk zip archive file) with a
    //! password.
    //!
    //! \param[in] zipname: the path where to create your zip file.
    //! \param[in] password: optional password (set empty for not using password).
    //! \param[in] flags: Overwrite (default) or append existing zip file (zipname).
    //! \throw std::runtime_error if something odd happened.
    // -------------------------------------------------------------------------
    Zipper(const std::string& zipname, const std::string& password,
           Zipper::openFlags flags = Zipper::openFlags::Overwrite);

    // -------------------------------------------------------------------------
    //! \brief Regular zip compression (inside a disk zip archive file) without
    //! password.
    //!
    //! \param[in] zipname: the path where to create your zip file.
    //! \param[in] flags: Overwrite (default) or append existing zip file (zipname).
    //! \throw std::runtime_error if something odd happened.
    // -------------------------------------------------------------------------
    Zipper(const std::string& zipname, Zipper::openFlags flags = Zipper::openFlags::Overwrite)
        : Zipper(zipname, std::string(), flags)
    {}

    // -------------------------------------------------------------------------
    //! \brief In-memory zip compression (storage inside std::iostream).
    //!
    //! \param[in] buffer: the stream in which to store zipped files.
    //! \param[in] password: optional password (set empty for not using password).
    //! \throw std::runtime_error if something odd happened.
    // -------------------------------------------------------------------------
    Zipper(std::iostream& buffer, const std::string& password = std::string());

    // -------------------------------------------------------------------------
    //! \brief In-memory zip compression (storage inside std::vector).
    //!
    //! \param[in] buffer: the vector in which to store zipped files.
    //! \param[in] password: optional password (set empty for not using password).
    //! \throw std::runtime_error if something odd happened.
    // -------------------------------------------------------------------------
    Zipper(std::vector<unsigned char>& buffer,
           const std::string& password = std::string());

    // -------------------------------------------------------------------------
    //! \brief Call close().
    // -------------------------------------------------------------------------
    ~Zipper();

    // -------------------------------------------------------------------------
    //! \brief Compress data \c source with a given timestamp in the archive
    //! with the given name \c nameInZip.
    //!
    //! \param[in,out] source: data to compress.
    //! \param[in] timestamp: the desired timestamp.
    //! \param[in] nameInZip: the desired name for \c source inside the archive.
    //! \param[in] flags: compression options (faster, better ...).
    //! \return true on success, else return false.
    //! \throw std::runtime_error if something odd happened.
    // -------------------------------------------------------------------------
    bool add(std::istream& source, const std::tm& timestamp, const std::string& nameInZip,
             Zipper::zipFlags flags = Zipper::zipFlags::Better);

    // -------------------------------------------------------------------------
    //! \brief Compress data \c source in the archive with the given name \c
    //! nameInZip. No timestamp will be stored.
    //!
    //! \param[in,out] source: data to compress.
    //! \param[in] nameInZip: the desired name for \c source inside the archive.
    //! \param[in] flags: compression options (faster, better ...).
    //! \return true on success, else return false.
    //! \throw std::runtime_error if something odd happened.
    // -------------------------------------------------------------------------
    bool add(std::istream& source, const std::string& nameInZip,
             Zipper::zipFlags flags = Zipper::zipFlags::Better);

    bool add(std::istream& source,
             Zipper::zipFlags flags = Zipper::zipFlags::Better)
    {
        return add(source, std::string(), flags);
    }

    // -------------------------------------------------------------------------
    //! \brief Compress a folder or a file in the archive.
    //!
    //! \param[in,out] fileOrFolderPath: file or folder to compress.
    //! \param[in] flags: compression options (faster, better ...).
    //! \return true on success, else return false.
    //! \throw std::runtime_error if something odd happened.
    // -------------------------------------------------------------------------
    bool add(const std::string& fileOrFolderPath,
             Zipper::zipFlags flags = Zipper::zipFlags::Better);


    bool add(const char* fileOrFolderPath,
             Zipper::zipFlags flags = Zipper::zipFlags::Better)
    {
        return add(std::string(fileOrFolderPath), flags);
    }

    // -------------------------------------------------------------------------
    //! \brief Depending on your selection of constructor, this method will do
    //! some actions such as closing the access to the zip file, flushing in the
    //! stream, releasing memory ...
    //! \note this method is called by the destructor.
    // -------------------------------------------------------------------------
    void close();

    // -------------------------------------------------------------------------
    //! \brief To be called after a close(). Depending on your selection of
    //! constructor, this method will do some actions such as opening the zip
    //! file, reserve buffers.
    //! \param[in] flags: Overwrite or append (default) existing zip file (zipname).
    //! \note this method is not called by the constructor.
    // -------------------------------------------------------------------------
    bool open(Zipper::openFlags flags = Zipper::openFlags::Append);

    // -------------------------------------------------------------------------
    //! \brief Get the error information when a method returned false.
    // -------------------------------------------------------------------------
    std::error_code const& error() const;

private:

    void release();

private:

    std::iostream& m_obuffer;
    std::vector<unsigned char>& m_vecbuffer;
    std::string m_zipname;
    std::string m_password;
    bool m_usingMemoryVector;
    bool m_usingStream;
    bool m_open;
    std::error_code m_error_code;

    struct Impl;
    Impl* m_impl;
};

} // namespace zipper

#endif // ZIPPER_ZIPPER_HPP
