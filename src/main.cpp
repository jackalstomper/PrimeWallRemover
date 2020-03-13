#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <cstring>

#if __GNUC__ && __GNUC__ < 8
#   include <experimental/filesystem>
#else
#   include <filesystem>
#endif

#if _WIN32 && UNICODE
#   include <codecvt>
#endif

#include "nod/nod.hpp"
#include "nod/DiscBase.hpp"
#include "nod/DiscGCN.hpp"
#include "prime_files/PAK.hpp"
#include "prime_files/MREA.hpp"
#include "WallRemover.hpp"

int main(int argc, char** argv)
{
#if __GNUC__ && __GNUC__ < 8
    namespace fs = std::experimental::filesystem;
#else
    namespace fs = std::filesystem;
#endif

    std::string isoLocation;
    const std::string outputDirectory("discExtract");

    if (argc <= 1) {
        std::cout << "Enter ISO location:\n> " << std::flush;
        std::cin >> isoLocation;
    } else {
        isoLocation = std::string(argv[1]);
    }

    if (isoLocation.empty() || !fs::exists(isoLocation)) {
        std::cerr << "Invalid ISO Location" << std::endl;
        return 1;
    }

#if _WIN32 && UNICODE
    std::wstring_convert<std::codecvt_utf8_utf16<std::wstring::value_type>> converter;
    std::unique_ptr<nod::DiscBase> disc = nod::OpenDiscFromImage(converter.from_bytes(isoLocation));
#else
    std::unique_ptr<nod::DiscBase> disc = nod::OpenDiscFromImage(isoLocation);
#endif

    nod::IPartition* dataPart = disc->getDataPartition();
    if (!dataPart)
        return 1;

    bool isPrime = strncmp(dataPart->getHeader().m_gameID, "GM8E01", 6) == 0;
    if (!isPrime) {
        std::cerr << "Provided ISO is not a Metroid prime disc" << std::endl;
        return 1;
    }

    if (fs::is_directory(outputDirectory)) {
        std::cout << outputDirectory << " already exists. Clearing." << std::endl;
        fs::remove_all(outputDirectory);
    }

    std::cout << "Creating " << outputDirectory << std::endl;
    fs::create_directory(outputDirectory);

    std::cout << "Extracting disc to " << outputDirectory << std::endl;
    nod::ExtractionContext ctx = {false, [](std::string_view str, float c) {
        std::cout << "\rProgress: " << std::to_string(static_cast<uint32_t>(c * 100.f)) << '%' << std::flush;
    }};

#if _WIN32 && UNICODE
    dataPart->extractToDirectory(converter.from_bytes(outputDirectory), ctx);
#else
    dataPart->extractToDirectory(outputDirectory, ctx);
#endif

    std::cout << "\nDone.\nRemoving Walls..." << std::endl;
    for (int i = 1; i < 9; ++i) {
        std::string pakName("Metroid" + std::to_string(i) + ".pak");
        std::fstream file(outputDirectory + "/files/" + pakName, std::ios::binary | std::ios::in | std::ios::out);
        if (file.is_open()) {
            PAK::Index pakIndex;
            pakIndex.read(file);
            std::cout << "Modding " << pakName << " ..." << std::flush;
            for (const auto& r : pakIndex.resources) {
                if (r.assetType == "MREA") {
                    file.seekg(r.offset());
                    std::vector<char> buff(r.size());
                    file.read(buff.data(), r.size());
                    PWR::removeWalls(buff);
                    file.seekp(r.offset());
                    file.write(buff.data(), r.size());
                }
            }

            std::cout << "Done." << std::endl;
        }
    }

    std::cout << "Rebuilding ISO as prime-nowalls.iso ..." << std::endl;
    auto progFunc = [](float totalProg, nod::SystemStringView fileName, size_t fileBytesXfered) {
        std::cout << "\rProgress: " << std::to_string(static_cast<uint32_t>(totalProg * 100.f)) << '%' << std::flush;
    };

#if _WIN32 && UNICODE
    std::wstring_view isoName(L"prime-nowalls.iso");
#else
    std::string_view isoName("prime-nowalls.iso");
#endif

    if (fs::exists(isoName))
        fs::remove(isoName);

    nod::DiscBuilderGCN builder(isoName, progFunc);

#if _WIN32 && UNICODE
    builder.buildFromDirectory(converter.from_bytes(outputDirectory));
#else
    builder.buildFromDirectory(outputDirectory);
#endif

    fs::remove_all(outputDirectory);

    std::cout << "\nDone." << std::endl;
    return 0;
}
