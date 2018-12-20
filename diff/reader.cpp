#include "reader.h"
#include "mainwindow.h"
#include "file_data.h"
#include "sha256.h"
#include "ui_mainwindow.h"

#include <QApplication>

bits::bits() :x(0), y(0), z(0) {}

bits::bits(char x, char y, char z) : x(x), y(y), z(z) {}

bool operator==(bits const& a, bits const& b) {
    return (a.x == b.x);

}
bool operator<(bits const& a, bits const& b) {
    return (a.x < b.x);
}

std::size_t number_of_files_in_directory(fs::path path)
{
    return std::distance(fs::recursive_directory_iterator(path), fs::recursive_directory_iterator{});
}

std::map<std::pair<size_t, bits>, std::vector<fs::path>> get(std::string directory, QProgressBar *bar, bool upd) {
    std::map<std::pair<size_t, bits>, std::vector<fs::path>> input;


    int cnt = 0;
    double add_progress = 100.0 / number_of_files_in_directory(directory) * 1.0;

    file_data& file_data = file_data.get_instance();
    if (!upd) {
        file_data._SHA256.clear();
    }

    bar->setValue(cnt * add_progress);

    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        try {
            fs::path path = entry.path();
            if (fs::is_directory(path) || fs::is_empty(path) || fs::is_block_file(path)) {
                continue;
            }

            if (!upd) {
                file_data._SHA256[path] = sha256_file(path);
            }
            size_t _size = fs::file_size(path);

            std::ifstream in(path, std::ios::in | std::ios::binary);

            char x, y, z;
            in.read((char *) &x, sizeof (x));
            in.seekg(_size / 2);
            in.read((char *) &y, sizeof (y));
            in.seekg(0, in.end);
            in.read((char *) &z, sizeof (z));

            bits bit(x, y, z);

            std::pair<size_t, bits> p(_size, bit);

            input[p].push_back(path);
        } catch (fs::filesystem_error e) {
            std::cout << e.what() << std::endl;
        }
        cnt++;
        bar->setValue(int(cnt * add_progress));
    }

    return input;
}


QStringList read(std::string directory, QProgressBar *bar, bool upd) {
    std::map<std::pair<size_t, bits>, std::vector<fs::path>> input = get(directory, bar, upd);
    QStringList List;
    file_data &file_data = file_data.get_instance();
    file_data._KEYS.clear();
    file_data._FILES = input;

    for (auto row : input) {
        std::pair<size_t, bits> key = row.first;
        for (auto file : row.second) {
            std::string size = std::to_string(fs::file_size(file));
            std::string name = file.filename();
            file_data._KEYS[name] = key;
            QString QName = QString::fromStdString(name);
            List << QName;
        }
    }

    return List;
}

QStringList read(QString directory, QProgressBar *bar, bool upd)
{
    std::string str = directory.toStdString();
    return read(str, bar, upd);
}

QStringList read_update(std::string directory, QProgressBar *bar)
{
    return read(directory, bar, true);
}

QStringList get_same(std::string name)
{
    file_data &file_data = file_data.get_instance();
    std::pair<size_t, bits> key = file_data._KEYS[name];
    auto row = file_data._FILES[key];

    fs::path item_file;
    for (auto it : row) {
        if (it.filename() == name) {
            item_file = it;
            break;
        }
    }


    std::string sha256 = file_data._SHA256[item_file];
    std::vector<fs::path> same;

    for (auto it : row) {
        if (it.filename() != name) {
            std::string sha256_it = file_data._SHA256[it];
            if (sha256 == sha256_it) {
                same.push_back(it);
            }
        }
    }

    QStringList List;

    for (auto it : same) {
        List << QString::fromStdString(it.filename());
    }

    return List;
}
