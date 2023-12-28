[![CI testing](https://github.com/Lecrapouille/zipper/actions/workflows/ci.yaml/badge.svg)](https://github.com/Lecrapouille/zipper/actions/workflows/ci.yaml)

![Zipper](doc/logo.png)

[Zipper](https://github.com/lecrapouille/zipper) is a C++11 wrapper around minizip compression library. Its goal is to bring the power and simplicity of minizip to a more object-oriented/c++ user-friendly library.

## Fork Information

This fork is a simplified, cmake-ified version designed to be included as a submodule to a larger linux-based project. It does not support AES encryption.
First, you must initialize the zipper submodule (minizip) via `git submodule init`. Add the top level directory to your project via `add_subdirectory(zipper)`. 
This will create a target `zipper` that you can depend on.

## API

There are two classes available `Zipper` and `Unzipper`. They behave in the same manner regarding constructors and storage parameters.

### Zipping API

#### Header

```c++
#include "Zipper.hpp"
using namespace zipper;
```

#### Constructor

- Constructor without password and replace `ziptest.zip` if already present. The new zip archive is dummy.

```c++
Zipper zipper("ziptest.zip", Zipper::openFlags::Overwrite);
// Or simply: zipper("ziptest.zip");
```

- Constructor without password and preserve `ziptest.zip` if already present.

```c++
Zipper zipper("ziptest.zip", Zipper::openFlags::Append);
```

- Constructor with password (using AES algorithm) and replace `ziptest.zip` if already present. The new zip archive is dummy.

```c++
Zipper zipper("ziptest.zip", "mypassword", Zipper::openFlags::Overwrite);
// Or simply: zipper("ziptest.zip");
```

- Constructor with a password and preserve `ziptest.zip` if already present.

```c++
Zipper zipper("ziptest.zip", "mypassword", Zipper::openFlags::Append);
```

- Note: all constructors will throw `std::run_time` exception in case of failure.

```c++
try
{
    Zipper zipper("ziptest.zip", ...);
    ...
}
catch (std::run_time const& e)
{
    std::cerr << e.what() << std::endl;
}
```

#### Destructor / closing / reopening

Do not forget to call explicitly `close()` (called implicitly from its destructor) else
the zip will not be well formed and `Unzipper` will fail to open it for example.

```c++
Zipper zipper("ziptest.zip", ...);
...
zipper.close(); // Now Unzipper unzipper("ziptest.zip") can work
```

After `close()` you can reopen the zip archive with `open()`. The implicit option
`Zipper::openFlags::Append` is to preserve the zip content. To replace the zip
file is to use `Zipper::openFlags::Overwrite`.

```c++
Zipper zipper("ziptest.zip", ...);
...
zipper.close();
...

zipper.open(); // equivalent to zipper.open(Zipper::openFlags::Append);
// or zipper.open(Zipper::openFlags::Overwrite);
...
zipper.close();
```

In case of success the `open()` will return `true` else will return `false` and the
`error()` should be used for getting the [std::error_code](https://akrzemi1.wordpress.com/2017/07/12/your-own-error-code/).

```c++
if (!zipper.open())
{
    std::cerr << zipper.error().message() << std::endl;
}
```

#### Appending files or folders inside the archive.

The `add()` method allows appending files or folders. The `Zipper::zipFlags::Better` is set implicitly. Other options are (in the last option in the arguments):
- Store only: `Zipper::zipFlags::Store`.
- Compress faster, less compressed: `Zipper::zipFlags::Faster`.
- Compress intermediate time/compression: `Zipper::zipFlags::Medium`.
- Compress faster better: `Zipper::zipFlags::Better`.

In case of success the `open()` will return `true` else will return `false` and the `error()` should be used for getting the std::error_code.

- Adding an entire folder to a zip:

```c++
Zipper zipper("ziptest.zip");
zipper.add("myFolder/");
zipper.close();
```

- Adding a file by name:

```c++
Zipper zipper("ziptest.zip");
zipper.add("somefile.txt");
zipper.close();
```

- You can change their name in the archive:

```c++
Zipper zipper("ziptest.zip");
zipper.add("somefile.txt", "new_name_in_archive");
zipper.close();
```

- Create a zip file with 2 files referred by their `std::ifstream` and change their name in the archive:

```c++
std::ifstream input1("first_file");
std::ifstream input2("second_file");

Zipper zipper("ziptest.zip");
zipper.add(input1, "Test1");
zipper.add(input2, "Test2");
zipper.close();
```

- Zipper has security against [Zip Slip vulnerability](https://security.snyk.io/research/zip-slip-vulnerability).

```c++
zipper.add(input1, "../Test1");
```

Will always return `false` because `Test1` will be extracted outside the destination folder. This prevents malicious attack from replacing your password files:

```c++
zipper.add(malicious_passwd, "../../../../../../../../../../../../../../../etc/passwd");
```

Becase in Unix try to go outside the root folder `/` will stay in the root folder. Example
```bash
cd /
pwd
cd ../../../../../../../../../../../../../../..
pwd
```

- The Zipper lib force canonical paths in the archive. The following code works (will return `true`):
```c++
zipper.add(input1, "foo/../Test1");
```

because `foo/../Test1` is replaced by `Test1` (even if the folder `foo` is not present in the
zip archive.

#### Vector and stream

- Creating a zip file using the awesome streams from the [boost](https://www.boost.org/) library that lets us use a vector as a stream:

```c++
#include <boost/interprocess/streams/vectorstream.hpp>
...

boost::interprocess::basic_vectorstream<std::vector<char>> input_data(some_vector);

Zipper zipper("ziptest.zip");
zipper.add(input_data, "Test1");
zipper.close();
```

- Creating a zip in a vector with files:

```c++
#include <boost/interprocess/streams/vectorstream.hpp>
...

boost::interprocess::basic_vectorstream<std::vector<char>> zip_in_memory;
std::ifstream input1("some file");

Zipper zipper(zip_in_memory); // You can pass password
zipper.add(input1, "Test1");
zipper.close();

zipper::Unzipper unzipper(zip_in_memory);
unzipper.extractEntry(...
```

Or:

```c++
#include <vector>

std::vector<unsigned char> zip_vect;
std::ifstream input1("some file");

Zipper zipper(zip_vect); // You can pass password
zipper.add(input1, "Test1");
zipper.close();
```

- Creating a zip in-memory stream with files:

```c++
std::stringstream ss;
std::ifstream input1("some file");

Zipper zipper(ss); // You can pass password
zipper.add(input1, "Test1");
zipper.close();

zipper::Unzipper unzipper(ss);
unzipper.extractEntry(...
```

### Unzipping API

#### Header

```c++
#include <Zipper/Unzipper.hpp>
using namespace zipper;
```

#### Constructor

- Constructor without password and opening `ziptest.zip` (shall be already present).

```c++
Zipper unzipper("ziptest.zip");
...
zipper.close();
```

- Constructor with a password and opening `ziptest.zip` (shall be already present).

```c++
Zipper unzipper("ziptest.zip", "mypassword");
...
zipper.close();
```

- Note: all constructors will throw `std::run_time` exception in case of failure.

```c++
try
{
    Zipper zipper("ziptest.zip", ...);
    ...
}
catch (std::run_time const& e)
{
    std::cerr << e.what() << std::endl;
}
```

#### Getting the list of zip entries

```c++
Unzipper unzipper("zipfile.zip");
std::vector<ZipEntry> entries = unzipper.entries();
for (auto& it: unzipper.entries())
{
    std::cout << it.name << ": "
              << it.timestamp
              << std::endl;
}
unzipper.close();
```

#### Extracting all entries from the zip file

Two methods `extract()` for the whole archive and `extractEntry()` for
a single element in the archive. They return `true` in case of success
or `false` in case of failure. In case of failure, use `unzipper.error();`
to get the `std::error_code`.

- If you do not care about replacing existing files or folders:

```c++
Unzipper unzipper("zipfile.zip");
unzipper.extractAll(true);
unzipper.close();
```

- If you care about replacing existing files or folders. The method will fail (return `false`)
if a file is replaced.

```c++
Unzipper unzipper("zipfile.zip");
unzipper.extractAll(); // equivalent to unzipper.extractAll(false);
unzipper.close();
```

- Extracting all entries from the zip file to the desired destination:

```c++
Unzipper unzipper("zipfile.zip");
unzipper.extractAll("/the/destination/path");        // Fail if a file exists (false argument is implicit)
unzipper.extractAll("/the/destination/path", true);  // Replace existing files
unzipper.close();
```

- Extracting all entries from the zip file using alternative names for existing files on disk:

```c++
std::map<std::string, std::string> alternativeNames = { {"Test1", "alternative_name_test1"} };
Unzipper unzipper("zipfile.zip");
unzipper.extractAll(".", alternativeNames);
unzipper.close();
```

- Extracting a single entry from the zip file:

```c++
Unzipper unzipper("zipfile.zip");
unzipper.extractEntry("entry name");
unzipper.close();
```

Return `true` in case of success or `false` in case of failure.
In case of failure, use `unzipper.error();` to get the `std::error_code`.

- Extracting a single entry from the zip file to destination:

```c++
Unzipper unzipper("zipfile.zip");
unzipper.extractEntry("entry name", "/the/destination/path"); // Fail if a file exists (false argument is implicit)
unzipper.extractEntry("entry name", "/the/destination/path", true); // Replace existing file
unzipper.close();
```

Return `true` in case of success or `false` in case of failure.
In case of failure, use `unzipper.error();` to get the `std::error_code`.

- Extracting a single entry from the zip file to memory:

```c++
std::vector<unsigned char> unzipped_entry;
Unzipper unzipper("zipfile.zip");
unzipper.extractEntryToMemory("entry name", unzipped_entry);
unzipper.close();
```

Return `true` in case of success or `false` in case of failure.
In case of failure, use `unzipper.error();` to get the `std::error_code`.

- Extracting from a vector:

```c++

std::vector<unsigned char> zip_vect; // Populated with Zipper zipper(zip_vect);

Unzipper unzipper(zip_vect);
unzipper.extractEntry("Test1");
```

- Zipper has security against [Zip Slip vulnerability](https://security.snyk.io/research/zip-slip-vulnerability): if an entry has a path outside the extraction folder (like `../foo.txt`) it
will returns `false` even if the replace option is set.

## Hello zip

Basic unzipper standalone application given as demo in [doc/demos](doc/demos).

## For developers

### Non regression tests

Depends on:
- [googletest](https://github.com/google/googletest) framework
- lcov for code coverage:

Two ways of running them:
- From the root folder:
```shell
make check -j`nproc --all`
```

- From the tests/ folder:
```shell
cd tests
make coverage -j`nproc --all`
```

A coverage report has created an opening. If you do not desire to generate the report.
```shell
cd tests
make -j`nproc --all`
./build/Zipper-UnitTest
```

## FAQ

- Q: I used a password when zipping with the Zipper lib, but now when I want to extract data with my operating system zip tool, I got an error.
  A: By default, Zipper encrypts with the EAS algorithm which is not the default encryption algorithm for zip files. Your operating system zip tools seem
  not to understand EAS. You can extract it with the 7-Zip tool: `7za e your.zip`. If you want the default zip encryption (at your own risk since the
  password can be cracked) you can remove EAS option in the following files: [Make](Make) and [external/compile-external-libs.sh](external/compile-external-libs.sh)
  and recompile the Zipper project.

